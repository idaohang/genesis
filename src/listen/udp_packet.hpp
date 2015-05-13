/*!
 * \file udp_packet.hpp
 * \brief Defines the structure of the packet received by UDP listeners.
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

#ifndef GENESIS_LISTEN_UDP_PACKET_HPP
#define GENESIS_LISTEN_UDP_PACKET_HPP

#include "station.hpp"
#include <boost/static_assert.hpp>

namespace genesis {
namespace listen {

/*!
 * \brief The information contained in a UDP packet received from a client.
 */
struct udp_packet {
   typedef genesis::station::station_type station_type;

   enum {
       NAME_SIZE = 15,
       PORT_SIZE = 2,
       TYPE_SIZE = 4,
       FIXED_DATA_SIZE = NAME_SIZE + PORT_SIZE + TYPE_SIZE
   };

   inline udp_packet ()
       : type_ (genesis::station::STATION_TYPE_UNKNOWN)
      {
      }

   template <size_t N>
   void unpack (char (&packet)[N]) {
       BOOST_STATIC_ASSERT (N == FIXED_DATA_SIZE);
       unpack_impl (packet);
   }

   inline const char *get_name () const {
       return name_;
   }

   inline unsigned short get_port () const {
       return port_;
   }

   inline station_type get_station_type () const {
       return type_;
   }

private:
   char name_ [NAME_SIZE + 1];
   unsigned short port_;
   station_type type_;

private:
   void unpack_impl (char *packet);
};

inline genesis::station make_station (const udp_packet &packet,
                                      const std::string &address)
{
    std::string name (packet.get_name ());

    if (name.length () == 0) {
        return genesis::station (packet.get_station_type (),
                                 address,
                                 packet.get_port ());
    }
    else {
        return genesis::station (packet.get_station_type (),
                                 address,
                                 packet.get_port (),
                                 packet.get_name ());
    }
}

}
}

#endif // GENESIS_LISTEN_UDP_PACKET_HPP
