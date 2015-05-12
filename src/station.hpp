/*!
 * \file station.hpp
 * \brief Defines the interface for a remote GNSS antenna.
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
 * along with GNSS-SDR. If not, see <http://www.gnu.org/licenses/>.
 *
 * -------------------------------------------------------------------------
 */

#ifndef GENESIS_STATION_HPP
#define GENESIS_STATION_HPP

#include <string>

namespace genesis {

/*!
 * \brief A class which represents a remote antenna and the
 * receiver which reads from it and processes the data.
 */
class station {
public:
   enum station_type {
      STATION_TYPE_UNKNOWN = 0,
      STATION_TYPE_BASE = 1,
      STATION_TYPE_ROVER = 2
   };


   /*!
    * \brief Construct an empty station.
    */
   station ()
       : type_ (STATION_TYPE_UNKNOWN), port_ (0)
      {
      }

   /*!
    * \brief Construct a station, setting the station type
    * the address and the port. The name defaults to the last
    * 15 characters of the address.
    */
   station (station_type type,
            const std::string &address,
            unsigned short port)
       : type_ (type), address_ (address), port_ (port)
      {
      }

   /*!
    * \brief Construct a station, setting the station type
    * the address, the port, and the name.
    */
   station (station_type type,
            const std::string &address,
            unsigned short port,
            const std::string &name)
       : name_ (name), type_ (type), address_ (address), port_ (port)
      {
      }

   inline const std::string &get_name () const {
       return name_;
   }

   inline station_type get_type () const {
       return type_;
   }

   inline const std::string &get_address () const {
       return address_;
   }

   inline unsigned short get_port () const {
       return port_;
   }


private:
   std::string name_; // friendly name
   station_type type_; // base station or rover
   std::string address_; // IPv4 or 6 address
   unsigned short port_; // The port to connect to
};


inline bool operator < (const station &l, const station &r) {
    return l.get_name () < r.get_name ();
}
inline bool operator <= (const station &l, const station &r) {
    return l.get_name () <= r.get_name ();
}
inline bool operator > (const station &l, const station &r) {
    return l.get_name () > r.get_name ();
}
inline bool operator >= (const station &l, const station &r) {
    return l.get_name () >= r.get_name ();
}

}

#endif // GENESIS_STATION_HPP
