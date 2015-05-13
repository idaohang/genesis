/*!
 * \file udp_packet.cpp
 * \brief Defines the method for unpacking UDP packets.
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

#include "udp_packet.hpp"
#include <cstring>
#include <boost/asio/detail/socket_ops.hpp>

namespace genesis {
namespace listen {

using namespace boost::asio::detail::socket_ops;

void udp_packet::unpack_impl (char *packet) {
    // unpack the port number
    port_ = *reinterpret_cast <unsigned short *> (&packet[0]);
    port_ = network_to_host_short (port_);

    // unpack the type
    unsigned t = *reinterpret_cast <unsigned *>(&packet[PORT_SIZE]);
    type_ = static_cast<station_type> (network_to_host_long (t));
    if (type_ != genesis::station::STATION_TYPE_BASE &&
        type_ != genesis::station::STATION_TYPE_ROVER)
    {
        type_ = genesis::station::STATION_TYPE_UNKNOWN;
    }
}

}
}
