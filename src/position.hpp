/*!
 * \file position.hpp
 * \brief Interface for performing RTK positioning.
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
#ifndef GENESIS_POSITION_HPP
#define GENESIS_POSITION_HPP

#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "gnss_sdr_data.h"
#include "error.hpp"
#include "log.hpp"


namespace genesis {

class client_controller;
struct gps_data;
struct rtk_t;

/*!
 * \brief Class performs RTK positioning.
 */
class position : boost::noncopyable {
public:

   typedef boost::system::error_condition error_type;
   typedef boost::shared_ptr<client_controller> controller_ptr;
   typedef boost::shared_ptr<gps_data> gps_data_ptr;
   typedef boost::shared_ptr<rtk_t> rtk_ptr;

   position (controller_ptr controller, gps_data_ptr gps);
   ~position ();

   error_type rtk_position (const std::vector <gnss_sdr_data> &observables);

private:
   controller_ptr controller_;
   gps_data_ptr gps_data_;
   logger lg_;
   rtk_ptr rtk_;
};

}

#endif // GENESIS_POSITION_HPP
