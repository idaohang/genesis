/*!
 * \file session.cpp
 * \brief Server-side comms with child process.
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
#include "session.hpp"
#include "client_controller.hpp"
#include "station.hpp"
#include "log.hpp"
#include "position.hpp"
#include "gps_data.hpp"
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/streambuf.hpp>
//#include <boost/asio/read.hpp>
#include <boost/move/core.hpp>
#include <boost/thread/mutex.hpp>
//#include <boost/archive/binary_iarchive.hpp>

namespace genesis {

struct session::impl {
   typedef session::controller_ptr controller_ptr;

   enum {
       BUFFER_SIZE = 1024
   };

   impl (boost::asio::io_service& service,
         const station &st,
         int outfd,
         controller_ptr controller)
       : socket_(service),
         mut_buf_(buffer_.prepare (sizeof (gnss_sdr_data) * 32)),
         station_ (st),
         controller_ (controller),
         outfd_ (outfd),
         gps_data_ (new gps_data (st)),
         pos_ (controller_, gps_data_)
      {
      }

   boost::asio::local::stream_protocol::socket socket_;
   boost::asio::streambuf buffer_;
   boost::asio::streambuf::mutable_buffers_type mut_buf_;
   //boost::array <char, sizeof (gnss_sdr_data)> buffer_;
   const station station_;
   controller_ptr controller_;
   logger lg_;
   int outfd_;
   boost::shared_ptr <gps_data> gps_data_;
   position pos_;
};


session::session(boost::asio::io_service& service,
                 const station &st,
                 int outfd,
                 controller_ptr controller)
    : impl_ (new impl (service, st, outfd, controller))
{
}

session::~session () {
    impl_->controller_->remove_station (impl_->station_);
    close (impl_->outfd_);
}

boost::asio::local::stream_protocol::socket &session::socket () {
    return impl_->socket_;
}

void session::start() {
    start_read ();
}

void session::handle_read(const boost::system::error_code& err,
                          size_t /* bytes_transferred */ )
{
    if (!err)
    {
        // Data to RTKLIB
        std::vector <gnss_sdr_data> observables;

        // deserialize observation data
        try {
            //std::istream is (&impl_->buffer_);
            //boost::archive::binary_iarchive ia (is);

            while (impl_->buffer_.size () >= sizeof (gnss_sdr_data)) {
                gnss_sdr_data dat;
                impl_->buffer_.sgetn (reinterpret_cast <char *> (&dat),
                                      sizeof (gnss_sdr_data));
                //memcpy (&dat, &impl_->buffer_[0], sizeof (gnss_sdr_data));
                //ia >> dat;
                observables.push_back (boost::move (dat));
            }
        }
        catch ( const std::exception &ex ) {
            BOOST_LOG_SEV (impl_->lg_, error)
               << "Deserialization error: " << ex.what ();
            // clear out buffer
            //impl_->buffer_.consume (impl_->buffer_.size ());
            start_read ();
            return;
        }

        if (observables.size () > 0) {
            BOOST_LOG_SEV (impl_->lg_, trace)
               << "Received " << observables.size () << " observables "
               << "from GNSS-SDR@" << impl_->station_.get_address ();

            if (impl_->station_.get_type () == station::STATION_TYPE_BASE) {
                // set global base observables
                impl_->controller_->set_base_observables (observables);
            }
            else {
                // perform RTK
                boost::system::error_condition e =
                   impl_->pos_.rtk_position (observables);
                if (e) {
                    BOOST_LOG_SEV (impl_->lg_, debug)
                       << "RTK positioning failed: " << e.message ();
                }
            }
        }

        start_read ();
    }
    else {
        impl_->socket_.close ();
        BOOST_LOG (impl_->lg_) << "Removing station "
                               << impl_->station_.get_address ();
        impl_->controller_->remove_station (impl_->station_);
    }
}


void session::start_read () {
    impl_->socket_.async_read_some (
        //boost::asio::async_read (
        //impl_->socket_,
        boost::asio::buffer (impl_->mut_buf_),
        //boost::asio::buffer (impl_->buffer_),
        boost::bind(&session::handle_read,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));

}


}
