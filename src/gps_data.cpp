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

static std::string shared_name (const genesis::station &st) {
    return st.get_type () == genesis::station::STATION_TYPE_BASE ?
       ( std::string ("genesis.base") ) :
       ( std::string ("genesis") + st.get_address () );
}

namespace genesis {

enum {
    MAP_SIZE_MULTIPLIER = 128
};

gps_data::gps_data (const station &st)
    :
    shared_name_(shared_name (st)),
    name_ (st.get_address ())
{
}


const std::string &gps_data::name () const {
    return name_;
}

gps_data::ref_time_ptr gps_data::ref_time () {
    if (!ref_time_) {
        ref_time_.reset (
            new concurrent_shared_map<Gps_Ref_Time> (
                shared_name_ + ".gps_ref_time"));
    }
    return ref_time_;
}

gps_data::utc_model_ptr gps_data::utc_model () {
    if (!utc_model_) {
        utc_model_.reset (
            new concurrent_shared_map<Gps_Utc_Model> (
                shared_name_ + ".gps_utc_model"));
    }
    return utc_model_;
}

gps_data::almanac_ptr gps_data::almanac () {
    if (!almanac_) {
        almanac_.reset (
            new concurrent_shared_map<Gps_Almanac> (
                shared_name_ + ".gps_almanac"));
    }
    return almanac_;
}

gps_data::iono_ptr gps_data::iono () {
    if (!iono_) {
        iono_.reset (
            new concurrent_shared_map<Gps_Iono> (
                shared_name_ + ".gps_iono"));
    }
    return iono_;
}

gps_data::ephemeris_ptr gps_data::ephemeris () {
    if (!ephemeris_) {
        ephemeris_.reset (
            new concurrent_shared_map<Gps_Ephemeris> (
                shared_name_ + ".gps_ephemeris"));
    }
    return ephemeris_;
}


}
