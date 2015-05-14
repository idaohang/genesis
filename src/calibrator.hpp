/*!
 * \file calibrator.hpp
 * \brief Interface for a front end calibration service.
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
#ifndef GENESIS_CALIBRATOR_HPP
#define GENESIS_CALIBRATOR_HPP

#include "error.hpp"
#include <boost/move/core.hpp>
#include <boost/function.hpp>
#include "station.hpp"
#include "log.hpp"

namespace genesis {

/*!
 * \brief This class attempts to determine the IF of the
 * front end.
 */
class calibrator {
   BOOST_MOVABLE_BUT_NOT_COPYABLE (calibrator)
public:
   typedef boost::system::error_condition error_type;

   inline calibrator ()
      : IF_ (0)
   {
   }

   template <typename Ffork, typename Fcfork, typename Fpfork>
   error_type calibrate (
      const station &st,
      Ffork prepare_fork,
      Fcfork child_fork,
      Fpfork parent_fork)
   {
      return calibrate_impl (st, prepare_fork, child_fork, parent_fork);
   }

   inline double get_IF () const {
      return IF_;
   }
private:
   error_type calibrate_impl (
      const station &st,
      boost::function <void ()> prepare_fork,
      boost::function <void ()> child_fork,
      boost::function <void (int)> parent_fork);
private:
   double IF_;
   logger lg_;
};

}

#endif // GENESIS_CALIBRATOR_HPP
