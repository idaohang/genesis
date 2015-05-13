/*!
 * \file calibrator.hpp
 * \brief Interface for remote antenna frequency shift calibration.
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
#ifndef GENESIS_CALIBRATE_HPP
#define GENESIS_CALIBRATE_HPP

#include <string>

namespace genesis {

/*!
 * \brief Class reads samples from remote antenna and calculates the
 * apparent doppler shift.
 */
extern double calibrate (const std::string &address, unsigned short port);

}

#endif // GENESIS_CALIBRATE_HPP
