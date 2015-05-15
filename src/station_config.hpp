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
#ifndef GENESIS_STATION_CONFIG_HPP
#define GENESIS_STATION_CONFIG_HPP


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/filesystem.hpp>

namespace genesis {

/*!
 * \brief Information about the station.
 */
class station_config {
   friend class boost::serialization::access;

   double if_bias_; // The IF bias recorded by front-end-cal


   template<class Archive>
   void serialize(Archive & ar, const unsigned int) {
       ar & if_bias_;
   }

public:

   inline explicit station_config (double if_bias = 0)
       : if_bias_ (if_bias)
      {
      }

   inline double if_bias () const {
       return if_bias_;
   }

   inline void set_if_bias (double bias) {
       if_bias_ = bias;
   }
};

/*!
 * \brief Load a station config from a file.
 * \returns true if the config was loaded.
 */
bool load_station_config (const boost::filesystem::path &file,
                          station_config &config);


/*!
 * \brief Save a station config to a file.
 * \returns true if the config was saved.
 */
bool save_station_config (const boost::filesystem::path &file,
                          station_config &config);

} // namespace genesis

#endif //GENESIS_STATION_CONFIG_HPP
