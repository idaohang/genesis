/*!
 * \file session.hpp
 * \brief Interface for the server-side comms with child process.
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
#pragma once
#ifndef GENESIS_SESSION_HPP
#define GENESIS_SESSION_HPP

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/local/stream_protocol.hpp>

namespace genesis {

class client_controller;
class station;
class logger;

/*!
 * \brief Class reads incoming data from child process.
 */
class session : public boost::enable_shared_from_this<session> {
public:
   typedef boost::shared_ptr <client_controller> controller_ptr;

   session(boost::asio::io_service& service,
           const station &st,
           controller_ptr controller);

   ~session ();

   boost::asio::local::stream_protocol::socket &socket ();

   void start();

   void handle_read(const boost::system::error_code& error,
                    size_t bytes_transferred);

private:
   void start_read ();
private:
   struct impl;
   boost::shared_ptr <impl> impl_;
};

}
#endif //GENESIS_SESSION_HPP
