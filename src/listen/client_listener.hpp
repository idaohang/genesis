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
 * along with Genesis. If not, see <http://www.gnu.org/licenses/>.
 *
 * -------------------------------------------------------------------------
 */

#ifndef GENESIS_CLIENT_LISTENER_HPP
#define GENESIS_CLIENT_LISTENER_HPP

#include <boost/noncopyable.hpp>
#include <boost/system/error_code.hpp>
#include <boost/shared_ptr.hpp>

namespace genesis {

class client_controller;

namespace listen {


/*!
 * \brief This is the base class for accepting client notifications
 * (pings) and adding the clients to the client controller.
 */
class client_listener : boost::noncopyable {
public:
   typedef client_controller controller_type;
   typedef boost::shared_ptr <controller_type> controller_ptr;

   typedef boost::system::error_condition error_type;
protected:
   inline client_listener (controller_ptr controller)
       : controller_ (controller)
      {
      }

public:
   inline virtual ~client_listener ()
      {
      }

   virtual error_type start () = 0;
   virtual error_type stop () = 0;
   virtual bool is_listening () const = 0;

protected:
   inline controller_ptr get_controller ( ) const {
      return controller_;
   }


private:
   controller_ptr controller_;
};

}
}

#endif // GENESIS_CLIENT_LISTENER_HPP
