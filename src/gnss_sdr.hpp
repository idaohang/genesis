/*!
 * \file gnss_sdr.hpp
 * \brief Interface for running GNSS-SDR.
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
#ifndef GENESIS_GNSS_SDR_HPP
#define GENESIS_GNSS_SDR_HPP

#include "error.hpp"
#include <boost/noncopyable.hpp>

namespace genesis {

class fork_handler;
class station;

/*!
 * \brief This class sets up the configuration and
 * launches gnss-sdr for a given remote station.
 */
class gnss_sdr : boost::noncopyable {
public:
   typedef boost::system::error_condition error_type;

   error_type run (const station &st,
                   fork_handler *handler,
                   double bias = 0);
};

}

#endif // GENESIS_GNSS_SDR_HPP
