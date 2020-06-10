#
# copyright : (c) 2010 Maxime Lenoir, Alain Coulais,
#                      Sylwester Arabas and Orion Poplawski
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#

# - Find numpy
# Find the native numpy includes
# This module defines
#  PYTHON_NUMPY_INCLUDE_DIR, where to find numpy/arrayobject.h, etc.
#  PYTHON_NUMPY_FOUND, If false, do not try to use numpy headers.

if (PYTHON_NUMPY_INCLUDE_DIR)
  # in cache already
  set (PYTHON_NUMPY_FIND_QUIETLY TRUE)
endif (PYTHON_NUMPY_INCLUDE_DIR)

#INCLUDE(FindPythonInterp) #SA: this is now done in CMakeLists.txt

IF(PYTHON_EXECUTABLE)
    execute_process (COMMAND "${PYTHON_EXECUTABLE}" "-c" "import numpy; print(numpy.get_include(), end='')"
      OUTPUT_VARIABLE PYTHON_NUMPY_INCLUDE_DIR
      RESULT_VARIABLE PYTHON_NUMPY_NOT_FOUND)

    if (PYTHON_NUMPY_NOT_FOUND)
      set(PYTHON_NUMPY_FOUND FALSE)
    else()
      set (PYTHON_NUMPY_FOUND TRUE)
      set (PYTHON_NUMPY_INCLUDE_DIR ${PYTHON_NUMPY_INCLUDE_DIR} CACHE STRING "Numpy include path")
    endif()
else()
  message(FATAL_ERROR "PYTHON_EXECUTABLE not defined!")
ENDIF(PYTHON_EXECUTABLE)

if (PYTHON_NUMPY_FOUND)
  if (NOT PYTHON_NUMPY_FIND_QUIETLY)
    message (STATUS "Numpy headers found")
  endif (NOT PYTHON_NUMPY_FIND_QUIETLY)
else (PYTHON_NUMPY_FOUND)
  if (PYTHON_NUMPY_FIND_REQUIRED)
    message (FATAL_ERROR "Numpy headers missing")
  endif (PYTHON_NUMPY_FIND_REQUIRED)
endif (PYTHON_NUMPY_FOUND)

MARK_AS_ADVANCED (PYTHON_NUMPY_INCLUDE_DIR)
