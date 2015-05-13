/*!
 * \file session.cpp
 * \brief Server-side comms with child process.
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
#include "session.hpp"
#include "client_controller.hpp"
#include <boost/bind.hpp>

namespace genesis {



void session::start() {
    start_read ();
}

void session::handle_read(const boost::system::error_code& error,
                          size_t /* bytes_transferred */)
{
    if (!error)
    {
        // TODO: Data to RTKLIB
        start_read ();
    }
    else {
        socket_.close ();
        BOOST_LOG (lg_) << "Removing station " << station_.get_address ();
        controller_->remove_station (station_.get_address ());
    }
}


void session::start_read () {
    socket_.async_read_some(
        boost::asio::buffer(data_),
        boost::bind(&session::handle_read,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));

}

}
