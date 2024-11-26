# cmake/modules/TestForStdintCXX.cmake
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
if(NOT DEFINED CMAKE_CXX_STDINT_H)
  message(STATUS "Check for using stdint.h with CXX compiler")
  try_compile(CMAKE_CXX_STDINT_H
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/src/plplot/modules/TestForStdintCXX.cxx
    OUTPUT_VARIABLE OUTPUT)
  if(CMAKE_CXX_STDINT_H)
    message(STATUS "Check for using stdint.h with CXX compiler - ok")
    set(PL_HAVE_CXX_STDINT_H 1 CACHE INTERNAL
      "Does the CXX compiler support using stdint.h")
      file(APPEND ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeOutput.log
      "Determining if the CXX compiler supports using stdint.h passed with "
      "the following output:\n${OUTPUT}\n\n")
  else(CMAKE_CXX_STDINT_H)
    message(STATUS "Check for using stdint.h with CXX compiler - not found")
    set(PL_HAVE_CXX_STDINT_H 0 CACHE INTERNAL
      "Does the CXX compiler support using stdint.h")
      file(APPEND ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
      "Determining if the CXX compiler supports using stdint.h failed with "
      "the following output:\n${OUTPUT}\n\n")
  endif(CMAKE_CXX_STDINT_H)
endif(NOT DEFINED CMAKE_CXX_STDINT_H)
