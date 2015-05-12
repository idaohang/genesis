/*!
 * \file udp_multicast_listener.cpp
 * \brief Definitions for accepting client notifications via UDP multicast.
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

#include "udp_multicast_listener.hpp"
#include "client_controller.hpp"
#include "udp_packet.hpp"
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>

namespace genesis {
namespace listen {

namespace ip = boost::asio::ip;
using ip::udp;

class udp_multicast_listener::listener_impl {
   typedef udp_multicast_listener::error_type error_type;
public:

   listener_impl (const char *address, short port, udp_multicast_listener &owner)
      : running_ (false), owner_ (owner), socket_ (service_)
   {
      using namespace boost::system;

      // Get multicast address
      mcast_address_= ip::address::from_string (address, ctor_error_);
      if (ctor_error_) {
	 return;
      }
      if (!mcast_address_.is_multicast ()) {
	 ctor_error_ = errc::make_error_code(errc::invalid_argument);
	 return;
      }

      // Listen on any interface
      ip::address listen;
      if (mcast_address_.is_v4 ()) {
	 listen = ip::address_v4::any ();
      }
      else {
	 listen = ip::address_v6::any ();
      }

      // Make enpoint
      endpoint_ = udp::endpoint (listen, port);
   }

   ~listener_impl () {
      stop ();
   }

   error_type start () {
      if (ctor_error_) {
	 return ctor_error_;
      }
      if (!running_) {
	 error_type ec;
	 struct close_on_error {
	    close_on_error (udp::socket &socket)
	       : ok_ (false), socket_ (socket)
	    {
	    }

	    ~close_on_error () {
	       if (!ok_) {
		  socket_.close ();
	       }
	    }

	    void set_ok () {
	       ok_ = true;
	    }

	    bool ok_;
	    udp::socket &socket_;
	 };

	 // open the socket
	 socket_.open (endpoint_.protocol (), ec);
	 if (ec) {
	    return ec;
	 }
	 close_on_error closer (socket_);

	 // set options
	 socket_.set_option (udp::socket::reuse_address (true), ec);
	 if (ec) {
	    return ec;
	 }

	 // bind
	 socket_.bind (endpoint_, ec);
	 if (ec) {
	    return ec;
	 }

	 // join the group
	 socket_.set_option (ip::multicast::join_group (mcast_address_), ec);
	 if (ec) {
	    return ec;
	 }

	 // All OK now
	 closer.set_ok ();

	 // handle reads
	 socket_.async_receive_from (
	    boost::asio::buffer (data_, MAX_DATA_LENGTH), sender_endpoint_,
            boost::bind (&listener_impl::handle_receive_from,
			 this,
			 boost::asio::placeholders::error,
			 boost::asio::placeholders::bytes_transferred));

	 // start running
	 thread_ = boost::thread (boost::bind (&boost::asio::io_service::run,
					       &service_));
	 running_ = true;
      }
   }

   error_type stop () {
      if (ctor_error_) {
	 return ctor_error_;
      }
      if (running_) {
	 service_.stop ();
	 thread_.join ();
	 running_ = false;
	 socket_.close ();
      }
   }

   bool is_listening () const {
      return running_;
   }

private:

   void handle_receive_from (error_type error, size_t bytes_received) {
      if (error) {
	 // Damn
	 stop ();
      }
      else {
	 // Unwrap packet
	 udp_packet packet;
	 packet.unpack<MAX_DATA_LENGTH> (data_);

	 if (packet.get_station_type () != station::STATION_TYPE_UNKNOWN) {
	    // TODO: Add new client to controller

	 }
	 else {
	    // bork
	 }

	 socket_.async_receive_from (
	    boost::asio::buffer (data_, MAX_DATA_LENGTH), sender_endpoint_,
            boost::bind (&listener_impl::handle_receive_from, this, _1, _2));
      }
   }

private:
   enum {
      MAX_DATA_LENGTH = udp_packet::FIXED_DATA_SIZE
   };

   char data_ [MAX_DATA_LENGTH];
   udp::endpoint sender_endpoint_;
   error_type ctor_error_;
   bool running_;
   udp_multicast_listener &owner_;
   boost::thread thread_;
   boost::asio::io_service service_;
   ip::address mcast_address_;
   udp::socket socket_;
   udp::endpoint endpoint_;
};

udp_multicast_listener::udp_multicast_listener (const char *address,
						   short port,
						   controller_ptr controller)
   : client_listener (controller),
     impl_ (new listener_impl (address, port, *this))
{
}

udp_multicast_listener::~udp_multicast_listener () {
}


udp_multicast_listener::error_type udp_multicast_listener::start () {
   return impl_->start ();
}

udp_multicast_listener::error_type udp_multicast_listener::stop () {
   return impl_->stop ();
}

bool udp_multicast_listener::is_listening () const {
   return impl_->is_listening ();
}

}
}
