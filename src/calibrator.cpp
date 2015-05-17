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
#include <boost/optional.hpp>

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

struct reader {
   enum {
       BUFFER_SIZE = 1024
   };

   typedef calibrator::error_type error_type;

   typedef boost::optional<boost::system::error_code> opt_ec;

   reader ()
       : stream_ (service_),
         buffer_ (BUFFER_SIZE)
      {
      }

   double read (int fd) {
       boost::system::error_code ec;
       val_ = 0;

       // Use asio stream
       BOOST_LOG_SEV (lg_, trace) << "Opening stdout on front-end-cal";
       read_error_ = make_error_condition (if_bias_not_found);
       stream_.assign (fd, ec);
       if (ec) {
           BOOST_LOG_SEV (lg_, error)
              << "Failed to assign stream to Boost.Asio container: "
              << ec.message ();
       }

       // 2 minutes to run
       BOOST_LOG_SEV (lg_, trace) << "Setting 2 minute timeout";
       boost::asio::deadline_timer timer (service_);
       timer.expires_from_now (boost::posix_time::minutes (2));

       // Read asynchronously
       opt_ec read_result;
       BOOST_LOG_SEV (lg_, trace) << "Reading";
       read_next (&read_result);

       // Wait asynchronously
       opt_ec timer_result;
       timer.async_wait (
           boost::bind (&reader::handle_timeout, this,
                        &timer_result,
                        boost::asio::placeholders::error));

       // Wait for the read or the time to end
       service_.reset ();
       while (service_.run_one ()) {
           if (read_result) {
               timer.cancel ();
           }
           else if (timer_result) {
               stream_.cancel ();
           }
       }

       return val_;
   }

   error_type get_read_error () const {
       return read_error_;
   }

private:
   void handle_timeout (opt_ec *errorp, const boost::system::error_code &ec) {
       BOOST_LOG_SEV (lg_, trace)
          << "Timer ended: (" << ec.message () << ")";
       errorp->reset (ec);
   }


   void handle_read (opt_ec *errorp,
                     boost::system::error_code ec,
                     size_t len)
   {
       static const boost::regex expression (
           "IF bias present in baseband\\=(\\-?[0-9]+\\.[0-9]*) \\[Hz\\]");

       if (ec && ec != boost::asio::error::not_found) {
           errorp->reset (ec);
           BOOST_LOG_SEV (lg_, error)
              << "Error reading from front-end-cal: "
              << ec.message ();
           read_error_ = to_error_condition (ec);
           return;
       }

       // no error
       ec = boost::system::error_code ();

       char data[BUFFER_SIZE];
       buffer_.sgetn (data, len);
#ifdef GENESIS_DEBUG
       if (len) {
           std::cout << std::string (&data[0], &data[len]);
       }
#endif

       const char *begin = &data[0];
       const char *end = &data[len];

       // Search for the output which states the IF bias
       boost::match_results <const char *> what;
       boost::match_flag_type flags = boost::match_default;
       if (boost::regex_search (begin, end, what, expression, flags)) {
           BOOST_LOG_SEV (lg_, debug)
              << "Found IF bias of " << what[1];
           val_ = boost::lexical_cast <double> (what[1]);
           read_error_ = error_type ();
           errorp->reset (ec);
       }
       else {
           // Keep reading
           read_next (errorp);
       }
   }

   void read_next (opt_ec *ec) {
       boost::asio::async_read_until (
           stream_, buffer_, '\n',
           boost::bind (&reader::handle_read,
                        this,
                        ec,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
   }

   double val_;
   boost::asio::io_service service_;
   boost::asio::posix::stream_descriptor stream_;
   boost::asio::streambuf buffer_;
   logger lg_;
   error_type read_error_;
};

} // detail

enum {
    BUFFER_SIZE = 1024
};

struct calibrator::impl {
   impl ()
       : IF_ (0)
      {
      }

   double IF_;
   logger lg_;
};

calibrator::calibrator ()
    : impl_ (new impl ())
{
}

calibrator::error_type calibrator::read_if (int fd) {
    detail::reader r;
    impl_->IF_ = r.read (fd);
    return r.get_read_error ();
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
    BOOST_LOG_SEV (impl_->lg_, trace)
       << "Looking for previously saved calibrations.";
    if (detail::load_bias (path, impl_->IF_)) {
        BOOST_LOG_SEV (impl_->lg_, debug)
           << "IF bias for "
           << st.get_address ()
           << " loaded from "
           << path.native ();
        return error_type ();
    }
    BOOST_LOG_SEV (impl_->lg_, trace)
       << "No previously saved calibrations.";

    // Write configuration
    BOOST_LOG_SEV (impl_->lg_, trace)
       << "Writing config file.";
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
    BOOST_LOG_SEV (impl_->lg_, trace) << "Starting front-end-cal";
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
    BOOST_LOG_SEV (impl_->lg_, trace) << "front-end-cal started";
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
