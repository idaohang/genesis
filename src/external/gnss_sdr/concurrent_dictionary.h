/*!
 * \file concurrent_dictionary.h
 * \brief Interface of a thread-safe dictionary.
 * \author Javier Arribas, 2011. jarribas(at)cttc.es
 *         Anthony ARnold, 2015. anthony.arnold(at)uqconnect.edu.au
 *
 * -------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2015  (see AUTHORS file for a list of contributors)
 *
 * GNSS-SDR is a software defined Global Navigation
 *          Satellite Systems receiver
 *
 * This file is part of GNSS-SDR.
 *
 * GNSS-SDR is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GNSS-SDR is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNSS-SDR. If not, see <http://www.gnu.org/licenses/>.
 *
 * -------------------------------------------------------------------------
 */

#ifndef GNSS_SDR_CONCURRENT_DICTIONARY_H
#define GNSS_SDR_CONCURRENT_DICTIONARY_H

#include <map>

template<typename Data>
/*!
 * \brief This class is a pure virtual interface to a dictionary type.
 * Implementations should handle concurrency.
 *
 */
class concurrent_dictionary
{
public:
   virtual void write(int key, Data const& data) = 0;

   virtual std::map <int, Data> get_map_copy() = 0;

   virtual int size() = 0;

   virtual bool read(int key, Data& p_data) = 0;
};

#endif /* GNSS_SDR_CONCURRENT_DICTIONARY_H */
