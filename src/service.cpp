/*!
 * \file service.cpp
 * \brief Definition for the IO service controller.
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

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include "service.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "session.hpp"
#include "calibrator.hpp"
#include <boost/thread.hpp>
#include <boost/foreach.hpp>

namespace genesis {

enum {
   GENESIS_PORT = 9255
};

using boost::asio::ip::udp;
using boost::asio::local::stream_protocol;

service::service ()
   : acceptor_ (io_service_),
     signal_ (io_service_, SIGCHLD),
     udp_socket_ (io_service_),
     stdin_ (io_service_, ::dup (STDIN_FILENO)),
     stdin_buf_ ((size_t)MAX_STDIN),
     controller_ (boost::make_shared<client_controller> ())
{
   start_signal_wait ();
}

service::~service () {
   // cleanly shutdown children
   scoped_lock lock (mutex_);
   BOOST_FOREACH (int pid, to_kill_) {
      ::kill (pid, SIGTERM);
   }
   to_kill_.clear ();
}

service::error_type service::run (const std::string &socket_file,
                                  const std::string &multicast_address)
{
   error_type ec;
   ec = setup_acceptor (socket_file);
   if (!ec) {
      ec = setup_listener (multicast_address);

      if (!ec) {
         // handle input
         boost::asio::async_read_until (
            stdin_, stdin_buf_, '\n',
            boost::bind (&service::on_stdin,
                         this,
                         boost::asio::placeholders::error,
                         boost::asio::placeholders::bytes_transferred));

         // start running
         io_service_.run ();
      }
   }

   return ec;
}


service::error_type service::setup_acceptor (const std::string &socket_file) {
   boost::system::error_code ec;

   // Start domain socket acceptor
   socket_file_ = socket_file;
   std::remove (socket_file_.c_str());

   acceptor_.open (stream_protocol (), ec);
   if (ec) {
      BOOST_LOG_SEV (lg_, critical) << "Failed to open acceptor: "
                                    << ec.message ();
      return to_error_condition (ec);
   }

   acceptor_.bind (stream_protocol::endpoint (socket_file_), ec);
   if (ec) {
      BOOST_LOG_SEV (lg_, critical) << "Failed to bind to socket file "
                                    << socket_file_
                                    << ": " << ec.message ();
      return to_error_condition (ec);
   }

   if (acceptor_.listen (boost::asio::socket_base::max_connections, ec)) {
      BOOST_LOG_SEV (lg_, critical) << "Failed to listen to socket file "
                                    << socket_file_
                                    << ": " << ec.message ();
      return to_error_condition (ec);
   }

   return error_type ();
}
service::error_type service::setup_listener
(const std::string &multicast_address)
{
   boost::system::error_code ec;

   // Start multicast listener
   boost::asio::ip::address addr =
      boost::asio::ip::address::from_string (multicast_address, ec);
   if (ec) {
      BOOST_LOG_SEV (lg_, critical) << "Bad multicast address.";
      return to_error_condition (ec);
   }

   boost::asio::ip::address listen;
   if (addr.is_v4 ()) {
      listen = boost::asio::ip::address_v4::any ();
   }
   else {
      listen = boost::asio::ip::address_v6::any ();
   }
   udp::endpoint ep (listen, GENESIS_PORT);

   // open the socket
   BOOST_LOG_SEV (lg_, trace) << "Opening UDP socket";
   udp_socket_.open (ep.protocol (), ec);
   if (ec) {
      BOOST_LOG_SEV (lg_, error) <<
         "Failed to open socket: " << ec.message ();
      return to_error_condition (ec);
   }

   // set options
   BOOST_LOG_SEV (lg_, trace) << "Setting reuse address option.";
   udp_socket_.set_option (udp::socket::reuse_address (true), ec);
   if (ec) {
      BOOST_LOG_SEV (lg_, error) <<
         "Failed to set reuse address socket option: " << ec.message ();
      return to_error_condition (ec);
   }

   // bind
   BOOST_LOG_SEV (lg_, debug) << "Binding to endpoint " << ep;
   udp_socket_.bind (ep, ec);
   if (ec) {
       BOOST_LOG_SEV (lg_, error) <<
          "Failed to bind to endpoint" << ep << ": " << ec.message ();
       return to_error_condition (ec);
   }

   if (addr.is_multicast ()) {
       // join the group
       BOOST_LOG_SEV (lg_, debug) << "Address is multicast. Joining group" << addr;
       udp_socket_.set_option (boost::asio::ip::multicast::join_group (addr), ec);
       if (ec) {
           BOOST_LOG_SEV (lg_, error) <<
              "Failed to join multicast group: " << ec.message ();
           return to_error_condition (ec);
       }
   }
   BOOST_LOG_SEV (lg_, trace) << "UDP socket open and ready.";

   // handle reads
   udp_socket_.async_receive_from (
      boost::asio::buffer (data_, MAX_DATA_LENGTH), sender_endpoint_,
      boost::bind (&service::handle_udp_receive,
                   this,
                   boost::asio::placeholders::error,
                   boost::asio::placeholders::bytes_transferred));

   return error_type ();
}

void service::handle_udp_receive (const boost::system::error_code &error,
                                  size_t bytes_received)
{
   if (error) {
      // Damn
      BOOST_LOG_SEV (lg_, critical) <<
         "Error received during receive: " << error.message ();
      shutdown ();
   }
   else {
      if (bytes_received != MAX_DATA_LENGTH) {
         // Didn't read the whole packet
         BOOST_LOG_SEV (lg_, warning) <<
            "Short packet received";
      }
      else {
         handle_packet ();
      }

      udp_socket_.async_receive_from (
         boost::asio::buffer (data_, MAX_DATA_LENGTH), sender_endpoint_,
         boost::bind (&service::handle_udp_receive,
                      this,
                      boost::asio::placeholders::error,
                      boost::asio::placeholders::bytes_transferred));
   }
}


void service::handle_packet () {
   // Unwrap packet
   packet p;
   p.unpack (data_);
   data_.assign (0);

   std::string address = sender_endpoint_.address ().to_string ();
   station st = make_station (p, address);

   BOOST_LOG_SEV (lg_, trace)
      << "Received station packet from "
      << st.get_address ()
      << " port=" << st.get_port ()
      << " type=" << st.get_type ();

   // Adding the station to the controller
   // prevents duplicates from being initiated.
   error_type e = controller_->add_station (st);
   if (e) {
      BOOST_LOG_SEV (lg_, error)
         << "Error adding new station: " << e.message();
   }
   else {
      // New thread to calibrate and run
      boost::thread (boost::bind (&service::start_station,
                                  this,
                                  st));
   }
}

void service::prepare_fork () {
   boost::log::core::get ()->flush ();
   boost::log::core::get ()->set_logging_enabled (false);
   io_service_.notify_fork (boost::asio::io_service::fork_prepare);
}

void service::child_fork () {
   acceptor_.close ();
   udp_socket_.close ();
   signal_.cancel ();

   io_service_.notify_fork (boost::asio::io_service::fork_child);
}

void service::parent_fork (int pid) {
   io_service_.notify_fork (boost::asio::io_service::fork_parent);
   boost::log::core::get ()->set_logging_enabled (true);
   if (pid > 0) {
      scoped_lock lock (mutex_);
      to_kill_.insert (pid);
   }
}

void service::start_station (const station &st) {
   // Calibrate
   calibrator cal (io_service_);
   error_type ec = cal.calibrate (st, this);

   if (ec) {
      BOOST_LOG_SEV (lg_, error) << "Failed to calibrate station "
                                 << st.get_address ()
                                 << ": "
                                 << ec.message ();
   }
   else {
      // TODO: Run GNSS-SDR
   }

   if (!io_service_.stopped ()) {
       // Done - remove station
       controller_->remove_station (st);
   }
}

void service::start_signal_wait () {
   signal_.async_wait (boost::bind (&service::handle_signal_wait, this));
}

void service::handle_signal_wait () {
   if (acceptor_.is_open ()) {
      int status = 0;
      int count = 0;
      int pid;
      while ((pid = waitpid (-1, &status, WNOHANG)) > 0) {
         count++;
         scoped_lock lock (mutex_);
         to_kill_.erase (pid);
      }

      BOOST_LOG_SEV (lg_, trace) << "Reaped " << count << " zombies.";

      start_signal_wait ();
   }
}

void service::on_stdin (const boost::system::error_code &ec,
			size_t length)
{
   size_t line_len = length;
   std::string s;

   if (!ec) {
      line_len--;
   }
   else if (ec != boost::asio::error::not_found) {
      BOOST_LOG_SEV (lg_, error) << "Error reading standard input: "
                                 << ec.message ();
      shutdown ();
      return;
   }

   stdin_buf_.sgetn (&stdin_cstr_[0], line_len);
   stdin_buf_.consume (length - line_len);
   s.assign (&stdin_cstr_[0], line_len);

   // TODO: More control messages can go here
   // May need to refactor out to own class
   if (s == "q" || s == "Q") {
      // quit
      BOOST_LOG (lg_) << "Received shutdown signal.";
      shutdown ();
   }
   else {
      // handle input
      boost::asio::async_read_until (
	 stdin_, stdin_buf_, '\n',
            boost::bind (&service::on_stdin,
                         this,
                         boost::asio::placeholders::error,
                         boost::asio::placeholders::bytes_transferred));
   }
}

void service::shutdown () {
   BOOST_LOG_SEV (lg_, trace) << "Shutting down.";
   io_service_.stop ();
   // cleanly shutdown children
   scoped_lock lock (mutex_);
   BOOST_FOREACH (int pid, to_kill_) {
      ::kill (pid, SIGTERM);
   }
   to_kill_.clear ();
}
}
