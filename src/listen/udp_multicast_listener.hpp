/*!
 * \file udp_multicast_listener.hpp
 * \brief An interface for accepting client notifications via UDP multicast.
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

#ifndef GENESIS_UDP_MULTICAST_LISTENER_HPP
#define GENESIS_UDP_MULTICAST_LISTENER_HPP

#include "client_listener.hpp"
#include "log.hpp"
#include <string>

namespace genesis {
namespace listen {

/*!
 * \brief This class listens on a multicast address for client messages.
 * When one is detected, the client is added to the client controller.
 */
class udp_multicast_listener : public client_listener {
public:
   typedef client_listener::controller_type controller_type;
   typedef client_listener::controller_ptr controller_ptr;
   typedef client_listener::error_type error_type;

   /*!
    * \brief Set up a new multicast listener.
    * \param address The multicast address to listen to.
    * \param port The port to listen on.
    * \param controller The controller to defer to.
    */
   udp_multicast_listener (const char *address,
                           short port,
                           controller_ptr controller);

   virtual ~udp_multicast_listener ();

   virtual error_type start ();
   virtual error_type stop ();
   virtual bool is_listening () const;

private:

   class listener_impl;
   friend class ::genesis::listen::udp_multicast_listener::listener_impl;
   boost::shared_ptr <listener_impl> impl_;

   logger_mt lg;
};

}
}


#endif //GENESIS_UDP_MULTICAST_LISTENER_HPP
