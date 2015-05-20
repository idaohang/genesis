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

#define GENESIS_BASE_CPP
#include "base.hpp"
#include <boost/make_shared.hpp>
#include "concurrent_shared_map.h"

namespace genesis {

boost::shared_ptr <ref_time_map> get_global_base_station_ref_time () {
    static boost::shared_ptr <ref_time_map>  global_ =
       boost::make_shared <concurrent_shared_map<Gps_Ref_Time>>
       ("genesis.base.gps_ref_time");

    return global_;
}

}
