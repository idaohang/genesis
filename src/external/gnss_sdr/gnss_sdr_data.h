/*!
 * \file gnss_sdr_data.h
 * \brief  A structure serializable with Boost.Serialization, which contains
 * the data from a gnss_synchro object. This file requires nothing except
 * for Boost.Serialization so it can be copied to other projects which wish
 * to consume gnss_synchro data.
 * \author Anthony Arnold 2015. anthony.arnold(at)uqconnect.edu.au
 *
 * -------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2015  (see AUTHORS file for a list of contributors)
 *
 * GNSS-SDR is a software defined Global Navigation
 *          Satellite Systems receiver
 *
 * This file is part of GNSS-SDR.
 *
 * GNSS-SDR is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GNSS-SDR is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNSS-SDR. If not, see <http://www.gnu.org/licenses/>.
 *
 * -------------------------------------------------------------------------
 */

#ifndef GNSS_SDR_GNSS_SDR_DATA_H_
#define GNSS_SDR_GNSS_SDR_DATA_H_

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/is_bitwise_serializable.hpp>

/*!
 * \brief Serializable structure that contains the information
 * from GNSS-SDR's \ref Gnss_Synchro structure.
 */
struct gnss_sdr_data {
   // Satellite and signal info
   char System;
   char Signal[3];
   unsigned int PRN;
   int Channel_ID;

   // Acquisition
   double Acq_delay_samples;
   double Acq_doppler_hz;
   unsigned long int Acq_samplestamp_samples;
   bool Flag_valid_acquisition;

   //Tracking
   double Prompt_I;
   double Prompt_Q;
   double CN0_dB_hz;
   double Carrier_Doppler_hz;
   double Carrier_phase_rads;
   double Code_phase_secs;
   double Tracking_timestamp_secs;
   bool Flag_valid_tracking;

   //Telemetry Decoder
   double Prn_timestamp_ms;
   double Prn_timestamp_at_preamble_ms;

   bool Flag_valid_word;
   bool Flag_preamble;
   double d_TOW;
   double d_TOW_at_current_symbol;
   double d_TOW_hybrid_at_current_symbol;
   double Pseudorange_m;
   bool Flag_valid_pseudorange;

   template <typename Archive>
   void serialize (Archive &ar, const unsigned int /*version*/) {
       // Satellite and signal info
       ar & System;
       ar & Signal;
       ar & PRN;
       ar & Channel_ID;

       // Acquisition
       ar & Acq_delay_samples;
       ar & Acq_doppler_hz;
       ar & Acq_samplestamp_samples;
       ar & Flag_valid_acquisition;

       //Tracking
       ar & Prompt_I;
       ar & Prompt_Q;
       ar & CN0_dB_hz;
       ar & Carrier_Doppler_hz;
       ar & Carrier_phase_rads;
       ar & Code_phase_secs;
       ar & Tracking_timestamp_secs;
       ar & Flag_valid_tracking;

       //Telemetry Decoder
       ar & Prn_timestamp_ms;
       ar & Prn_timestamp_at_preamble_ms;

       ar & Flag_valid_word;
       ar & Flag_preamble;
       ar & d_TOW;
       ar & d_TOW_at_current_symbol;
       ar & d_TOW_hybrid_at_current_symbol;
       ar & Pseudorange_m;
       ar & Flag_valid_pseudorange;
   }

};
BOOST_IS_BITWISE_SERIALIZABLE(gnss_sdr_data)

#endif /*GNSS_SDR_GNSS_SDR_DATA_H_*/
