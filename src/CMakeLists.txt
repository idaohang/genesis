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
  ${ARMADILLO_INCLUDE_DIRS}
  ${GNURADIO_RUNTIME_INCLUDE_DIRS}
  ${GNURADIO_BLOCKS_INCLUDE_DIRS}
  ${VOLK_GNSSSDR_INCLUDE_DIRS}
  ${GNSS_SDR_SOURCE_DIR}/src/utils/front-end-cal
  ${GNSS_SDR_SOURCE_DIR}/src/core/receiver
  ${GNSS_SDR_SOURCE_DIR}/src/core/system_parameters
  ${GNSS_SDR_SOURCE_DIR}/src/core/interfaces
  ${GNSS_SDR_SOURCE_DIR}/src/core/libs
  ${GNSS_SDR_SOURCE_DIR}/src/core/libs/supl
  ${GNSS_SDR_SOURCE_DIR}/src/core/libs/supl/asn-rrlp
  ${GNSS_SDR_SOURCE_DIR}/src/core/libs/supl/asn-supl
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/libs
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/signal_source/libs
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/signal_source/adapters
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/signal_source/gnuradio_blocks
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/output_filter/adapters
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/channel/adapters
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/channel/libs
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/conditioner/adapters
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/data_type_adapter/adapters
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/data_type_adapter/gnuradio_blocks
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/resampler/adapters
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/input_filter/adapters
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/input_filter/gnuradio_blocks
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/acquisition/adapters
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/acquisition/gnuradio_blocks
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/tracking/adapters
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/tracking/gnuradio_blocks
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/tracking/libs
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/telemetry_decoder/adapters
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/telemetry_decoder/gnuradio_blocks
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/telemetry_decoder/libs
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/observables/adapters
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/observables/gnuradio_blocks
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/PVT/adapters
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/PVT/gnuradio_blocks
  ${GNSS_SDR_SOURCE_DIR}/src/algorithms/PVT/libs
  )

link_directories (
  ${Boost_LIBRARY_DIR}
  ${GFlags_LIBRARY_PATH}
  ${OPENSSL_LIBRARY_PATH}
  ${GNSS_SDR_BINARY_DIR}/src/core/receiver
  ${GNSS_SDR_BINARY_DIR}/src/core/system_parameters
  ${GNSS_SDR_BINARY_DIR}/src/core/interfaces
  ${GNSS_SDR_BINARY_DIR}/src/core/libs
  ${GNSS_SDR_BINARY_DIR}/src/core/libs/supl
  ${GNSS_SDR_BINARY_DIR}/src/core/libs/supl/asn-rrlp
  ${GNSS_SDR_BINARY_DIR}/src/core/libs/supl/asn-supl
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/libs
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/signal_source/libs
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/signal_source/adapters
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/signal_source/gnuradio_blocks
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/output_filter/adapters
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/channel/adapters
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/channel/libs
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/conditioner/adapters
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/data_type_adapter/adapters
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/data_type_adapter/gnuradio_blocks
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/resampler/adapters
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/resampler/gnuradio_blocks
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/input_filter/adapters
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/input_filter/gnuradio_blocks
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/acquisition/adapters
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/acquisition/gnuradio_blocks
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/tracking/adapters
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/tracking/gnuradio_blocks
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/tracking/libs
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/telemetry_decoder/adapters
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/telemetry_decoder/gnuradio_blocks
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/telemetry_decoder/libs
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/observables/adapters
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/observables/gnuradio_blocks
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/PVT/adapters
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/PVT/gnuradio_blocks
  ${GNSS_SDR_BINARY_DIR}/src/algorithms/PVT/libs
)

if (ENABLE_UHD)
  set (OPTIONAL_UHD_LIBS ${UHD_LIBRARIES} ${GNURADIO_UHD_LIBRARIES})
endif (ENABLE_UHD)

file (GLOB GENESIS_HEADERS "*.hpp")
add_executable (genesis ${GENESIS_SOURCES} ${GENESIS_HEADERS})
target_link_libraries (
  genesis
  gnss_rx
  gnss_sp_libs
  signal_source_adapters
  signal_source_lib
  signal_source_gr_blocks
  datatype_adapters
  data_type_gr_blocks
  input_filter_adapters
  input_filter_gr_blocks
  conditioner_adapters
  resampler_adapters
  resampler_gr_blocks
  acq_adapters
  acq_gr_blocks
  tracking_lib
  tracking_adapters
  tracking_gr_blocks
  telemetry_decoder_adapters
  telemetry_decoder_lib
  telemetry_decoder_gr_blocks
  channel_adapters
  channel_fsm
  obs_adapters
  obs_gr_blocks
  pvt_adapters
  pvt_lib
  pvt_gr_blocks
  out_adapters
  rx_core_lib
  supl_library
  gnss_sp_libs
  gnss_system_parameters
  gnss_rx
  ${Boost_LIBRARIES}
  ${GLOG_LIBRARIES}
  ${GFlags_LIBRARY}
  ${ARMADILLO_LIBRARIES}
  ${VOLK_GNSSSDR_LIBRARY} ${ORC_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${GNURADIO_BLOCKS_LIBRARIES}
  ${GNURADIO_FFT_LIBRARIES}
  ${GNURADIO_FILTER_LIBRARIES}
  ${OPTIONAL_UHD_LIBS}
  ${VOLK_LIBRARIES}
)

source_group (Headers FILES ${GENESIS_HEADERS})