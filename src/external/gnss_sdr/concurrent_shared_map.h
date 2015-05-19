/*!
 * \file concurrent_map.h
 * \brief Interface of a thread-safe std::map
 * \author Anthony Arnold, 2015. anthony.arnold(at)uqconnect.edu.au
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

#ifndef GNSS_SDR_CONCURRENT_SHARED_MAP_H
#define GNSS_SDR_CONCURRENT_SHARED_MAP_H

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <utility>
#include "concurrent_dictionary.h"

template<typename Data>


/*!
 * \brief This class implements a thread-safe boost::interprocess::map
 *
 */
class concurrent_shared_map
   : public concurrent_dictionary <Data>
{
   typedef std::pair<const int, Data> value_type;
   typedef boost::interprocess::shared_memory_object shmem_object;
   typedef boost::interprocess::managed_shared_memory segment_type;
   typedef boost::interprocess::allocator <value_type,
					   segment_type::segment_manager
					   > shmem_allocator;

   typedef boost::interprocess::map <int,
				     Data,
				     std::less <int>,
				     shmem_allocator> map_type;

   typedef boost::interprocess::named_mutex mutex_type;
   typedef boost::interprocess::scoped_lock<mutex_type> scoped_lock;
   typedef typename map_type::iterator iterator_type;
private:
   struct shm_remove
   {
      shm_remove (const std::string &name) : name_ (name)
      {
	 shmem_object::remove (name_.c_str ());
	 mutex_type::remove ((name_ + "_LOCK").c_str ());
      }
      ~shm_remove ()
      {
	 shmem_object::remove (name_.c_str ());
	 mutex_type::remove ((name_ + "_LOCK").c_str ());
      }

   private:
      std::string name_;
   };

   shm_remove remover;
   segment_type segment;
   shmem_allocator allocator;
   map_type *the_map;
   mutex_type the_mutex;
public:
   concurrent_shared_map (const std::string &name, size_t segment_size)
      :
      remover (name),
      segment (boost::interprocess::create_only, name.c_str (), segment_size),
      allocator (segment.get_segment_manager ()),
      the_map (segment.construct<map_type>("the_map") (std::less<int>(), allocator)),
      the_mutex (boost::interprocess::create_only, (name + "_LOCK").c_str ())
   {
   }


   void write(int key, Data const& data)
   {
      scoped_lock lock(the_mutex);
      iterator_type data_iter;
      data_iter = the_map->find(key);
      if (data_iter != the_map->end())
      {
	 data_iter->second = data; // update
      }
      else
      {
	 the_map->insert(std::pair<int, Data>(key, data)); // insert SILENTLY fails if the item already exists in the map!
      }
   }

   std::map<int,Data> get_map_copy()
   {
      std::map <int, Data> the_copy;
      {
	 scoped_lock lock(the_mutex);
	 BOOST_FOREACH (const value_type &value, *the_map) {
	    the_copy.insert (value);
	 }
      }
      return the_copy;
   }

   int size()
   {
      scoped_lock lock(the_mutex);
      return the_map->size();
   }

   bool read(int key, Data& p_data)
   {
      scoped_lock lock(the_mutex);
      iterator_type data_iter;
      data_iter = the_map->find(key);
      if (data_iter != the_map->end())
      {
	 p_data = data_iter->second;
	 return true;
      }
      else
      {
	 return false;
      }
   }
};

#endif
