# cmake/modules/TestBrokenIsnanCXX.cmake
#
# C++ binding configuration
#
# Copyright (C) 2006  Andrew Ross
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
# along with the file PLplot; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

# Check if isnan is in <math.h> but not in <cmath>
IF(NOT DEFINED CMAKE_BROKEN_ISNAN_CXX)
  MESSAGE(STATUS "Check for broken isnan support in <cmath>")
  TRY_COMPILE(CMAKE_BROKEN_ISNAN_CXX
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/src/plplot/modules/TestBrokenIsnanCXX.cxx
    OUTPUT_VARIABLE OUTPUT)
  IF (CMAKE_BROKEN_ISNAN_CXX)
    MESSAGE(STATUS "Check for isnan in <cmath> - found")
    SET (PL_BROKEN_ISNAN_CXX 0 CACHE INTERNAL
      "Does <cmath> contain isnan")
      FILE(APPEND ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeOutput.log
      "Determining if <cmake> contains isnan passed with "
      "the following output:\n${OUTPUT}\n\n")
  ELSE (CMAKE_BROKEN_ISNAN_CXX)
    MESSAGE(STATUS "Check for isnan in <cmath> - not found")
    SET (PL_BROKEN_ISNAN_CXX 1 CACHE INTERNAL
      "Does <cmath> contain isnan")
      FILE(APPEND ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
      "Determining if <cmath> contains isnan failed with "
      "the following output:\n${OUTPUT}\n\n")
  ENDIF (CMAKE_BROKEN_ISNAN_CXX)
ENDIF(NOT DEFINED CMAKE_BROKEN_ISNAN_CXX)
