/*!
 * \file station_config.hpp
 * \brief Contains information about stations that can be persisted.
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
#include "station_config.hpp"
#include <fstream>

namespace genesis {

namespace fs = boost::filesystem;

/*!
 * \brief Load a station config from a file.
 * \returns true if the config was loaded.
 */
bool load_station_config (const fs::path &file, station_config &config) {
   std::ifstream ifs (file.c_str ());
   if (!ifs) {
      return false;
   }

   boost::archive::text_iarchive ia (ifs);
   ia >> config;
   return true;
}

/*!
 * \brief Save a station config to a file.
 * \returns true if the config was saved.
 */
bool save_station_config (const fs::path &file, station_config &config) {
    std::ofstream ofs (file.c_str ());
    if (!ofs) {
        return false;
    }

    boost::archive::text_oarchive oa (ofs);
    oa << config;
    return true;
}

}
