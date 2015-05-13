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
cmake_minimum_required (VERSION 2.8)
project (genesis CXX)

include(ExternalProject)

# Build gnss-sdr
ExternalProject_Add(gnss-sdr
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gnss-sdr
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/gnss-sdr
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/gnss-sdr/build
  DOWNLOAD_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  INSTALL_COMMAND ""
  BUILD_COMMAND make
  INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/gnss-sdr/install)

ExternalProject_Get_Property(gnss-sdr binary_dir)
set(GNSS_SDR_LIBRARY_DIRS ${binary_dir})
set(GNSS_SDR_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/gnss-sdr/src)

# Find Boost
find_package(Boost COMPONENTS system thread date_time log REQUIRED)

# Compiler flags
set (MY_CXX_FLAGS_DEBUG "-DGENESIS_DEBUG")

if(CMAKE_COMPILER_IS_GNUCXX AND NOT WIN32)
  set(MY_CXX_FLAGS "${MY_CXX_FLAGS} -Wall -Wextra -Werror -pedantic-errors")
  set(MY_CXX_FLAGS_RELEASE "${MY_CXX_FLAGS_RELEASE} -Wno-unused")
endif(CMAKE_COMPILER_IS_GNUCXX AND NOT WIN32)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(MY_CXX_FLAGS "${MY_CXX_FLAGS} -stdlib=libc++")
  set(MY_CXX_FLAGS_RELEASE "${MY_CXX_FLAGS_RELEASE} -Wno-unused-private-field")
endif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MY_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${MY_CXX_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${MY_CXX_FLAGS_DEBUG}")


add_subdirectory (src)
