/*!
 * \file genesis_ping.cpp
 * \brief Test UDP comms by sending a packet to Genesis.
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

#include "packet.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstring>

enum {
    GENESIS_PORT = 9255
};

struct sender {
   sender (boost::asio::io_service &service,
           const boost::asio::ip::address &address,
           bool base)
       : endpoint_ (address, GENESIS_PORT),
         socket_ (service, endpoint_.protocol ())
      {
          ::memset (data_, 0, sizeof (data_));

          unsigned short *port = reinterpret_cast <unsigned short *>(
              &data_[0]);
          *port = 1234;

          unsigned *type = reinterpret_cast <unsigned *>(
              &data_[genesis::packet::PORT_SIZE]);
          *type = base ?
             genesis::station::STATION_TYPE_BASE :
             genesis::station::STATION_TYPE_ROVER;
          *type = boost::asio::detail::socket_ops::host_to_network_long (*type);

          std::cout << "Sending "
                    << (base ? "base" : "rover")
                    << " ping to Genesis"
                    << std::endl;

          socket_.async_send_to (
              boost::asio::buffer (data_, DATA_SIZE), endpoint_,
              boost::bind(&sender::handle_send_to, this,
                          boost::asio::placeholders::error));
      }

private:
   enum {
       DATA_SIZE = genesis::packet::FIXED_DATA_SIZE
   };

   void handle_send_to (boost::system::error_code ec) {
       if (ec) {
           std::cerr << "Error in sending: " << ec << std::endl;
       }
       else {
           std::cout << "Sent ping to Genesis" << std::endl;
       }
   }

   boost::asio::ip::udp::endpoint endpoint_;
   boost::asio::ip::udp::socket socket_;
   char data_[DATA_SIZE];
};

int main (int argc, char *argv[]) {
    try {
        if (argc < 2 || argc > 3) {
            std::cerr << "Usage: ping <multicast_address> [r|b]" << std::endl
                      << "r is rover (default) and b is base" << std::endl;
            return 1;
        }

        boost::asio::io_service service;
        sender s (service,
                  boost::asio::ip::address::from_string (argv[1]),
                  argc == 3 && std::string (argv[2]) == "b");
        service.run ();
    }
    catch (std::exception &e) {
        std::cerr << "Exception: " << e.what () << std::endl;
    }
}
