# cmake/modules/summary.cmake
#
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
# along with the file PLplot; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

# Macro for outputting all the most important CMake variables for PLplot.

macro(summary)
set(_output_results "
Summary of CMake build system results for PLplot

Install location variables which can be set by the user.  N.B. These
variables are ordered by decreasing degree of generality, with the
_default_ values of the later ones in the list determined by the
values of variables earlier in the list.  So, for example, it is
usually sufficient in the vast majority of cases to just set
CMAKE_INSTALL_PREFIX, and the rest of these variables are adjusted
accordingly (at least for a fresh configuration), and it is rare
indeed that is is necessary for a user to set any variable here whose
name does not start with \"CMAKE_INSTALL_\".

")
message("${_output_results}")

foreach(INSTALL_LOCATION_VARIABLE CMAKE_INSTALL_PREFIX ${INSTALL_LOCATION_VARIABLES_LIST})
  message("${INSTALL_LOCATION_VARIABLE}: ${${INSTALL_LOCATION_VARIABLE}}")

  # Check for prefix consistency for the install locations.
  if(NOT "${${INSTALL_LOCATION_VARIABLE}}" MATCHES "^${CMAKE_INSTALL_PREFIX}.*")
    message(WARNING "${INSTALL_LOCATION_VARIABLE} = ${${INSTALL_LOCATION_VARIABLE}} has prefix inconsistency with CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
  endif(NOT "${${INSTALL_LOCATION_VARIABLE}}" MATCHES "^${CMAKE_INSTALL_PREFIX}.*")
endforeach(INSTALL_LOCATION_VARIABLE ${INSTALL_LOCATION_VARIABLES_LIST})

set(
_output_results
"

Other important CMake variables:

CMAKE_SYSTEM_NAME:	${CMAKE_SYSTEM_NAME}
UNIX:			${UNIX}
WIN32:			${WIN32}
APPLE:			${APPLE}
MSVC:			${MSVC}	(MSVC_VERSION:	${MSVC_VERSION})
MINGW:			${MINGW}
MSYS:			${MSYS}
CYGWIN:			${CYGWIN}
BORLAND:		${BORLAND}
WATCOM:		  ${WATCOM}

SWIG_FOUND:		${SWIG_FOUND}
PERL_FOUND:		${PERL_FOUND}
X11_FOUND:		${X11_FOUND}

CMAKE_BUILD_TYPE:	${CMAKE_BUILD_TYPE}
CMAKE_C_COMPILER CMAKE_C_FLAGS:			${CMAKE_C_COMPILER} ${CMAKE_C_FLAGS}")

if(CMAKE_CXX_COMPILER_WORKS)
set(
_output_results
"${_output_results}
CMAKE_CXX_COMPILER CMAKE_CXX_FLAGS:		${CMAKE_CXX_COMPILER} ${CMAKE_CXX_FLAGS}")
endif(CMAKE_CXX_COMPILER_WORKS)

if(ENABLE_fortran)
set(
_output_results
"${_output_results}
CMAKE_Fortran_COMPILER CMAKE_Fortran_FLAGS:	${CMAKE_Fortran_COMPILER} ${CMAKE_Fortran_FLAGS}
Target Fortran:	${TARGET_FORTRAN}")
endif(ENABLE_fortran)

if(ENABLE_python)
set(
_output_results
"${_output_results}
PYTHON_EXECUTABLE:				${PYTHON_EXECUTABLE}
PYTHON_INCLUDE_PATH:				${PYTHON_INCLUDE_PATH}
PYTHON_LIBRARIES:				${PYTHON_LIBRARIES}
NUMPY_INCLUDE_PATH:				${NUMPY_INCLUDE_PATH}")
endif(ENABLE_python)

set(
_output_results
"${_output_results}

ENABLE_DYNDRIVERS:	${ENABLE_DYNDRIVERS}
DRIVERS_LIST: ${DRIVERS_LIST}
DEVICES_LIST: ${DEVICES_LIST}

Library options:
BUILD_SHARED_LIBS:	${BUILD_SHARED_LIBS}		PL_DOUBLE:	${PL_DOUBLE}

Optional libraries:
PL_HAVE_QHULL:		${PL_HAVE_QHULL}		WITH_CSA:	${WITH_CSA}
PL_HAVE_FREETYPE:	${PL_HAVE_FREETYPE}		PL_HAVE_PTHREAD:	${PL_HAVE_PTHREAD}
HAVE_AGG:		${HAVE_AGG}		HAVE_SHAPELIB:	${HAVE_SHAPELIB}

Language Bindings:
ENABLE_ada:		${ENABLE_ada}
ENABLE_cxx:		${ENABLE_cxx}
ENABLE_d:		${ENABLE_d}
ENABLE_fortran:		${ENABLE_fortran}
ENABLE_java:		${ENABLE_java}
ENABLE_lua:		${ENABLE_lua}
ENABLE_ocaml:		${ENABLE_ocaml}
ENABLE_octave:		${ENABLE_octave}
ENABLE_python:		${ENABLE_python}
ENABLE_qt:		${ENABLE_qt}")

if(PLPLOT_USE_QT5)
  set(
_output_results
"${_output_results}
ENABLE_pyqt5:		${ENABLE_pyqt5}")
else(PLPLOT_USE_QT5)
  set(
_output_results
"${_output_results}
ENABLE_pyqt4:		${ENABLE_pyqt4}")
endif(PLPLOT_USE_QT5)

set(
_output_results
"${_output_results}
ENABLE_tcl:		${ENABLE_tcl}
ENABLE_itcl:		${ENABLE_itcl}
ENABLE_tk:		${ENABLE_tk}
ENABLE_itk:		${ENABLE_itk}
ENABLE_wxwidgets:	${ENABLE_wxwidgets}
")
message("${_output_results}")
endmacro(summary)
