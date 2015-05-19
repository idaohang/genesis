/*!
 * \file gps_data.hpp
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
#pragma once
#ifndef GENESIS_GPS_DATA_HPP
#define GENESIS_GPS_DATA_HPP

#include <boost/shared_ptr.hpp>
#include "concurrent_dictionary.h"
#include "gps_ref_time.h"
#include "gps_utc_model.h"
#include "gps_iono.h"
#include "gps_ephemeris.h"
#include "gps_almanac.h"

namespace genesis {
struct gps_data {
   boost::shared_ptr <concurrent_dictionary <Gps_Ref_Time>> ref_time;
   boost::shared_ptr <concurrent_dictionary <Gps_Utc_Model>> utc_model;
   boost::shared_ptr <concurrent_dictionary <Gps_Almanac>> almanac;
   boost::shared_ptr <concurrent_dictionary <Gps_Iono>> iono;
   boost::shared_ptr <concurrent_dictionary <Gps_Ephemeris>> ephemeris;
};
}

#endif // #ifndef GENESIS_GPS_DATA_HPP
