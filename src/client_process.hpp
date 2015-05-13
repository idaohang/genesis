/*!
 * \file client_process.hpp
 * \brief Defines the interface for running a child process
 * for a station.
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

#ifndef GENESIS_CLIENT_PROCESS_HPP
#define GENESIS_CLIENT_PROCESS_HPP

#include "client_controller.hpp"
#include "error.hpp"
#include <boost/move/core.hpp>

namespace genesis {

/*!
 * \brief Class takes a station definition and starts a subprocess
 * to perform GNSS calculations.
 */
class client_process {
public:
   BOOST_MOVABLE_BUT_NOT_COPYABLE (client_process)

   typedef boost::system::error_condition error_type;

   inline client_process (client_controller_ptr controller, const station &st)
      : controller_ (controller), station_ (st)
   {
   }

   virtual error_type start () = 0;

   virtual error_type stop () = 0;

protected:

   inline client_controller_ptr get_controller () const {
      return controller_;
   }

private:
   client_controller_ptr controller_;
   station station_;
};

}

#endif // GENESIS_CLIENT_PROCESS_HPP
