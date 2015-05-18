/*!
 * \file gnss_sdr.hpp
 * \brief For running GNSS-SDR.
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

#include "gnss_sdr.hpp"
#include "fork.hpp"
#include "station.hpp"
#include "log.hpp"
#include <boost/filesystem.hpp>
#include <gflags/gflags.h>
#include <fstream>
#include <boost/algorithm/string/replace.hpp>

DECLARE_string (socket_file);

namespace fs = boost::filesystem;

extern fs::path GNSS_SDR_EXECUTABLE;
extern fs::path GNSS_SDR_CONFIG_FILE;

namespace genesis {

namespace detail {

// Write the INI file to the local directory
static gnss_sdr::error_type write_config (const station &st,
                                          const fs::path &path,
                                          double bias)
{
    std::ifstream ifs (GNSS_SDR_CONFIG_FILE.c_str (), std::ios::binary);
    if (!ifs) {
        return make_error_condition (file_not_found);
    }
    std::ofstream ofs (path.c_str (), std::ios::binary);
    if (!ofs) {
        return make_error_condition (file_not_found);
    }

    ofs << ifs.rdbuf ();

    // Writing these at the end will override previous definitions
    fs::path socket_file = FLAGS_socket_file;
    ofs << std::endl << "SignalSource.address=" << st.get_address () << std::endl;
    ofs << "SignalSource.port=" << st.get_port () << std::endl;
    ofs << "InputFilter.IF=" << bias << std::endl;
    if (socket_file.is_absolute ()) {
        ofs << "OutputFilter.filename=" << socket_file.c_str () << std::endl;
    }
    else {
        // relative from run directory
        ofs << "OutputFilter.filename=../" << socket_file.c_str () << std::endl;
    }
    return gnss_sdr::error_type ();
}

} // namespace detail


gnss_sdr::error_type gnss_sdr::run (const station &st,
                                    fork_handler *handler,
                                    int &out,
                                    double bias)
{
    logger lg;

    boost::system::error_code ec;
    fs::path path (boost::algorithm::replace_all_copy (
                       st.get_address (), ":", "."));
    if (!fs::exists (path)) {
        if (!fs::create_directory (st.get_address (), ec)) {
            return to_error_condition (ec);
        }
    }

    // Write configuration
    fs::path config_file = path;
    config_file /= "gnss-sdr.conf";
    error_type et = detail::write_config (st, config_file, bias);
    if (et) {
        BOOST_LOG_SEV (lg, error) << "Failed to write config file "
                                          << "for station "
                                          << st.get_address ();
        return et;
    }

    // Execute gnss-sdr
   BOOST_LOG_SEV (lg, trace) << "Starting gnss-sdr";
   std::vector<std::string> args;
   args.push_back ("gnss-sdr");
   args.push_back ("--config_file");
   args.push_back ("gnss-sdr.conf");
   args.push_back ("-log_dir=./");
   out = genesis::fork (handler,
                        path,
                        GNSS_SDR_EXECUTABLE,
                        args);

   BOOST_LOG_SEV (lg, trace) << "gnss-sdr started";


   return et;
}

}
