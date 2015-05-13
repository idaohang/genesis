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
#include "service.hpp"

namespace genesis {

enum {
    GENESIS_PORT = 9255
};

using boost::asio::ip::udp;
using boost::asio::local::stream_protocol;

service::service ()
    : acceptor_ (io_service_),
      mcast_socket_ (io_service_)
{
}

service::error_type service::run (const std::string &socket_file,
                                  const std::string &multicast_address)
{
    error_type ec;
    ec = setup_acceptor (socket_file);
    if (!ec) {
        ec = setup_listener (multicast_address);

        if (!ec) {
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
    if (!addr.is_multicast ()) {
        BOOST_LOG_SEV (lg_, critical) << "Address is not multicast.";
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
    mcast_socket_.open (ep.protocol (), ec);
    if (ec) {
        BOOST_LOG_SEV (lg_, error) <<
           "Failed to open socket: " << ec.message ();
        return to_error_condition (ec);
    }

    // set options
    mcast_socket_.set_option (udp::socket::reuse_address (true), ec);
    if (ec) {
        BOOST_LOG_SEV (lg_, error) <<
           "Failed to set reuse address socket option: " << ec.message ();
        return to_error_condition (ec);
    }

    // bind
    mcast_socket_.bind (ep, ec);
    if (ec) {
        BOOST_LOG_SEV (lg_, error) <<
           "Failed to bind to endpoint: " << ec.message ();
        return to_error_condition (ec);
    }

    // join the group
    mcast_socket_.set_option (boost::asio::ip::multicast::join_group (addr), ec);
    if (ec) {
        BOOST_LOG_SEV (lg_, error) <<
           "Failed to join multicast group: " << ec.message ();
        return to_error_condition (ec);
    }

    // handle reads
    mcast_socket_.async_receive_from (
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

        // TODO: Signal shutdown
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

        mcast_socket_.async_receive_from (
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
    ::memset (data_, 0, MAX_DATA_LENGTH);

    std::string address = sender_endpoint_.address ().to_string ();
    BOOST_LOG_SEV (lg_, trace)
       << "Received station packet from "
       << address
       << " port=" << p.get_port ()
       << " type=" << p.get_station_type ();

    error_type e = controller_.add_station (make_station (p, address));
    if (e) {
        BOOST_LOG_SEV (lg_, error)
           << "Error adding new station: " << e.message();
    }
    else {
        // TODO fork

        // child opens domain socket

        // parent accepts child client and starts a new session
    }
}

}
