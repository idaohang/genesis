/*!
 * \file gnss_sdr_globals.hpp
 * \brief Global objects required for running GNSS-SDR.
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
#ifndef GENESIS_GNSS_SDR_GLOBALS_HPP
#define GENESIS_GNSS_SDR_GLOBALS_HPP

#include "gnss_sdr_supl_client.h"
#include "concurrent_map.h"
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
#include "gps_navigation_message.h"
#include "gps_ephemeris.h"
#include "gps_almanac.h"
#include "gps_iono.h"

#ifdef GNSS_SDR_GLOBALS_CPP
#define GNSS_SDR_GLOBALS_EXTERN
#else
#define GNSS_SDR_GLOBALS_EXTERN extern
#endif

GNSS_SDR_GLOBALS_EXTERN concurrent_queue<Gps_Ephemeris> global_gps_ephemeris_queue;
GNSS_SDR_GLOBALS_EXTERN concurrent_queue<Gps_Iono> global_gps_iono_queue;
GNSS_SDR_GLOBALS_EXTERN concurrent_queue<Gps_Utc_Model> global_gps_utc_model_queue;
GNSS_SDR_GLOBALS_EXTERN concurrent_queue<Gps_Almanac> global_gps_almanac_queue;
GNSS_SDR_GLOBALS_EXTERN concurrent_queue<Gps_Acq_Assist> global_gps_acq_assist_queue;

GNSS_SDR_GLOBALS_EXTERN concurrent_map<Gps_Ephemeris> global_gps_ephemeris_map;
GNSS_SDR_GLOBALS_EXTERN concurrent_map<Gps_Iono> global_gps_iono_map;
GNSS_SDR_GLOBALS_EXTERN concurrent_map<Gps_Utc_Model> global_gps_utc_model_map;
GNSS_SDR_GLOBALS_EXTERN concurrent_map<Gps_Almanac> global_gps_almanac_map;
GNSS_SDR_GLOBALS_EXTERN concurrent_map<Gps_Acq_Assist> global_gps_acq_assist_map;

// For GALILEO NAVIGATION
GNSS_SDR_GLOBALS_EXTERN concurrent_queue<Galileo_Ephemeris> global_galileo_ephemeris_queue;
GNSS_SDR_GLOBALS_EXTERN concurrent_queue<Galileo_Iono> global_galileo_iono_queue;
GNSS_SDR_GLOBALS_EXTERN concurrent_queue<Galileo_Utc_Model> global_galileo_utc_model_queue;
GNSS_SDR_GLOBALS_EXTERN concurrent_queue<Galileo_Almanac> global_galileo_almanac_queue;

GNSS_SDR_GLOBALS_EXTERN concurrent_map<Galileo_Ephemeris> global_galileo_ephemeris_map;
GNSS_SDR_GLOBALS_EXTERN concurrent_map<Galileo_Iono> global_galileo_iono_map;
GNSS_SDR_GLOBALS_EXTERN concurrent_map<Galileo_Utc_Model> global_galileo_utc_model_map;
GNSS_SDR_GLOBALS_EXTERN concurrent_map<Galileo_Almanac> global_galileo_almanac_map;

// For SBAS CORRECTIONS
GNSS_SDR_GLOBALS_EXTERN concurrent_queue<Sbas_Raw_Msg> global_sbas_raw_msg_queue;
GNSS_SDR_GLOBALS_EXTERN concurrent_queue<Sbas_Ionosphere_Correction> global_sbas_iono_queue;
GNSS_SDR_GLOBALS_EXTERN concurrent_queue<Sbas_Satellite_Correction> global_sbas_sat_corr_queue;
GNSS_SDR_GLOBALS_EXTERN concurrent_queue<Sbas_Ephemeris> global_sbas_ephemeris_queue;

GNSS_SDR_GLOBALS_EXTERN concurrent_map<Sbas_Ionosphere_Correction> global_sbas_iono_map;
GNSS_SDR_GLOBALS_EXTERN concurrent_map<Sbas_Satellite_Correction> global_sbas_sat_corr_map;
GNSS_SDR_GLOBALS_EXTERN concurrent_map<Sbas_Ephemeris> global_sbas_ephemeris_map;

#endif // GENESIS_GNSS_SDR_GLOBALS_HPP
