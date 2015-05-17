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
DEFINE_string (socket_file,
               "/var/run/genesis.socket",
               "The domain socket to open");
DEFINE_string (listen_address,
               "0.0.0.0",
               "The address to listen to pings from (can be multicast).");

#ifdef GENESIS_DEBUG
#define VERY_VERBOSE true
#else
#define VERY_VERBOSE false
#endif

DEFINE_bool (verbose,
             false,
             "Verbose output");
DEFINE_bool (very_verbose,
             VERY_VERBOSE,
             "Very verbose output");

fs::path GNSS_SDR_CONFIG_FILE,
   FRONT_END_CAL_CONFIG_FILE,
   GNSS_SDR_EXECUTABLE,
   FRONT_END_CAL_EXECUTABLE;

genesis::logger lg;

static void check_path (fs::path &dest,
                        const std::string &path,
                        const std::string &desc)
{
    boost::system::error_code ec;
    dest = fs::canonical (path, ec);
    if (ec) {
        BOOST_LOG_SEV (lg, genesis::critical)
           << "Cannot open " << desc << " " << path
           << ": " << ec.message ();
        ::exit (1);
    }
    BOOST_LOG_SEV (lg, genesis::debug)
       << "Using "
       << dest
       << " for " << desc;
}

int main (int argc, char *argv[]) {
  const std::string intro_help
    ("Copyright (C) Anthony Arnold 2015.\n"
     "Genesis is a realtime multi-station GNSS receiver.\n"
     "This program comes with ABSOLUTELY NO WARRANTY\n"
     "See LICENSE file to see a copy of the General Public License\n\n");

  google::SetUsageMessage(intro_help);
  google::ParseCommandLineFlags(&argc, &argv, true);

  genesis::init_logging ();

  check_path (GNSS_SDR_CONFIG_FILE,
              FLAGS_config_file,
              "gnss-sdr config");

  check_path (FRONT_END_CAL_CONFIG_FILE,
              FLAGS_cal_config_file,
              "front-end-cal config");

  check_path (GNSS_SDR_EXECUTABLE,
              FLAGS_gnss_sdr,
              "gnss-sdr executable");

  check_path (FRONT_END_CAL_EXECUTABLE,
              FLAGS_front_end_cal,
              "front-end-cal executable");

  // Start the service
  genesis::service service;

  boost::system::error_condition ec = service.run (FLAGS_socket_file,
                                                   FLAGS_listen_address);
  if (ec) {
    BOOST_LOG_SEV (lg, genesis::critical) << "Failed to run: " << ec.message();
  }
}
