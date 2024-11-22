# cmake/modules/TestForNamespace.cmake
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

# Check if "using namespace std;" works
IF(NOT DEFINED CMAKE_USE_NAMESPACE)
  MESSAGE(STATUS "Check for using namespace support")
  TRY_COMPILE(CMAKE_USE_NAMESPACE
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/src/plplot/modules/TestForNamespace.cxx
    OUTPUT_VARIABLE OUTPUT)
  IF (CMAKE_USE_NAMESPACE)
    MESSAGE(STATUS "Check for using namespace - found")
    SET (PL_USE_NAMESPACE 1 CACHE INTERNAL
      "Does the compiler support using namespace")
      FILE(APPEND ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeOutput.log
      "Determining if the CXX compiler has using namespace passed with "
      "the following output:\n${OUTPUT}\n\n")
  ELSE (CMAKE_USE_NAMESPACE)
    MESSAGE(STATUS "Check for using namespace - not found")
    SET (PL_USE_NAMESPACE 0 CACHE INTERNAL
      "Does the compiler support using namespace")
      FILE(APPEND ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
      "Determining if the CXX compiler has using namespace failed with "
      "the following output:\n${OUTPUT}\n\n")
  ENDIF (CMAKE_USE_NAMESPACE)
ENDIF(NOT DEFINED CMAKE_USE_NAMESPACE)


