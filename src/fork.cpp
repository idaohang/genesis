/*!
 * \file fork.cpp
 * \brief An interface for forking.
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
#include "fork.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <boost/filesystem.hpp>

namespace genesis {

int forker::fork (fork_handler *handler,
	     const boost::filesystem::path &dir,
	     const boost::filesystem::path &cmd,
	     const std::vector <std::string> &args)
{
   handler->prepare_fork ();

   int p[2];
   pipe (p);

   pid_t pid = ::fork ();
   if (pid == 0) {
      handler->child_fork ();

      while ((dup2(p[1], STDERR_FILENO) == -1) && (errno == EINTR)) {}
      while ((dup2(p[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
      close(p[1]);
      close(p[0]);

      boost::filesystem::current_path (dir);

      char **argv = new char *[args.size () + 1];
      for (size_t i = 0; i < args.size (); i++) {
	 argv[i] = new char [args[i].length () + 1];
	 strcpy (argv[i], args[i].c_str ());
      }
      argv[args.size ()] = 0;

      execvp(cmd.c_str (), argv);
      exit (1);
   }
   handler->parent_fork (pid);
   close (p[1]);

   return p[0];
}

}
