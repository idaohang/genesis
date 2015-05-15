/*!
 * \file log.cpp
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
#include "log.hpp"
#include <iomanip>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/expressions.hpp>

namespace genesis {

using namespace boost::log;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;


std::ostream& operator<< (std::ostream& strm, genesis::log_severity level) {
    static const char* strings[] =
    {
        "trace",
        "debug",
	"info",
        "warning",
        "error",
        "critical"
    };

    if (static_cast< std::size_t >(level) < sizeof(strings) / sizeof(*strings))
        strm << strings[level];
    else
        strm << static_cast< int >(level);

    return strm;
}

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", genesis::log_severity)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "Timestamp", boost::posix_time::ptime)

void init_logging () {
    boost::log::formatter fmt =
       expr::stream
       << std::setw (8) << std::left << severity
       << " [" << timestamp << "] "
       << expr::smessage;

    boost::shared_ptr<sinks::synchronous_sink<sinks::text_ostream_backend> >
       sink = add_console_log();
#ifndef GENESIS_DEBUG
    sink->set_filter(severity > debug);
#endif

    sink->set_formatter (fmt);
    sink->locked_backend ()->auto_flush (true);

    add_file_log ("genesis.log")->set_formatter (fmt);
    add_common_attributes ();
    boost::log::core::get ()->add_global_attribute("Timestamp",
						   attrs::local_clock());
}

}
