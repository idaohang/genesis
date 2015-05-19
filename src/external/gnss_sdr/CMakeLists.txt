# Copyright (C) 2010-2015 (see AUTHORS file for a list of contributors)
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

set (GNSS_SDR_LIB_SOURCES
  galileo_almanac.cc
  galileo_iono.cc
  galileo_ephemeris.cc
  galileo_utc_model.cc
  gps_almanac.cc
  gps_ephemeris.cc
  gps_utc_model.cc
  gps_iono.cc
  gps_ref_time.cc
  gps_ref_location.cc
  sbas_ionospheric_correction.cc
  sbas_satellite_correction.cc
  sbas_ephemeris.cc
  gnss_satellite.cc)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${GFlags_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIR})

add_library (gnss_sdr_lib ${GNSS_SDR_LIB_SOURCES})

target_link_libraries(gnss_sdr_lib
  ${Boost_LIBRARIES})