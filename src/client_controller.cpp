/*!
 * \file client_controller.cpp
 * \brief Mechanism for controlling which stations are connected to
 * the receiver.
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

#include "client_controller.hpp"
#include "error.hpp"
#include <boost/range.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include <set>
#include "station.hpp"
#include "concurrent_shared_map.h"

namespace genesis {

namespace detail {

typedef client_controller::error_type error_type;
using namespace boost::system::errc;

bool validate_station (const station &st) {
   if (st.get_type () == station::STATION_TYPE_UNKNOWN) {
      return false;
   }

   if (st.get_address ().length () == 0) {
      return false;
   }

   if (st.get_port () == 0) {
      return false;
   }

   return true;
}

struct find_by_address {
   find_by_address (const std::string &address)
      : address_ (address)
   {
   }

   bool operator() (const station &st) {
      return st.get_address () == address_;
   }

private:
   const std::string &address_;
};

} //namespace detail

struct client_controller::impl {
   station base_;
   std::set<station> rovers_;
   client_controller::observable_vector base_observables_;
   client_controller::ref_time_ptr base_ref_time_;

   mutable boost::recursive_mutex mutex_;

   typedef boost::lock_guard <boost::recursive_mutex> lock;
};

client_controller::client_controller()
    : impl_ (new impl())
{
}

client_controller::~client_controller()
{
}


client_controller::error_type client_controller::add_station (
   const station &st)
{
   using namespace std;

   if (!detail::validate_station (st)) {
      return make_error_condition (invalid_station);
   }

   impl::lock lock (impl_->mutex_);
   if (st.get_type () == station::STATION_TYPE_ROVER) {
      if (st.get_address () == impl_->base_.get_address ()) {
         // already the base station
         return make_error_condition (station_is_base);
      }
      if (!impl_->rovers_.insert (st).second) {
         // already inserted
         return make_error_condition (station_exists);
      }
   }
   else {
      if (has_base ()) {
          if (impl_->base_ == st) {
              return make_error_condition (station_is_base);
          }
         return make_error_condition (base_already_set);
      }
      if (impl_->rovers_.find (st) != boost::end (impl_->rovers_)) {
         // already a rover
         return make_error_condition (station_is_rover);
      }

      impl_->base_ = st;
      impl_->base_observables_.clear ();
      impl_->base_ref_time_.reset ();
   }

   return error_type ();
}

client_controller::error_type
client_controller::remove_station (const station &st) {
   impl::lock lock (impl_->mutex_);
   if (impl_->base_.get_address () == st.get_address ()) {
      return reset_base ();
   }

   if (!impl_->rovers_.erase (st)) {
      // Not found
      return make_error_condition (station_not_found);
   }
   return error_type ();
}

bool client_controller::has_base () const {
   impl::lock lock (impl_->mutex_);
   return detail::validate_station (impl_->base_);
}

client_controller::error_type client_controller::reset_base () {
   impl::lock lock (impl_->mutex_);
   impl_->base_ = station ();
   impl_->base_observables_.clear ();
   impl_->base_ref_time_.reset ();
   return error_type ();
}

client_controller::ref_time_ptr
client_controller::base_ref_time () const {
   impl::lock lock (impl_->mutex_);
   if (!impl_->base_ref_time_) {
       impl_->base_ref_time_.reset (
           new concurrent_shared_map<Gps_Ref_Time> (
               "GNSS-SDR.base.gps_ref_time"));
   }
   return impl_->base_ref_time_;
}

client_controller::observable_vector
client_controller::base_observables () const {
   impl::lock lock (impl_->mutex_);
   return impl_->base_observables_;
}

void client_controller::set_base_observables (observable_vector v) {
   impl::lock lock (impl_->mutex_);
   impl_->base_observables_ = v;
}

}
