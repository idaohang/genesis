/*!
 * \file error.hpp
 * \brief Defines custom error codes for Genesis.
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

#include "error.hpp"

namespace genesis {

const boost::array <std::string, max_error> error_category::messages_ = {{
        "Success",
        "Invalid packet length",
        "Invalid station",
        "Unknown station type",
        "Base station is already set",
        "The specified rover already exists",
        "The specified station was not found",
        "Already running"
    }};

const char *error_category::name () const {
    return "genesis";
}

std::string error_category::message (int ev) const {
    if (ev < 0 || ev >= static_cast<int> (messages_.size ())) {
        return "Unknown error";
    }
    else {
        return messages_[ev];
    }
}

}
