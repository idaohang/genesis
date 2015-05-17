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
#include <iostream>

enum {
    GENESIS_PORT = 9255,
    RTL_TCP_PORT = 1234,
    DATA_SIZE = genesis::packet::FIXED_DATA_SIZE
};

using namespace boost::asio::ip;

int main (int argc, char *argv[]) {
    try {
        if (argc < 2 || argc > 3) {
            std::cerr << "Usage: ping <address> [r|b]" << std::endl
                      << "r is rover (default) and b is base" << std::endl;
            return 1;
        }
        bool base = false;
        if (argc == 3) {
            std::string ("b") == argv[2];
        }

        boost::asio::io_service service;
        address addr = address::from_string (argv[1]);
        udp::endpoint ep (addr, GENESIS_PORT);
        udp::socket socket (service, ep.protocol ());

        char data[DATA_SIZE];
        ::memset (data, 0, sizeof (data));

        unsigned short *port = reinterpret_cast <unsigned short *>(&data[0]);
        *port = RTL_TCP_PORT;
        *port = boost::asio::detail::socket_ops::host_to_network_short (*port);

        unsigned *type = reinterpret_cast <unsigned *>(
            &data[genesis::packet::PORT_SIZE]);
        *type = base ?
           genesis::station::STATION_TYPE_BASE :
           genesis::station::STATION_TYPE_ROVER;
        *type = boost::asio::detail::socket_ops::host_to_network_long (*type);

        std::cout << "Sending "
                  << (base ? "base" : "rover")
                  << " ping to Genesis at " << addr
                  << std::endl;

        socket.send_to (boost::asio::buffer (data, DATA_SIZE), ep);
    }
    catch (std::exception &e) {
        std::cerr << "Exception: " << e.what () << std::endl;
    }
}
