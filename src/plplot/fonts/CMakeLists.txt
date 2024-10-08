# fonts/CMakeLists.txt
# Copyright (C) 2006-2018 Alan W. Irwin
#
# This file is part of PLplot.
#
# PLplot is free software; you can redistribute it and/or modify
# it under the terms of the GNU Library General Public License as published
# by the Free Software Foundation; version 2 of the License.
#
# PLplot is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with PLplot; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

# N.B. the plhershey-unicode-gen stuff is taken care of by
# ../include/CMakeLists.txt.  This file only used to optionally build the
# Hershey fonts.

option(BUILD_HERSHEY_FONTS "Build Hershey fonts?" OFF)

if(BUILD_HERSHEY_FONTS)

  # Build Hershey fonts.

  # Create "convenience static library" to help build Hershey fonts.

  set(libhershey_SRC
    font01.c
    font02.c
    font03.c
    font04.c
    font05.c
    font06.c
    font07.c
    font08.c
    font09.c
    font10.c
    font11.c
    )

  include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/qsastime
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_BINARY_DIR}/lib/qsastime
    )
  add_library(hershey STATIC ${libhershey_SRC})

  add_executable(generate_stndfonts stndfont.c)

  target_link_libraries(generate_stndfonts hershey PLPLOT::plplot)

  add_executable(generate_xtndfonts xtndfont.c)

  target_link_libraries(generate_xtndfonts hershey PLPLOT::plplot)

endif(BUILD_HERSHEY_FONTS)
