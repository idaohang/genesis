/*!
 * \file main.cpp
 * \brief Genesis main line.
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

#include "client_controller.hpp"
#include "udp_multicast_listener.hpp"
#include "log.hpp"

enum {
    GENESIS_PORT = 6951
};

int main () {
    genesis::init_logging ();
    genesis::logger lg;

    genesis::client_controller_ptr controller =
       genesis::make_client_controller ();

    genesis::listen::udp_multicast_listener
       listener ("239.255.255.1", GENESIS_PORT, controller);

    boost::system::error_condition ec = listener.start ();
    if (!ec) {
        BOOST_LOG (lg) << "Listening...";
        std::string str;
        std::cin >> str;
        ec = listener.stop ();
    }
    if (ec) {
        std::cerr << ec << std::endl;
    }
}
