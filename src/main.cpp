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

DEFINE_string (gnss_sdr_config_file,
	       "./gnss-sdr.conf",
	       "The GNSS-SDR configuration file to use.");

boost::filesystem::path GNSS_SDR_CONFIG_FILE;

int main (int argc, char *argv[]) {
   const std::string intro_help(
      "Copyright (C) Anthony Arnold 2015.\n"
"Genesis is a realtime multi-station GNSS receiver.\n"
"This program comes with ABSOLUTELY NO WARRANTY\n"
"See LICENSE file to see a copy of the General Public License\n\n");

    google::SetUsageMessage(intro_help);
    google::ParseCommandLineFlags(&argc, &argv, true);

    genesis::init_logging ();
    genesis::logger lg;

    GNSS_SDR_CONFIG_FILE =
       boost::filesystem::absolute (FLAGS_gnss_sdr_config_file);
    BOOST_LOG_SEV (lg, genesis::debug)
       << "Using "
       << GNSS_SDR_CONFIG_FILE
       << " for gnss-sdr config";

    genesis::service service;

    boost::system::error_condition ec = service.run ("./genesis.socket",
                                                     "239.255.255.1");
    if (ec) {
       BOOST_LOG_SEV (lg, genesis::critical) << "Failed to run: " << ec.message();
    }
}
