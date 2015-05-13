/*!
 * \file packet.hpp
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

#ifndef GENESIS_PACKET_HPP
#define GENESIS_PACKET_HPP

#include "station.hpp"
#include <boost/static_assert.hpp>
#include <boost/array.hpp>

namespace genesis {
/*!
 * \brief The information contained in a UDP packet received from a client.
 */
struct packet {
   typedef genesis::station::station_type station_type;

   enum {
       PORT_SIZE = 2,
       TYPE_SIZE = 4,
       FIXED_DATA_SIZE = PORT_SIZE + TYPE_SIZE
   };

   inline packet ()
       : type_ (genesis::station::STATION_TYPE_UNKNOWN)
      {
      }

   template <size_t N>
   void unpack (char (&pkt)[N]) {
       BOOST_STATIC_ASSERT (N == FIXED_DATA_SIZE);
       unpack_impl (pkt);
   }


   template <size_t N>
   void unpack (const boost::array<char, N> &pkt) {
       BOOST_STATIC_ASSERT (N == FIXED_DATA_SIZE);
       unpack_impl (&pkt[0]);
   }

   inline unsigned short get_port () const {
       return port_;
   }

   inline station_type get_station_type () const {
       return type_;
   }

private:
   unsigned short port_;
   station_type type_;

private:
   void unpack_impl (const char *pkt);
};

inline station make_station (const packet &pkt,
                             const std::string &address)
{
   return station (pkt.get_station_type (),
			    address,
			    pkt.get_port ());
}


}

#endif // GENESIS_PACKET_HPP
