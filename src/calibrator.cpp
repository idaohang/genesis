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
#include "error.hpp"
#include "calibrator.hpp"
#include <unistd.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <sstream>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <fstream>

namespace fs = boost::filesystem;

extern fs::path FRONT_END_CAL_EXECUTABLE;
extern fs::path FRONT_END_CAL_CONFIG_FILE;

static const fs::path SAVED_BIAS_FILE = "saved_bias";

namespace genesis {

namespace detail {

class bias_serializer {
    friend class boost::serialization::access;

   double bias_;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int)
   {
      ar  & bias_;
   }
public:
   explicit bias_serializer (double bias = 0) : bias_ (bias)
   {
   }

   inline double bias () const { return bias_; }
};

static bool load_bias (const fs::path &subdir, double &bias) {
   fs::path file = subdir;
   file /= SAVED_BIAS_FILE;

   std::ifstream ifs (file.c_str ());
   if (!ifs) {
      return false;
   }

   bias_serializer bs;
   boost::archive::text_iarchive ia (ifs);
   ia >> bs;
   bias = bs.bias();
   return true;
}
static bool save_bias (const fs::path &subdir, double bias) {
   fs::path file = subdir;
   file /= SAVED_BIAS_FILE;

   std::ofstream ofs (file.c_str ());
   if (!ofs) {
      return false;
   }

   bias_serializer bs (bias);
   boost::archive::text_oarchive oa (ofs);
   oa << bs;
   return true;
}

static boost::regex expression (
   "IF bias present in baseband\\=([0-9]+\\.[0-9]*) \\[Hz\\]");

static calibrator::error_type read_if (int fd, logger &lg, double &bias) {
   bias = 0.0;
   char buffer[1024];
   std::ostringstream ss;

   while (1) {
      ssize_t count = read(fd, buffer, sizeof(buffer));
      if (count == -1) {
         if (errno == EINTR) {
            continue;
         } else {
            BOOST_LOG_SEV (lg, error) << "Error reading from front-end-cal";
         }
      } else if (count == 0) {
         break;
      } else {
         ss << std::string (buffer, count);
      }
   }
   close(fd);

   std::string out = ss.str ();
   std::string::const_iterator start = out.begin (), end = out.end ();
   boost::match_results <std::string::const_iterator> what;
   boost::match_flag_type flags = boost::match_default;
   if (boost::regex_search (start, end, what, expression, flags)) {
      BOOST_LOG_SEV (lg, debug)
         << "Found IF bias of " << what[1];
      bias = boost::lexical_cast <double> (what[1]);
      return calibrator::error_type ();
   }
   BOOST_LOG_SEV (lg, error)
      << "No IF bias found";

   return make_error_condition (if_bias_not_found);
}

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
   ofs << "\nSignalSource.address=" << st.get_address () << std::endl;
   ofs << "SignalSource.port=" << st.get_port () << std::endl;
   return calibrator::error_type ();
}

}

calibrator::error_type calibrator::calibrate_impl (
   const station &st,
   boost::function <void ()> prepare_fork,
   boost::function <void ()> child_fork,
   boost::function <void (int)> parent_fork)
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
   if (detail::load_bias (path, IF_)) {
      BOOST_LOG_SEV (lg_, debug)
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
      BOOST_LOG_SEV (lg_, error) << "Failed to write config file "
                                 << "for station "
                                 << st.get_address ();
      return et;
   }

   // Execute front-end-cal
   prepare_fork ();
   int p[2];
   pipe (p);

   pid_t pid = fork ();
   if (pid == 0) {
      child_fork ();

      while ((dup2(p[1], STDERR_FILENO) == -1) && (errno == EINTR)) {}
      while ((dup2(p[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
      close(p[1]);
      close(p[0]);

      fs::current_path (path, ec);
      if (ec) {
         exit (1);
      }

      execl(FRONT_END_CAL_EXECUTABLE.c_str (),
            "front-end-cal",
            "--config_file",
            "front-end-cal.conf",
            "--log_dir",
            "./",
            (char*)0);
      perror ("execl");
      exit (1);
   }
   parent_fork (pid);
   close (p[1]);

   et = detail::read_if (p[0], lg_, IF_);
   if (!et) {
      BOOST_LOG_SEV (lg_, debug) << "Saving IF bias";
      if (!detail::save_bias (path, IF_)) {
         BOOST_LOG_SEV (lg_, warning) << "Saving IF bias failed.";
      }
   }
   return et;
}

}
