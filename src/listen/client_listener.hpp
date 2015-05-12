/*!
 * \file client_listener.hpp
 * \brief An interface for accepting client notifications.
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

#ifndef GENESIS_CLIENT_LISTENER_HPP
#define GENESIS_CLIENT_LISTENER_HPP

#include <boost/move/core.hpp>
#include <boost/system/error_code.hpp>
#include <boost/shared_ptr.hpp>

namespace genesis {
namespace listen {

class client_controller;

/*!
 * \brief This is the base class for accepting client notifications
 * (pings) and adding the clients to the client controller.
 */
class client_listener {
public:
   typedef client_controller controller_type;
   typedef boost::shared_ptr <controller_type> controller_ptr;

   typedef boost::system::error_code error_type;
protected:
   client_listener (controller_ptr controller);

   BOOST_MOVABLE_BUT_NOT_COPYABLE (client_listener);

public:
   virtual ~client_listener ();

   virtual error_type start () = 0;
   virtual error_type stop () = 0;
   virtual bool is_listening () const = 0;
protected:
   inline const controller_type &get_controller ( ) const {
      return *controller_;
   }

   inline controller_type &get_controller ( ) {
      return *controller_;
   }

private:
   controller_ptr controller_;
};

}
}

#endif // GENESIS_CLIENT_LISTENER_HPP
