/*!
 * \file client_controller.hpp
 * \brief Defines the interface for designating rovers and base station
 *   connections.
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

#ifndef GENESIS_CLIENT_CONTROLLER_HPP
#define GENESIS_CLIENT_CONTROLLER_HPP


#include <boost/shared_ptr.hpp>
#include <boost/move/core.hpp>

namespace genesis {
   class client_controller;
   typedef boost::shared_ptr <client_controller> client_controller_ptr;

   /*!
    * Construct a new client controller.
    */
   client_controller_ptr make_client_controller ();

   /*!
    * \brief This class keeps track of which clients are connected
    *  and what kind of client they are.
    */
   class client_controller {
   private:
      BOOST_MOVABLE_BUT_NOT_COPYABLE (client_controller);

      client_controller ();
      friend client_controller_ptr make_client_controller ();
   public:
      ~client_controller ();

   private:

   };

}

#endif // GENESIS_CLIENT_CONTROLLER_HPP
