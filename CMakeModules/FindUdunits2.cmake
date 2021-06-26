#
# copyright : (c) 2010 Maxime Lenoir, Alain Coulais,
#                      Sylwester Arabas and Orion Poplawski
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
# ------------
# AC 2020-03-30 : It was not working on some OS (F28, U1604 ...)
# stoled here :
# https://ftp.space.dtu.dk/pub/Ioana/pism0.6.1-10/CMake/FindUDUNITS2.cmake
# reduced (we don't need to test LD_LIBRARY_PATH or EXPAT)
#
# tested on U 14.04 & 16.04, Debian 9 & 10, Fedora 28 & RHL 7

#find_library(UDUNITS2_LIBRARIES NAMES udunits2)
#find_path(UDUNITS2_INCLUDE_DIR NAMES udunits2.h)

if (UDUNITS2_INCLUDE_DIR)
  # Already in cache, be silent
  set (UDUNITS2_FIND_QUIETLY TRUE)
endif (UDUNITS2_INCLUDE_DIR)

find_path (UDUNITS2_INCLUDE_DIR udunits2.h
  HINTS "${UDUNITS2_ROOT}/include" "$ENV{UDUNITS2_ROOT}/include"
  PATH_SUFFIXES "udunits2"
  DOC "Path to udunits2.h")

# UDUNITS2 headers might be in .../include or .../include/udunits2.
# We try both.
if (${UDUNITS2_INCLUDE_DIR} MATCHES "udunits2/?$")
  string(REGEX REPLACE "/include/udunits2/?$" "/lib"
    UDUNITS2_LIB_HINT ${UDUNITS2_INCLUDE_DIR})
else()
  string(REGEX REPLACE "/include/?$" "/lib"
    UDUNITS2_LIB_HINT ${UDUNITS2_INCLUDE_DIR})
endif()

find_library (UDUNITS2_LIBRARIES
  NAMES udunits2
  HINTS ${UDUNITS2_LIB_HINT})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Udunits2 DEFAULT_MSG UDUNITS2_LIBRARIES UDUNITS2_INCLUDE_DIR)

mark_as_advanced(UDUNITS2_LIBRARIES UDUNITS2_INCLUDE_DIR)
