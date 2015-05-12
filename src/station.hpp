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
};
}

#endif // GENESIS_STATION_HPP
