/*!
 * \file calibrator.cpp
 * \brief Remote antenna frequency shift calibration.
 * \author Anthony Arnold, 2015. anthony.arnold(at)uqconnect.edu.au
 *
 * This class uses code taken from GNSS-SDR <http://gnss-sdr.org/>
 * -------------------------------------------------------------------------
 *
 * Copyright (C) Anthony Arnold 2015
 *
 * Genesis is a realtime multi-station GNSS receiver.
 *
 * This file is part of Genesis.
 *
 * Genesis is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Genesis is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Genesis. If not, see <http://www.gnu.org/licenses/>.
 *
 * -------------------------------------------------------------------------
 */
#include <ctime>
#include <exception>
#include <memory>
#include <queue>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gnuradio/msg_queue.h>
#include <gnuradio/top_block.h>
#include <gnuradio/blocks/null_sink.h>
#include <gnuradio/blocks/skiphead.h>
#include <gnuradio/blocks/head.h>
#include <gnuradio/blocks/file_source.h>
#include <gnuradio/blocks/file_sink.h>
#include "concurrent_map.h"
#include "file_configuration.h"
#include "gps_l1_ca_pcps_acquisition_fine_doppler.h"
#include "gnss_signal.h"
#include "gnss_synchro.h"
#include "gnss_block_factory.h"
#include "gps_navigation_message.h"
#include "gps_ephemeris.h"
#include "gps_almanac.h"
#include "gps_iono.h"
#include "gps_utc_model.h"
#include "galileo_ephemeris.h"
#include "galileo_almanac.h"
#include "galileo_iono.h"
#include "galileo_utc_model.h"
#include "sbas_telemetry_data.h"
#include "sbas_ionospheric_correction.h"
#include "sbas_satellite_correction.h"
#include "sbas_ephemeris.h"
#include "sbas_time.h"
#include "gnss_sdr_supl_client.h"
#include "front_end_cal.h"

concurrent_queue<Gps_Ephemeris> global_gps_ephemeris_queue;
concurrent_queue<Gps_Iono> global_gps_iono_queue;
concurrent_queue<Gps_Utc_Model> global_gps_utc_model_queue;
concurrent_queue<Gps_Almanac> global_gps_almanac_queue;
concurrent_queue<Gps_Acq_Assist> global_gps_acq_assist_queue;

concurrent_map<Gps_Ephemeris> global_gps_ephemeris_map;
concurrent_map<Gps_Iono> global_gps_iono_map;
concurrent_map<Gps_Utc_Model> global_gps_utc_model_map;
concurrent_map<Gps_Almanac> global_gps_almanac_map;
concurrent_map<Gps_Acq_Assist> global_gps_acq_assist_map;

// For GALILEO NAVIGATION
concurrent_queue<Galileo_Ephemeris> global_galileo_ephemeris_queue;
concurrent_queue<Galileo_Iono> global_galileo_iono_queue;
concurrent_queue<Galileo_Utc_Model> global_galileo_utc_model_queue;
concurrent_queue<Galileo_Almanac> global_galileo_almanac_queue;

concurrent_map<Galileo_Ephemeris> global_galileo_ephemeris_map;
concurrent_map<Galileo_Iono> global_galileo_iono_map;
concurrent_map<Galileo_Utc_Model> global_galileo_utc_model_map;
concurrent_map<Galileo_Almanac> global_galileo_almanac_map;

// For SBAS CORRECTIONS
concurrent_queue<Sbas_Raw_Msg> global_sbas_raw_msg_queue;
concurrent_queue<Sbas_Ionosphere_Correction> global_sbas_iono_queue;
concurrent_queue<Sbas_Satellite_Correction> global_sbas_sat_corr_queue;
concurrent_queue<Sbas_Ephemeris> global_sbas_ephemeris_queue;

concurrent_map<Sbas_Ionosphere_Correction> global_sbas_iono_map;
concurrent_map<Sbas_Satellite_Correction> global_sbas_sat_corr_map;
concurrent_map<Sbas_Ephemeris> global_sbas_ephemeris_map;

bool stop;
concurrent_queue<int> channel_internal_queue;
GpsL1CaPcpsAcquisitionFineDoppler *acquisition;
Gnss_Synchro *gnss_synchro;
std::vector<Gnss_Synchro> gnss_sync_vector;

extern boost::filesystem::path GNSS_SDR_CONFIG_FILE;

namespace genesis {

typedef std::shared_ptr<ConfigurationInterface> config_ptr;

void wait_message()
{
   while (!stop)
   {
      int message;
      channel_internal_queue.wait_and_pop(message);
      //std::cout<<"Acq mesage rx="<<message<<std::endl;
      switch (message)
      {
      case 1: // Positive acq
	 gnss_sync_vector.push_back(*gnss_synchro);
	 //acquisition->reset();
	 break;
      case 2: // negative acq
	 //acquisition->reset();
	 break;
      case 3:
	 stop = true;
	 break;
      default:
	 break;
      }
   }
}

bool front_end_capture(std::shared_ptr<ConfigurationInterface> configuration) {
   gr::top_block_sptr top_block;
   GNSSBlockFactory block_factory;
   boost::shared_ptr<gr::msg_queue> queue;

   queue =  gr::msg_queue::make(0);
   top_block = gr::make_top_block("Acquisition test");

   std::shared_ptr<GNSSBlockInterface> source;
   source = block_factory.GetSignalSource(configuration, queue);

   std::shared_ptr<GNSSBlockInterface> conditioner =
      block_factory.GetSignalConditioner(configuration,queue);

   gr::block_sptr sink;
   sink = gr::blocks::file_sink::make(sizeof(gr_complex), "tmp_capture.dat");

   //--- Find number of samples per spreading code ---
   long fs_in_ = configuration->property("GNSS-SDR.internal_fs_hz", 2048000);
   int samples_per_code =
      round(fs_in_ / (GPS_L1_CA_CODE_RATE_HZ / GPS_L1_CA_CODE_LENGTH_CHIPS));
   int nsamples = samples_per_code * 50;

   int skip_samples = fs_in_ * 5; // skip 5 seconds

   gr::block_sptr head =
      gr::blocks::head::make(sizeof(gr_complex), nsamples);

   gr::block_sptr skiphead =
      gr::blocks::skiphead::make(sizeof(gr_complex), skip_samples);

   try
   {
      source->connect(top_block);
      conditioner->connect(top_block);
      top_block->connect(
	 source->get_right_block(), 0, conditioner->get_left_block(), 0);
      top_block->connect(conditioner->get_right_block(), 0, skiphead, 0);
      top_block->connect(skiphead, 0, head, 0);
      top_block->connect(head, 0, sink, 0);
      top_block->run();
   }
   catch(std::exception& e)
   {
      return false;
   }

   return true;
}

double calibrate (const std::string &address,
                  unsigned short port)
{
   FLAGS_log_dir = boost::filesystem::current_path ().native ();

   // Fetch samples
   // 0. Instantiate the FrontEnd Calibration class
   FrontEndCal front_end_cal;

   // 1. Load configuration parameters from config file
   config_ptr configuration = std::make_shared<FileConfiguration> (
      GNSS_SDR_CONFIG_FILE.c_str ());

   // Override properties

   // Set signal source to RTLTCP
   configuration->set_property (
      "SignalSource.implementation", "RtlTcp_Signal_Source");
   configuration->set_property (
      "SignalSource.address", address);
   configuration->set_property (
      "SignalSource.port", boost::lexical_cast<std::string> (port));

   // Set input filter IF to 0
   configuration->set_property (
      "InputFilter.IF", "0");

   // Use liberal acquisition settings
   // We expect the rtl dongle to be within these bounds
   configuration->set_property (
      "Acquisition.doppler_max", "100000");
   configuration->set_property (
      "Acquisition.doppler_min", "-100000");
   configuration->set_property (
      "Acquisition.max_dwells", "15");

   // 2. Get SUPL information from server: Ephemeris record, assistance info and TOW
   front_end_cal.set_configuration (configuration);
   front_end_cal.get_ephemeris();

   // 3. Capture some front-end samples to hard disk
   front_end_capture(configuration);

   // 4. Setup GNU Radio flowgraph (file_source -> Acquisition_10m)
   gr::top_block_sptr top_block;
   boost::shared_ptr<gr::msg_queue> queue;
   queue = gr::msg_queue::make(0);
   top_block = gr::make_top_block("Acquisition test");

   // Satellite signal definition
   gnss_synchro = new Gnss_Synchro();
   gnss_synchro->Channel_ID = 0;
   gnss_synchro->System = 'G';
   std::string signal = "1C";
   signal.copy(gnss_synchro->Signal, 2, 0);
   gnss_synchro->PRN = 1;

   long fs_in_ = configuration->property("GNSS-SDR.internal_fs_hz", 2048000);

   GNSSBlockFactory block_factory;
   acquisition =
      new GpsL1CaPcpsAcquisitionFineDoppler(
	 configuration.get(), "Acquisition", 1, 1, queue);

   acquisition->set_channel(1);
   acquisition->set_gnss_synchro(gnss_synchro);
   acquisition->set_channel_queue(&channel_internal_queue);
   acquisition->set_threshold(
      configuration->property("Acquisition.threshold", 0.0));
   acquisition->set_doppler_max(
      configuration->property("Acquisition.doppler_max", 10000));
   acquisition->set_doppler_step(
      configuration->property("Acquisition.doppler_step", 250));

   gr::block_sptr source;
   source = gr::blocks::file_source::make(sizeof(gr_complex), "tmp_capture.dat");

   try
   {
      acquisition->connect(top_block);
      top_block->connect(source, 0, acquisition->get_left_block(), 0);
   }
   catch(std::exception& e)
   {
   }

   // 5. Run the flowgraph
   // Get visible GPS satellites (positive acquisitions with Doppler measurements)
   // Compute Doppler estimations

   std::map<int,double> doppler_measurements_map;
   std::map<int,double> cn0_measurements_map;

   boost::thread ch_thread;

   for (unsigned int PRN=1; PRN<33; PRN++)
   {
      gnss_synchro->PRN = PRN;
      acquisition->set_gnss_synchro(gnss_synchro);
      acquisition->init();
      acquisition->reset();
      stop = false;
      ch_thread = boost::thread(wait_message);
      top_block->run();

      if (gnss_sync_vector.size()>0)
      {
	 double doppler_measurement_hz = 0;
	 for (std::vector<Gnss_Synchro>::iterator it = gnss_sync_vector.begin();
	      it != gnss_sync_vector.end();
	      ++it)
	 {
	    doppler_measurement_hz += (*it).Acq_doppler_hz;
	 }
	 doppler_measurement_hz = doppler_measurement_hz/gnss_sync_vector.size();
	 doppler_measurements_map.insert(std::pair<int,double>(PRN, doppler_measurement_hz));
      }
      channel_internal_queue.push(3);
      ch_thread.join();
      gnss_sync_vector.clear();
      boost::dynamic_pointer_cast<gr::blocks::file_source>(source)->seek(0, 0);
      std::cout.flush();
   }

   //6. find TOW from SUPL assistance
   double current_TOW = 0;
   if (global_gps_ephemeris_map.size() > 0)
   {
      std::map<int,Gps_Ephemeris> Eph_map;
      Eph_map = global_gps_ephemeris_map.get_map_copy();
      current_TOW = Eph_map.begin()->second.d_TOW;
   }
   else
   {
      delete acquisition;
      delete gnss_synchro;
      google::ShutDownCommandLineFlags();
      return 0;
   }

   //Get user position from config file (or from SUPL using GSM Cell ID)
   double lat_deg = configuration->property("GNSS-SDR.init_latitude_deg", 41.0);
   double lon_deg = configuration->property("GNSS-SDR.init_longitude_deg", 2.0);
   double altitude_m = configuration->property("GNSS-SDR.init_altitude_m", 100);

   if (doppler_measurements_map.size() == 0)
   {
      delete acquisition;
      delete gnss_synchro;
      google::ShutDownCommandLineFlags();
      return 0;
   }

   std::map<int,double> f_if_estimation_Hz_map;
   std::map<int,double> f_fs_estimation_Hz_map;
   std::map<int,double> f_ppm_estimation_Hz_map;

   for (std::map<int,double>::iterator it = doppler_measurements_map.begin();
	it != doppler_measurements_map.end();
	++it)
   {
      try
      {
	 double doppler_estimated_hz;
	 doppler_estimated_hz =
	    front_end_cal.estimate_doppler_from_eph(it->first,
						    current_TOW,
						    lat_deg,
						    lon_deg,
						    altitude_m);

	 // 7. Compute front-end IF and sampling frequency estimation
	 // Compare with the measurements and compute clock drift using FE model
	 double estimated_fs_Hz, estimated_f_if_Hz, f_osc_err_ppm;
	 front_end_cal.GPS_L1_front_end_model_E4000(doppler_estimated_hz,
						    it->second,fs_in_,
						    &estimated_fs_Hz,
						    &estimated_f_if_Hz,
						    &f_osc_err_ppm);

	 f_if_estimation_Hz_map.insert(std::pair<int,double>(it->first,estimated_f_if_Hz));
	 f_fs_estimation_Hz_map.insert(std::pair<int,double>(it->first,estimated_fs_Hz));
	 f_ppm_estimation_Hz_map.insert(std::pair<int,double>(it->first,f_osc_err_ppm));
      }
      catch(int ex)
      {
      }
   }

   // FINAL FE estimations
   double mean_f_if_Hz = 0;
   double mean_fs_Hz = 0;
   double mean_osc_err_ppm = 0;
   int n_elements = f_if_estimation_Hz_map.size();

   for (std::map<int,double>::iterator it =  f_if_estimation_Hz_map.begin();
	it != f_if_estimation_Hz_map.end();
	++it)
   {
      mean_f_if_Hz += (*it).second;
      mean_fs_Hz += f_fs_estimation_Hz_map.find((*it).first)->second;
      mean_osc_err_ppm += f_ppm_estimation_Hz_map.find((*it).first)->second;
   }

   mean_f_if_Hz /= n_elements;
   //mean_fs_Hz /= n_elements;
   //mean_osc_err_ppm /= n_elements;

   delete acquisition;
   delete gnss_synchro;

   return mean_f_if_Hz;
}

}
