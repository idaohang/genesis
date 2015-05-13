/*!
 * \file service.hpp
 * \brief Interface for the IO service controller.
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

#ifndef GENESIS_SERVICE_HPP
#define GENESIS_SERVICE_HPP
/*
#ifndef BOOST_ASIO_HAS_LOCAL_SOCKETS
#error Local sockets are required
#endif
*/
#include <boost/asio.hpp>
#include <boost/move/core.hpp>
#include "client_controller.hpp"
#include "error.hpp"
#include "log.hpp"
#include "packet.hpp"
#include <set>
#include <string>

namespace genesis {

/*!
 * Class for operating the IO of Genesis.
 */
class service {
   BOOST_MOVABLE_BUT_NOT_COPYABLE (service)
public:
   typedef boost::system::error_condition error_type;

   service ();

   error_type run (const std::string &socket_file,
                   const std::string &multicast_address);
private:
   error_type setup_acceptor (const std::string &socket_file);
   error_type setup_listener (const std::string &multicast_address);

   // Handle a new incoming UDP packet
   void handle_udp_receive (const boost::system::error_code &error,
                            size_t bytes_received);

   void handle_packet ();
private:
   enum {
       MAX_DATA_LENGTH = packet::FIXED_DATA_SIZE
   };
   // IO service members
   boost::asio::io_service io_service_;

   // Domain socket members
   boost::asio::local::stream_protocol::acceptor acceptor_;
   std::string socket_file_;

   // Multicast members
   char data_ [MAX_DATA_LENGTH];
   boost::asio::ip::udp::socket mcast_socket_;
   boost::asio::ip::udp::endpoint sender_endpoint_;

   // Station members
   client_controller controller_;

   // Logging members
   logger lg_;
};

}

#endif // GENESIS_SERVICE_HPP
