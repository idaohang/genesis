/*!
 * \file fork_handler.hpp
 * \brief An interface for fork handling.
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
#ifndef GENESIS_FORK_HANDLER_HPP
#define GENESIS_FORK_HANDLER_HPP

namespace genesis {

/*!
 * \brief Interface for a fork handler.
 */
class fork_handler {
public:
   // Call this before forking
   virtual void prepare_fork () = 0;

   // Call this after forking, in the child process
   virtual void child_fork () = 0;

   // Call this after forking, in the parent process
   virtual void parent_fork (int pid) = 0;
};

}

#endif // GENESIS_FORK_HANDLER_HPP
