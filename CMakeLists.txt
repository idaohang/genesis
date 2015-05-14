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

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/gnss-sdr/cmake/Modules)

include(ExternalProject)


# gnss-sdr dependencies
# Look for them first, and if they're not found then
# assume gnss-sdr is fetching them
set(LOCAL_GFLAGS false)
set(gflags_RELEASE 2.1.2)
find_package(GFlags)

if (NOT GFlags_FOUND)
  set (LOCAL_GFLAGS true)
endif (NOT GFlags_FOUND)

find_package(GLOG)
if(GLOG_INCLUDE_DIRS)
  set(GLOG_FOUND ON)
endif(GLOG_INCLUDE_DIRS)
set(glog_RELEASE 0.3.4)


find_library(LAPACK lapack)
if(NOT LAPACK)
  message(FATAL_ERROR "LAPACK is required to build Genesis")
endif(NOT LAPACK)
find_library(BLAS blas)
find_package(Armadillo)

set(GR_REQUIRED_COMPONENTS RUNTIME ANALOG BLOCKS DIGITAL FFT FILTER PMT FEC TRELLIS UHD)
find_package(Gnuradio)
if(PC_GNURADIO_RUNTIME_VERSION)
  if(PC_GNURADIO_RUNTIME_VERSION VERSION_LESS 3.7.3)
    set(GNURADIO_RUNTIME_FOUND)
    message(STATUS "The GNU Radio version installed in your system is too old.")
  endif(PC_GNURADIO_RUNTIME_VERSION VERSION_LESS 3.7.3)
endif(PC_GNURADIO_RUNTIME_VERSION)
if(NOT GNURADIO_RUNTIME_FOUND)
  message(STATUS "CMake cannot find GNU Radio >= 3.7.3")
endif(NOT GNURADIO_RUNTIME_FOUND)

if(NOT GNURADIO_BLOCKS_FOUND)
  message(FATAL_ERROR "*** gnuradio-blocks 3.7 or later is required to build Genesis")
endif()
if(NOT GNURADIO_FILTER_FOUND)
  message(FATAL_ERROR "*** gnuradio-filter 3.7 or later is required to build Genesis")
endif()
if(NOT GNURADIO_FFT_FOUND)
  message(FATAL_ERROR "*** gnuradio-fft 3.7 or later is required to build Genesis")
endif()
if(NOT GNURADIO_ANALOG_FOUND)
  message(FATAL_ERROR "*** gnuradio-analog 3.7 or later is required to build Genesis")
endif()
if(NOT GNURADIO_FEC_FOUND)
  message(FATAL_ERROR "*** gnuradio-fec 3.7 or later is required to build Genesis")
endif()
if(NOT GNURADIO_TRELLIS_FOUND)
  message(FATAL_ERROR "*** gnuradio-trellis 3.7 or later is required to build Genesis")
endif()

find_package(UHD)
if(NOT UHD_FOUND)
  set(ENABLE_UHD OFF)
else(NOT UHD_FOUND)
  if(NOT GNURADIO_UHD_FOUND)
    message(FATAL_ERROR "*** gnuradio-uhd 3.7 or later is required to build Genesis")
  endif(NOT GNURADIO_UHD_FOUND)
  set(ENABLE_UHD ON)
endif(NOT UHD_FOUND)

find_package(Volk)
if(NOT VOLK_FOUND)
  message(FATAL_ERROR "*** VOLK is required to build Genesis")
endif()

find_package(VolkGnssSdr)
find_package(ORC)
if(NOT ORC_FOUND)
  set(ORC_LIBRARIES "")
  set(ORC_INCLUDE_DIRS "")
endif(NOT ORC_FOUND)


find_package(OpenSSL)
if(NOT OPENSSL_FOUND)
  message(FATAL_ERROR "OpenSSL libraries are required to build Genesis")
endif(NOT OPENSSL_FOUND)

# Build gnss-sdr
ExternalProject_Add(gnss-sdr
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gnss-sdr
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/gnss-sdr
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/gnss-sdr/build
  CMAKE_ARGS -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DENABLE_OSMOSDR=OFF -DENABLE_UHD=OFF
  DOWNLOAD_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  INSTALL_COMMAND ""
  BUILD_COMMAND make)

ExternalProject_Get_Property(gnss-sdr binary_dir)
set(GNSS_SDR_BINARY_DIR ${binary_dir})

ExternalProject_Get_Property(gnss-sdr source_dir)
set(GNSS_SDR_SOURCE_DIR ${source_dir})

# Set up dependency paths
if (NOT GFlags_FOUND)
  set (GFlags_INCLUDE_DIRS
    ${GNSS_SDR_BINARY_DIR}/gflags-${gflags_RELEASE}/include CACHE PATH "Local Gflags headers")
  set(GFlags_LIBRARY_PATH ${GNSS_SDR_BINARY_DIR}/gflags-${gflags_RELEASE}/lib )
  set(GFlags_LIBRARY ${GFlags_LIBRARY_PATH}/${CMAKE_FIND_LIBRARY_PREFIXES}gflags.a)
endif (NOT GFlags_FOUND)


if (NOT GLOG_FOUND OR ${LOCAL_GFLAGS})
  set(GLOG_INCLUDE_DIRS
    ${GNSS_SDR_SOURCE_DIR}/thirdparty/glog/glog-${glog_RELEASE}/src/
    ${GNSS_SDR_BINARY_DIR}/glog-${glog_RELEASE}/src
    )
  set(GLOG_LIBRARIES
    ${GNSS_SDR_BINARY_DIR}/glog-${glog_RELEASE}/.libs/${CMAKE_FIND_LIBRARY_PREFIXES}glog.a
    )
endif (NOT GLOG_FOUND OR ${LOCAL_GFLAGS})


if(NOT ARMADILLO_FOUND)
  find_library(GFORTRAN NAMES gfortran)
  if (NOT GFORTRAN)
    message (STATUS "gfortran was not found and will not be linked")
    set (GFORTRAN "")
  endif (NOT GFORTRAN)
  set (armadillo_RELEASE 5.100.1)
  set (ARMADILLO_SRC_DIR ${GNSS_SDR_BINARY_DIR}/armadillo-${armadillo_RELEASE}/src/armadillo-${armadillo_RELEASE})
  set (ARMADILLO_INCLUDE_DIRS ${ARMADILLO_SRC_DIR}/include)
  set(ARMADILLO_LIBRARIES ${BLAS} ${LAPACK} ${GFORTRAN} ${ARMADILLO_SRC_DIR}/${CMAKE_FIND_LIBRARY_PREFIXES}armadillo.a)
endif(NOT ARMADILLO_FOUND)


if(NOT VOLK_GNSSSDR_FOUND)
  set(VOLK_GNSSSDR_LIBRARY "${GNSS_SDR_BINARY_DIR}/volk_gnsssdr_module/install/lib/${CMAKE_FIND_LIBRARY_PREFIXES}volk_gnsssdr.a")
  set(VOLK_GNSSSDR_INCLUDE_DIRS "${GNSS_SDR_BINARY_DIR}/volk_gnsssdr_module/build/include/;${GNSS_SDR_SOURCE_DIR}/src/algorithms/libs/volk_gnsssdr_module/volk_gnsssdr/include;${ORC_INCLUDE_DIRS}")
endif(NOT VOLK_GNSSSDR_FOUND)

# Find Boost
find_package(Boost COMPONENTS
  system thread date_time log filesystem serialization chrono REQUIRED)

# Compiler flags
set (MY_CXX_FLAGS_DEBUG "-DGENESIS_DEBUG")

if(CMAKE_COMPILER_IS_GNUCXX AND NOT WIN32)
  set(MY_CXX_FLAGS "${MY_CXX_FLAGS} -Wall -Wextra -std=c++11")
  set(MY_CXX_FLAGS_RELEASE "${MY_CXX_FLAGS_RELEASE} -Wno-unused")
endif(CMAKE_COMPILER_IS_GNUCXX AND NOT WIN32)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(MY_CXX_FLAGS "${MY_CXX_FLAGS} -stdlib=libc++ -std=c++11")
  set(MY_CXX_FLAGS_RELEASE "${MY_CXX_FLAGS_RELEASE} -Wno-unused-private-field")
endif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MY_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${MY_CXX_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${MY_CXX_FLAGS_DEBUG}")


add_subdirectory (src)
