/*!
 * \file child.hpp
 * \brief Child process for gnss-sdr
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
#include "child.hpp"
#include "station.hpp"
#include "calibrate.hpp"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/bind.hpp>
#include <glog/logging.h>

namespace genesis {

namespace fs = boost::filesystem;

child::child (boost::asio::io_service &io_service)
    : io_service_ (io_service), socket_ (io_service),
      signal_ (io_service, SIGINT, SIGTERM)
{
    signal_.async_wait (boost::bind (&child::on_stop, this));
}


child::error_type child::run (const station &st,
                              const std::string &socket)
{
    boost::system::error_code ec;
    socket_.connect (socket.c_str (), ec);
    if (ec) {
        return to_error_condition (ec);
    }

    // Move to a new directory
    // This is so logs and other output files can live independantly from
    //  other instances.
    fs::path path (boost::algorithm::replace_all_copy (
                       st.get_address (), ":", "."));
    if (!fs::exists (path)) {
        if (!fs::create_directory (st.get_address (), ec)) {
            return to_error_condition (ec);
        }
    }
    fs::current_path (st.get_address (), ec);
    if (ec) {
        return to_error_condition (ec);
    }

    google::InitGoogleLogging ("genesis");

    calibrate (st.get_address (),
	       st.get_port ());

    // TODO: Start gnss-sdr
    sleep (3);
    return error_type ();
}


void child::on_stop () {
    // TODO: Stop gnss-sdr

    io_service_.stop ();
}

}
