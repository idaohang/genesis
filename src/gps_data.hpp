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

#include <string>
#include <boost/shared_ptr.hpp>
#include "concurrent_dictionary.h"
#include "gps_ref_time.h"
#include "gps_utc_model.h"
#include "gps_iono.h"
#include "gps_ephemeris.h"
#include "gps_almanac.h"

namespace genesis {
class station;

struct gps_data {
   gps_data (const station &st);

   typedef concurrent_dictionary <Gps_Ref_Time> ref_time_map;
   typedef boost::shared_ptr <ref_time_map> ref_time_ptr;

   typedef concurrent_dictionary <Gps_Utc_Model> utc_model_map;
   typedef boost::shared_ptr <utc_model_map> utc_model_ptr;

   typedef concurrent_dictionary <Gps_Almanac> almanac_map;
   typedef boost::shared_ptr <almanac_map> almanac_ptr;

   typedef concurrent_dictionary <Gps_Iono> iono_map;
   typedef boost::shared_ptr <iono_map> iono_ptr;

   typedef concurrent_dictionary <Gps_Ephemeris> ephemeris_map;
   typedef boost::shared_ptr <ephemeris_map> ephemeris_ptr;


   const std::string &name () const;
   ref_time_ptr ref_time ();
   utc_model_ptr utc_model ();
   almanac_ptr almanac ();
   iono_ptr iono ();
   ephemeris_ptr ephemeris ();

private:
   std::string shared_name_;
   std::string name_;
   ref_time_ptr ref_time_;
   utc_model_ptr utc_model_;
   almanac_ptr almanac_;
   iono_ptr iono_;
   ephemeris_ptr ephemeris_;
};
}

#endif // #ifndef GENESIS_GPS_DATA_HPP
