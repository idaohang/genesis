/*!
 * \file service.hpp
 * \brief Interface for the IO service controller.
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

#ifndef GENESIS_SERVICE_HPP
#define GENESIS_SERVICE_HPP

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/move/core.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include "client_controller.hpp"
#include "error.hpp"
#include "fork.hpp"
#include "log.hpp"
#include "packet.hpp"
#include <set>
#include <string>

#ifndef BOOST_ASIO_HAS_LOCAL_SOCKETS
#error Local sockets are required
#endif

#ifndef BOOST_ASIO_HAS_POSIX_STREAM_DESCRIPTOR
#error Posix stream descriptors are required
#endif

namespace genesis {

/*!
 * Class for operating the IO of Genesis.
 */
class service : public fork_handler {
   BOOST_MOVABLE_BUT_NOT_COPYABLE (service)
public:
   typedef boost::system::error_condition error_type;

   service ();
   ~service ();

   error_type run (const std::string &socket_file,
                   const std::string &multicast_address);
private:
   error_type setup_acceptor (const std::string &socket_file);
   error_type setup_listener (const std::string &multicast_address);

   // Handle a new incoming UDP packet
   void handle_udp_receive (const boost::system::error_code &error,
                            size_t bytes_received);

   void handle_packet ();
   void start_station (const station &st);

   // Child proceses
   void start_signal_wait ();
   void handle_signal_wait ();

   // Commands
   void on_stdin (const boost::system::error_code &error,
		  size_t length);

   void shutdown ();

   // fork_handler
   virtual void prepare_fork ();
   virtual void child_fork ();
   virtual void parent_fork (int pid);
private:
   enum {
       MAX_DATA_LENGTH = packet::FIXED_DATA_SIZE
   };
   // IO service members
   boost::asio::io_service io_service_;

   // Domain socket members
   boost::asio::local::stream_protocol::acceptor acceptor_;
   std::string socket_file_;

   // Signals members
   boost::asio::signal_set signal_;

   // Multicast members
   boost::array <char, MAX_DATA_LENGTH> data_;
   boost::asio::ip::udp::socket mcast_socket_;
   boost::asio::ip::udp::endpoint sender_endpoint_;

   // stdio members
   enum {
      MAX_STDIN = 128
   };
   boost::asio::posix::stream_descriptor stdin_;
   boost::asio::streambuf stdin_buf_;
   boost::array <char, MAX_STDIN> stdin_cstr_;

   // Station members
   boost::shared_ptr <client_controller> controller_;

   // Logging members
   logger_mt lg_;

   // to kill
   std::set <int> to_kill_;
   boost::mutex mutex_;
   typedef boost::lock_guard <boost::mutex> scoped_lock;
};

}

#endif // GENESIS_SERVICE_HPP
