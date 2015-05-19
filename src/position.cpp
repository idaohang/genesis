/*!
 * \file position.cpp
 * \brief Definition for performing RTK positioning.
 * \author Anthony Arnold, 2015. anthony.arnold(at)uqconnect.edu.au
 *
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

#include <map>
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/move/core.hpp>
#include "position.hpp"
#include "rtklib.h"
#include "log.hpp"
#include "gps_data.hpp"
#include "client_controller.hpp"

#define TWO_PI 6.28318530718

namespace genesis {

namespace detail {

void get_obs (const std::vector <gnss_sdr_data> &observables,
	       bool base,
	       const Gps_Ref_Time &ref_time,
	       std::vector <obsd_t> &out)
{
   std::map <unsigned int, obsd_t> rtkobs;
   typedef std::map<unsigned int, obsd_t>::value_type obs_pair;

   BOOST_FOREACH (const gnss_sdr_data &data, observables) {
      obsd_t &o = rtkobs[data.PRN];
      o.time.time = ref_time.d_tv_sec;
      o.time.sec = ref_time.d_tv_usec / 1000000.0;
      o.sat = (unsigned char)data.PRN;
      o.rcv = base ? 2 : 1;
      o.code [0] = CODE_L1C;
      o.L[0] = data.Carrier_phase_rads / TWO_PI; // radians to cycles
      o.P[0] = data.Pseudorange_m;
      o.D[0] = data.Carrier_Doppler_hz;
   }

   BOOST_FOREACH (const obs_pair pair, rtkobs) {
      out.push_back (pair.second);
   }
}

} // namespace detail

extern boost::mutex GLOBAL_BASE_STATION_MUTEX;
extern std::vector <gnss_sdr_data> GLOBAL_BASE_STATION_OBSERVABLES;
extern boost::shared_ptr <concurrent_dictionary <
   Gps_Ref_Time>> GLOBAL_BASE_STATION_REF_TIME;

position::position (controller_ptr controller, gps_data_ptr gps)
   : controller_ (controller), gps_data_ (gps)
{
}

position::error_type position::rtk_position (
   const std::vector <gnss_sdr_data> &observables)
{
   if (!controller_->has_base ()) {
      return make_error_condition (no_base_station);
   }

   struct autofree {
      rtk_t &rtk_;

      autofree (rtk_t &r)
	 : rtk_ (r)
      {
      }

      ~autofree () {
	 rtkfree (&rtk_);
      }
   };

   rtk_t rtk;
   prcopt_t options = prcopt_default;

   // TODO: Allow more options
   options.mode = PMODE_FIXED; // Fixed base station
   options.nf = 1; // GPS L1

   rtkinit (&rtk, &options);
   autofree freeme (rtk);

   // observation data
   // get_obs pushes in PRN order
   // rtklib requires in order of receiver, followed by satellite
   std::vector <obsd_t> observations;
   Gps_Ref_Time ref_time;
   GLOBAL_BASE_STATION_REF_TIME->read (0, ref_time);
   {
      boost::unique_lock<boost::mutex> lck (GLOBAL_BASE_STATION_MUTEX);
      detail::get_obs (GLOBAL_BASE_STATION_OBSERVABLES, true, ref_time, observations);
   }

   gps_data_->ref_time->read (0, ref_time);
   detail::get_obs (observables, false, ref_time, observations);

   // set up navigation data
   nav_t nav;
   memset (&nav, 0, sizeof (nav));

   std::vector <eph_t> ephemeris;
   std::map <int, Gps_Ephemeris> ephms = gps_data_->ephemeris->get_map_copy ();

   typedef  std::map <int, Gps_Ephemeris>::value_type eph_pair;
   BOOST_FOREACH (const eph_pair &e, ephms) {
      eph_t eph;
      const Gps_Ephemeris &dat = e.second;

      eph.sat = dat.i_satellite_PRN;
      eph.iodc = dat.d_IODC;
      eph.iode = dat.d_IODE_SF2;
      eph.sva = dat.i_SV_accuracy;
      eph.svh = dat.i_SV_health;
      eph.week = dat.i_GPS_week;
      eph.code = dat.i_code_on_L2;
      eph.flag = (int)dat.b_L2_P_data_flag;
      eph.toe.time = dat.d_Toe;
      eph.toe.sec = (dat.d_Toe - (time_t)dat.d_Toe) / 1000000.0;
      eph.toc.time = dat.d_Toc;
      eph.toc.sec = (dat.d_Toc - (time_t)dat.d_Toc) / 1000000.0;
      eph.ttr.time = dat.d_dtr;
      eph.ttr.sec = (dat.d_dtr - (time_t)dat.d_dtr) / 1000000.0;

      // Orbital parameters
      eph.A = (dat.d_sqrt_A * dat.d_sqrt_A);
      eph.e = dat.d_e_eccentricity;
      eph.i0 = dat.d_i_0;
      eph.OMG0 = dat.d_OMEGA0;
      eph.omg = dat.d_OMEGA;
      eph.M0 = dat.d_M_0;
      eph.deln = dat.d_Delta_n;
      eph.OMGd = dat.d_OMEGA_DOT;
      eph.idot = dat.d_IDOT;

      eph.crc = dat.d_Crc;
      eph.cic = dat.d_Cic;
      eph.cis = dat.d_Cis;
      eph.cus = dat.d_Cus;
      eph.crs = dat.d_Crs;
      eph.cuc = dat.d_Cuc;

      eph.toes = dat.d_TOW;
      eph.fit = dat.b_fit_interval_flag;
      eph.f0 = dat.d_A_f0;
      eph.f1 = dat.d_A_f1;
      eph.f2 = dat.d_A_f2;

      eph.tgd[0] = dat.d_TGD;

      ephemeris.emplace_back (boost::move (eph));
   }

   std::vector <alm_t> almanac;
   std::map <int, Gps_Almanac> alms = gps_data_->almanac->get_map_copy ();


   typedef  std::map <int, Gps_Almanac>::value_type alm_pair;
   BOOST_FOREACH (const alm_pair &a, alms) {
      alm_t alm;
      const Gps_Almanac &dat = a.second;

      alm.sat = dat.i_satellite_PRN;
      alm.svh = dat.i_SV_health;
      alm.svconf = 0; // fixme
      alm.week = ref_time.d_Week;
      alm.toa.time = dat.d_Toa;
      alm.toa.sec = (dat.d_Toa - (time_t)dat.d_Toa) / 1000000.0;
      alm.A = (dat.d_sqrt_A * dat.d_sqrt_A);
      alm.e = dat.d_e_eccentricity;
      alm.i0 = 0; // fixme
      alm.OMG0 = dat.d_OMEGA0;
      alm.omg = dat.d_OMEGA;
      alm.M0 = dat.d_M_0;
      alm.OMGd = dat.d_OMEGA_DOT;
      alm.toas = dat.d_Toa;
      alm.f0 = dat.d_A_f0;
      alm.f1 = dat.d_A_f1;

      almanac.emplace_back (boost::move (alm));
   }

   return error_type ();
}

}
