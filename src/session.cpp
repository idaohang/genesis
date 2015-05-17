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
#include "station.hpp"
#include "log.hpp"
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio/placeholders.hpp>

namespace genesis {

struct session::impl {
   typedef session::controller_ptr controller_ptr;

   enum {
       BUFFER_SIZE = 1024
   };

   impl (boost::asio::io_service& service,
         const station &st,
         controller_ptr controller)
       : socket_(service), station_ (st), controller_ (controller)
      {
      }

   boost::asio::local::stream_protocol::socket socket_;
   boost::array <char, BUFFER_SIZE> data_;
   const station station_;
   controller_ptr controller_;
   logger lg_;
};


session::session(boost::asio::io_service& service,
                 const station &st,
                 controller_ptr controller)
    : impl_ (new impl (service, st, controller))
{
}

session::~session () {
    impl_->controller_->remove_station (impl_->station_);
}

boost::asio::local::stream_protocol::socket &session::socket () {
    return impl_->socket_;
}

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
        impl_->socket_.close ();
        BOOST_LOG (impl_->lg_) << "Removing station "
                               << impl_->station_.get_address ();
        impl_->controller_->remove_station (impl_->station_);
    }
}


void session::start_read () {
    impl_->socket_.async_read_some(
        boost::asio::buffer(impl_->data_),
        boost::bind(&session::handle_read,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));

}

}
