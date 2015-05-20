/*!
 * \file log.hpp
 * \brief Set up logging.
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
#pragma once
#ifndef GENESIS_LOG_HPP
#define GENESIS_LOG_HPP

#define BOOST_LOG_DYN_LINK 1
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

namespace genesis {

void init_logging ();

enum log_severity {
    trace,
    debug,
    info,
    warning,
    error,
    critical
};


class logger : public boost::log::sources::severity_logger<log_severity> {
public:
   logger ()
       : severity_logger (boost::log::keywords::severity = info)
      {
      }
};


class logger_mt : public boost::log::sources::severity_logger_mt<log_severity> {
public:
   logger_mt ()
       : severity_logger_mt (boost::log::keywords::severity = info)
      {
      }
};


}

#endif // GENESIS_LOG_HPP
