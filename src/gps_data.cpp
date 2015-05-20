/*!
 * \file gps_data.cpp
 * \brief Structure holds the GPS data of an incoming receiver.
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
#include "gps_data.hpp"
#include "station.hpp"
#include "concurrent_shared_map.h"

static std::string shared_name (const genesis::station &st, const char *suffix) {
    return st.get_type () == genesis::station::STATION_TYPE_BASE ?
       ( std::string ("genesis.base") + suffix ) :
       ( std::string ("genesis") + st.get_address () + "." + suffix );
}

namespace genesis {

enum {
    MAP_SIZE_MULTIPLIER = 128
};

gps_data::gps_data (const station &st)
    : name (st.get_address ()),
      ref_time (
          new concurrent_shared_map<Gps_Ref_Time> (
              shared_name (st, "gps_ref_time"))),
      utc_model (
          new concurrent_shared_map<Gps_Utc_Model> (
              shared_name (st, "gps_utc_model"))),

      almanac (
          new concurrent_shared_map<Gps_Almanac> (
              shared_name (st, "gps_almanac"))),

      iono (
          new concurrent_shared_map<Gps_Iono> (
              shared_name (st, "gps_iono"))),

      ephemeris (
          new concurrent_shared_map<Gps_Ephemeris> (
              shared_name (st, "gps_ephemeris")))
{
}

}
