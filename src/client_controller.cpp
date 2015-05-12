/*!
 * \file client_controller.cpp
 * \brief Mechanism for controlling which stations are connected to
 * the receiver.
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

#include "client_controller.hpp"
#include "error.hpp"
#include <boost/move/algorithm.hpp>
#include <boost/range.hpp>

namespace genesis {

namespace detail {

typedef client_controller::error_type error_type;
using namespace boost::system::errc;

bool validate_station (const station &st) {
    if (st.get_type () == station::STATION_TYPE_UNKNOWN) {
        return false;
    }

    if (st.get_name ().length () == 0) {
        return false;
    }

    if (st.get_address ().length () == 0) {
        return false;
    }

    if (st.get_port () == 0) {
        return false;
    }

    return true;
}

struct find_by_name {
   find_by_name (const std::string &name)
       : name_ (name)
      {
      }

   bool operator() (const station &st) {
       return st.get_name () == name_;
   }

private:
   const std::string &name_;
};

}

client_controller_ptr make_client_controller () {
    return client_controller_ptr (new client_controller);
}

client_controller::client_controller()
{
}

client_controller::~client_controller()
{
    // TODO: Shutdown
}


client_controller::error_type client_controller::add_station (station st) {
    using namespace std;

    if (!detail::validate_station (st)) {
        return make_error_condition (invalid_station);
    }

    if (st.get_type () == station::STATION_TYPE_ROVER) {
        if (!rovers_.insert (boost::move (st)).second) {
            return make_error_condition (station_exists);
        }

        // TODO: Kick off new rover receiver
    }
    else {
        if (has_base ()) {
            return make_error_condition (base_already_set);
        }

        // TODO: Kick off base station receiver
        swap (base_, boost::move (st));
    }

    return error_type ();
}

client_controller::error_type
client_controller::remove_station (const std::string &name) {
    typedef std::set<station>::const_iterator iterator_type;

    if (base_.get_name () == name) {
        return reset_base ();
    }
    iterator_type found = std::find_if (boost::begin (rovers_),
                                        boost::end (rovers_),
                                        detail::find_by_name (name));
    if (found == boost::end (rovers_)) {
        // Not found
        return make_error_condition (station_not_found);
    }

    // TODO: handle any shutdown code required
    rovers_.erase (found);
    return error_type ();
}

bool client_controller::has_base () const {
    return detail::validate_station (base_);
}

client_controller::error_type client_controller::reset_base () {
    // TODO: handle any shutdown required
    base_ = station ();
    return error_type ();
}

}
