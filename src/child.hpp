/*!
 * \file child.hpp
 * \brief Child process for gnss-sdr
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
#ifndef GENESIS_CHILD_HPP
#define GENESIS_CHILD_HPP

#include <boost/asio.hpp>

namespace genesis {

class station;

/*!
 * \brief This class connects to the domain socket and
 * starts the gnss-sdr flowgraph.
 */
class child {
public:
   typedef boost::system::error_condition error_type;

   child (boost::asio::io_service &io_service);

   error_type run (const station &st,
                   const std::string &socket);
private:

   void on_stop ();

private:

   boost::asio::io_service &io_service_;
   boost::asio::local::stream_protocol::socket socket_;
   boost::asio::signal_set signal_;
};

}

#endif // GENESIS_CHILD_HPP
