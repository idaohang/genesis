/*!
 * \file main.cpp
 * \brief Genesis main line.
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

#include "log.hpp"
#include "service.hpp"
#include <gflags/gflags.h>
#include <string>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

DEFINE_string (config_file,
               "/usr/local/share/gnss-sdr/conf/gnss-sdr.conf",
               "The GNSS-SDR configuration file to use.");
DEFINE_string (cal_config_file,
               "/usr/local/share/gnss-sdr/conf/front-end-cal.conf",
               "The front-end-cal configuration file to use.");
DEFINE_string (gnss_sdr,
               "/usr/local/bin/gnss-sdr",
               "The gnss-sdr executable");
DEFINE_string (front_end_cal,
               "/usr/local/bin/front-end-cal",
               "The front-end-cal executable");

fs::path GNSS_SDR_CONFIG_FILE,
   FRONT_END_CAL_CONFIG_FILE,
   GNSS_SDR_EXECUTABLE,
   FRONT_END_CAL_EXECUTABLE;

int main (int argc, char *argv[]) {
  const std::string intro_help
    ("Copyright (C) Anthony Arnold 2015.\n"
     "Genesis is a realtime multi-station GNSS receiver.\n"
     "This program comes with ABSOLUTELY NO WARRANTY\n"
     "See LICENSE file to see a copy of the General Public License\n\n");

  google::SetUsageMessage(intro_help);
  google::ParseCommandLineFlags(&argc, &argv, true);

  genesis::init_logging ();
  genesis::logger lg;

  GNSS_SDR_CONFIG_FILE = fs::canonical (FLAGS_config_file);
  BOOST_LOG_SEV (lg, genesis::debug)
    << "Using "
    << GNSS_SDR_CONFIG_FILE
    << " for gnss-sdr config";

  if (!fs::exists (GNSS_SDR_CONFIG_FILE)) {
    BOOST_LOG_SEV (lg, genesis::critical) << "Config file not found";
    return 1;
  }


  FRONT_END_CAL_CONFIG_FILE = fs::canonical (FLAGS_cal_config_file);
  BOOST_LOG_SEV (lg, genesis::debug)
    << "Using "
    << FRONT_END_CAL_CONFIG_FILE
    << " for front-end-cal config";

  if (!fs::exists (FRONT_END_CAL_CONFIG_FILE)) {
    BOOST_LOG_SEV (lg, genesis::critical) << "Cal config file not found";
    return 1;
  }

  GNSS_SDR_EXECUTABLE = fs::canonical (FLAGS_gnss_sdr);
  BOOST_LOG_SEV (lg, genesis::debug)
    << "Using "
    << GNSS_SDR_EXECUTABLE
    << " for gnss-sdr executable";

  if (!fs::exists (GNSS_SDR_EXECUTABLE)) {
    BOOST_LOG_SEV (lg, genesis::critical) << "gnss-sdr not found";
    return 1;
  }

  FRONT_END_CAL_EXECUTABLE = fs::canonical (FLAGS_front_end_cal);
  BOOST_LOG_SEV (lg, genesis::debug)
    << "Using "
    << FRONT_END_CAL_EXECUTABLE
    << " for front-end-cal executable";

  if (!fs::exists (FRONT_END_CAL_EXECUTABLE)) {
    BOOST_LOG_SEV (lg, genesis::critical) << "front-end-cal not found";
    return 1;
  }

  // Start the service
  genesis::service service;

  boost::system::error_condition ec = service.run ("./genesis.socket",
                                                   "239.255.255.1");
  if (ec) {
    BOOST_LOG_SEV (lg, genesis::critical) << "Failed to run: " << ec.message();
  }
}
