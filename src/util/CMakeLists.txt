# Copyright (C) Anthony Arnold 2015
#
# This file is part of Genesis.
#
# Genesis is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Genesis is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Genesis. If not, see <http://www.gnu.org/licenses/>.
#
if (MAKE_GENESIS_PING)
set (GENESIS_PING_SOURCES
  genesis_ping.cpp)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS}
  )

add_executable (genesis_ping ${GENESIS_PING_SOURCES})
target_link_libraries (
  genesis_ping
  ${Boost_LIBRARIES})

install (TARGETS genesis_ping RUNTIME DESTINATION bin)
endif (MAKE_GENESIS_PING)