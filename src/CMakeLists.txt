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
add_subdirectory (util)

set (GENESIS_SOURCES
  main.cpp
  error.cpp
  client_controller.cpp
  log.cpp
  packet.cpp
  service.cpp
  session.cpp
  calibrator.cpp)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${GFlags_INCLUDE_DIRS}
  )

link_directories (
  ${Boost_LIBRARY_DIR}
  ${GFlags_LIBRARY_PATH}
)

file (GLOB GENESIS_HEADERS "*.hpp")
add_executable (genesis ${GENESIS_SOURCES} ${GENESIS_HEADERS})
target_link_libraries (
  genesis
  ${Boost_LIBRARIES}
  ${GFlags_LIBS}
)

source_group (Headers FILES ${GENESIS_HEADERS})