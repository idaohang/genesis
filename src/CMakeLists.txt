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
  child.cpp
  session.cpp
  calibrate.cpp
  ${GNSS_SDR_SOURCE_DIR}/src/utils/front-end-cal/front_end_cal.cc)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/src/listen
  ${Boost_INCLUDE_DIRS}
  ${GFlags_INCLUDE_DIRS}
  ${GLOG_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIR}
  ${GNSS_SDR_SOURCE_DIR}/src/utils/front-end-cal
  ${GNSS_SDR_SOURCE_DIR}/src/core/system_parameters
  ${GNSS_SDR_SOURCE_DIR}/src/core/interfaces
  ${GNSS_SDR_SOURCE_DIR}/src/core/receiver
  ${GNSS_SDR_SOURCE_DIR}/src/core/libs
  ${GNSS_SDR_SOURCE_DIR}/src/core/libs/supl
  ${GNSS_SDR_SOURCE_DIR}/src/core/libs/supl/asn-rrlp
  ${GNSS_SDR_SOURCE_DIR}/src/core/libs/supl/asn-supl
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/libs
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/acquisition/adapters
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/acquisition/gnuradio_blocks
  )

link_directories (
  ${Boost_LIBRARY_DIR}
  ${GFlags_LIBRARY_PATH}
  ${OPENSSL_LIBRARY_PATH}
  ${GNSS_SDR_BINARY_DIR}/src/utils/front-end-cal
  ${GNSS_SDR_BINARY_DIR}/src/core/system_parameters
  ${GNSS_SDR_BINARY_DIR}/src/core/interfaces
  ${GNSS_SDR_BINARY_DIR}/src/core/receiver
  ${GNSS_SDR_BINARY_DIR}/src/core/libs
  ${GNSS_SDR_BINARY_DIR}/src/core/libs/supl
  ${GNSS_SDR_BINARY_DIR}/src/core/libs/supl/asn-rrlp
  ${GNSS_SDR_BINARY_DIR}/src/core/libs/supl/asn-supl
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/libs
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/acquisition/adapters
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/acquisition/gnuradio_blocks
)

file (GLOB GENESIS_HEADERS "*.hpp")
add_executable (genesis ${GENESIS_SOURCES} ${GENESIS_HEADERS})
target_link_libraries (
  genesis
  gnss_rx
  gnss_sp_libs
  rx_core_lib
  supl_library
  gnss_system_parameters
  ${Boost_LIBRARIES}
  ${GLOG_LIBRARIES}
  ${GFlags_LIBRARY}
  ${ARMADILLO_LIBRARIES}
  ${VOLK_GNSSSDR_LIBRARY} ${ORC_LIBRARIES}
  ${OPENSSL_LIBRARIES}
)

source_group (Headers FILES ${GENESIS_HEADERS})