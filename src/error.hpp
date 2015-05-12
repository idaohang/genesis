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
 * along with GNSS-SDR. If not, see <http://www.gnu.org/licenses/>.
 *
 * -------------------------------------------------------------------------
 */

#ifndef GENESIS_ERROR_HPP
#define GENESIS_ERROR_HPP

#include <boost/system/error_code.hpp>
#include <boost/array.hpp>
#include <string>

namespace genesis {
enum errorc {
    success,
    invalid_packet_length,
    invalid_station,
    unknown_station_type,
    base_already_set,
    station_exists,
    station_not_found,
    already_running,
    max_error
};

class error_category : public boost::system::error_category {
   static const boost::array<std::string, max_error> messages_;

public:
   virtual const char *name () const BOOST_SYSTEM_NOEXCEPT;
   virtual std::string message (int ev) const;
};

inline const error_category &genesis_errors () {
    static error_category cat_;
    return cat_;
}

}

namespace boost {
namespace system {

template <>
struct is_error_condition_enum <genesis::errorc> {
   static const bool value = true;
};

}
}


namespace genesis {

inline boost::system::error_condition
make_error_condition (genesis::errorc e)
{
    return boost::system::error_condition (e, genesis_errors ());
}

inline boost::system::error_condition
to_error_condition (boost::system::error_code e)
{
    return e.default_error_condition ();
}

}

#endif // GENESIS_ERROR_HPP
