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

set (RTK_LIB_SOURCES
  rtkpos.c
  solution.c
  lambda.c
  sbas.c
  ppp.c
  ppp_ar.c
  ephemeris.c
  rtkcmn.c
  pntpos.c
  options.c
  geoid.c
  ionex.c
  datum.c
  tle.c
  preceph.c
  convkml.c
  convrnx.c
  rtcm.c
  rtcm2.c
  rtcm3.c
  rtcm3e.c
  rtksvr.c
  stream.c
  postpos.c
  download.c
  streamsvr.c
  qzslex.c
  rcvraw.c)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (RTK_LIB_LIBS "")
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set (RTK_LIB_LIBS m rt pthread)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set (RTK_LIB_LIBS m pthread)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

add_library (rtk_lib ${RTK_LIB_SOURCES})
target_link_libraries (rtk_lib ${RTK_LIB_LIBS})