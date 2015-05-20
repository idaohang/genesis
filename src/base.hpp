/*!
 * \file base.hpp
 * \brief Base station global objects.
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
#ifndef GENESIS_BASE_HPP
#define GENESIS_BASE_HPP

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <vector>
#include "gnss_sdr_data.h"
#include "concurrent_dictionary.h"
#include "gps_ref_time.h"

#ifndef GENESIS_BASE_CPP
#define BASE_EXTERN extern
#else
#define BASE_EXTERN
#endif

namespace genesis {


BASE_EXTERN boost::mutex GLOBAL_BASE_STATION_MUTEX;
BASE_EXTERN std::vector <gnss_sdr_data> GLOBAL_BASE_STATION_OBSERVABLES;


typedef concurrent_dictionary <Gps_Ref_Time> ref_time_map;
boost::shared_ptr <ref_time_map> get_global_base_station_ref_time ();

}

#endif // GENESIS_BASE_HPP
