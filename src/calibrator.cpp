/*!
 * \file calibrator.cpp
 * \brief Routines for a front end calibration service.
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
#include <boost/algorithm/string/replace.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include "calibrator.hpp"
#include "error.hpp"
#include "fork.hpp"
#include <fstream>
#include "log.hpp"
#include <sstream>
#include "station.hpp"
#include "station_config.hpp"
#include <unistd.h>

namespace fs = boost::filesystem;

extern fs::path FRONT_END_CAL_EXECUTABLE;
extern fs::path FRONT_END_CAL_CONFIG_FILE;

static const fs::path STATION_CONFIG_FILE = "station_config";

namespace genesis {

namespace detail {

// Load the bias from a saved file
static bool load_bias (const fs::path &subdir, double &bias) {
   fs::path file = subdir;
   file /= STATION_CONFIG_FILE;

   station_config cfg;
   bool result = load_station_config (file, cfg);
   if (result) {
       bias = cfg.if_bias ();
   }
   return result;
}

// Save the bias to a file
static bool save_bias (const fs::path &subdir, double bias) {
   fs::path file = subdir;
   file /= STATION_CONFIG_FILE;

   station_config cfg (bias);
   return save_station_config (file, cfg);
}

// Write the INI file to the local directory
static calibrator::error_type write_config (
   const station &st,
   const fs::path &path)
{
   std::ifstream ifs (FRONT_END_CAL_CONFIG_FILE.c_str (), std::ios::binary);
   if (!ifs) {
      return make_error_condition (file_not_found);
   }
   std::ofstream ofs (path.c_str (), std::ios::binary);
   if (!ofs) {
      return make_error_condition (file_not_found);
   }

   ofs << ifs.rdbuf ();

   // Writing these at the end will override previous definitions
   ofs << std::endl << "SignalSource.address=" << st.get_address () << std::endl;
   ofs << "SignalSource.port=" << st.get_port () << std::endl;
   return calibrator::error_type ();
}

} // detail

enum {
    BUFFER_SIZE
};

struct calibrator::impl {
   impl (boost::asio::io_service &io_service)
       : timer_ (io_service),
         stream_ (io_service),
         buffer_ (BUFFER_SIZE),
         IF_ (0)
      {
      }

   boost::asio::deadline_timer timer_;
   boost::asio::posix::stream_descriptor stream_;
   boost::asio::streambuf buffer_;
   double IF_;
   error_type read_error_;
   logger lg_;
};

calibrator::calibrator (boost::asio::io_service &io_service)
    : impl_ (new impl (io_service))
   {
   }

calibrator::error_type calibrator::read_if (int fd) {
   static const boost::regex expression (
      "IF bias present in baseband\\=([0-9]+\\.[0-9]*) \\[Hz\\]");

   boost::system::error_code ec;
   impl_->IF_ = 0;

   // Use asio stream
   impl_->read_error_ = make_error_condition (if_bias_not_found);
   impl_->stream_.assign (fd);

   // 2 minutes to run
   impl_->timer_.expires_from_now (boost::posix_time::minutes (2));

   boost::asio::async_read_until (
       impl_->stream_, impl_->buffer_, '\n',
       boost::bind (&calibrator::handle_read,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));

   boost::system::error_code timer_error;
   impl_->timer_.wait (timer_error);
   impl_->stream_.cancel ();
   impl_->stream_.close ();
   return impl_->read_error_;
}

void calibrator::handle_read (boost::system::error_code ec, size_t len) {
   static const boost::regex expression (
      "IF bias present in baseband\\=([0-9]+\\.[0-9]*) \\[Hz\\]");
   if (len == 0) {
       return;
   }

    if (ec && ec != boost::asio::error::not_found) {
        BOOST_LOG_SEV (impl_->lg_, error) << "Error reading from front-end-cal: "
                                   << ec.message ();
        impl_->read_error_ = to_error_condition (ec);
        impl_->timer_.cancel ();
        return;
    }

    char data[BUFFER_SIZE];
    impl_->buffer_.sgetn (data, len);

    const char *begin = &data[0];
    const char *end = &data[len];

    // Search for the output which states the IF bias
    boost::match_results <const char *> what;
    boost::match_flag_type flags = boost::match_default;
    if (boost::regex_search (begin, end, what, expression, flags)) {
        BOOST_LOG_SEV (impl_->lg_, debug)
           << "Found IF bias of " << what[1];
        impl_->IF_ = boost::lexical_cast <double> (what[1]);
        impl_->read_error_ = error_type ();
        impl_->timer_.cancel ();
    }
    else {
        // Keep reading
        boost::asio::async_read_until (
            impl_->stream_, impl_->buffer_, '\n',
            boost::bind (&calibrator::handle_read,
                         this,
                         boost::asio::placeholders::error,
                         boost::asio::placeholders::bytes_transferred));
    }

}

calibrator::error_type calibrator::calibrate (const station &st,
					      fork_handler *handler)
{
   boost::system::error_code ec;
   fs::path path (boost::algorithm::replace_all_copy (
                     st.get_address (), ":", "."));
   if (!fs::exists (path)) {
      if (!fs::create_directory (st.get_address (), ec)) {
         return to_error_condition (ec);
      }
   }

   // Look for existing calibration
   if (detail::load_bias (path, impl_->IF_)) {
      BOOST_LOG_SEV (impl_->lg_, debug)
         << "IF bias for "
         << st.get_address ()
         << " loaded from "
         << path.native ();
      return error_type ();
   }

   // Write configuration
   fs::path config_file = path;
   config_file /= "front-end-cal.conf";
   error_type et = detail::write_config (st, config_file);
   if (et) {
      BOOST_LOG_SEV (impl_->lg_, error) << "Failed to write config file "
                                 << "for station "
                                 << st.get_address ();
      return et;
   }

   // Execute front-end-cal
   std::vector<std::string> args;
   args.push_back ("front-end-cal");
   args.push_back ("--config_file");
   args.push_back ("front-end-cal.conf");
   args.push_back ("-log_dir=./");
   int fd = genesis::fork (handler,
                           path,
                           FRONT_END_CAL_EXECUTABLE,
                           args);

   // In the parent - read the output from front-end-cal
   et = read_if (fd);
   if (!et) {
      BOOST_LOG_SEV (impl_->lg_, debug) << "Saving IF bias";
      if (!detail::save_bias (path, impl_->IF_)) {
         BOOST_LOG_SEV (impl_->lg_, warning) << "Saving IF bias failed.";
      }
   }
   return et;
}

double calibrator::get_IF () const {
    return impl_->IF_;
}
}
