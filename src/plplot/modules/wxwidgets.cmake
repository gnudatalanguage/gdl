# cmake/modules/wxwidgets.cmake
#
# Copyright (C) 2006 Werner Smekal
# Copyright (C) 2015-2017 Alan W. Irwin
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

# Module for determining all configuration variables related to the wxwidgets
# device driver.
# The following variables are set/modified:
# PLD_wxwidgets		  - ON means the wxwidgets device is enabled.
# wxwidgets_COMPILE_FLAGS	  - individual COMPILE_FLAGS required to compile wxwidgets
# 			    device.
# wxwidgets_LINK_FLAGS	  - list of full path names of libraries and
# 			    linker flags for dynamic wxwidgets device driver.
# wxwidgets_RPATH	  - rpath (if needed) for anything linked to the
#                           wxwidgets libraries.
# DRIVERS_LINK_FLAGS  	  - list of device LINK_FLAGS for case
# 			    when ENABLE_DYNDRIVERS OFF.
# Find wxWidgets needed for driver and binding

if(PLD_wxwidgets)
  if(FORCE_EXTERNAL_STATIC)
    set(wxWidgets_USE_STATIC ON)
  endif(FORCE_EXTERNAL_STATIC)
#  find_package(wxWidgets 3.0.0 COMPONENTS base core adv)
  if(WIN32)
    list(APPEND wxWidgets_DEFINITIONS UNICODE _UNICODE)
  endif(WIN32)

  message(STATUS "wxWidgets_FOUND             : ${wxWidgets_FOUND}")
  message(STATUS "wxWidgets_INCLUDE_DIRS      : ${wxWidgets_INCLUDE_DIRS}")
  message(STATUS "wxWidgets_LIBRARIES         : ${wxWidgets_LIBRARIES}")
  message(STATUS "wxWidgets_LIBRARY_DIRS      : ${wxWidgets_LIBRARY_DIRS}")
  message(STATUS "wxWidgets_DEFINITIONS       : ${wxWidgets_DEFINITIONS}")
  message(STATUS "wxWidgets_DEFINITIONS_DEBUG : ${wxWidgets_DEFINITIONS_DEBUG}")
  # Convert wxWidgets_LIBRARIES to full pathname form.
  #cmake_link_flags(wxwidgets_LINK_FLAGS "${wxWidgets_LIBRARIES}")
  if(NOT wxWidgets_FOUND ) #OR NOT wxwidgets_LINK_FLAGS)
    message(STATUS
      "WARNING: wxWidgets or its libraries not found so "
      "setting all wxwidgets devices to OFF."
      )
    set(PLD_wxwidgets OFF CACHE BOOL "Enable wxwidgets device" FORCE)
  else(NOT wxWidgets_FOUND ) #OR NOT wxwidgets_LINK_FLAGS)
    # Check if stdint.h can be used from c++ (PL_HAVE_CXX_STDINT_H)
    include(TestForStdintCXX)
  endif(NOT wxWidgets_FOUND ) #OR NOT wxwidgets_LINK_FLAGS)
endif(PLD_wxwidgets)

if(PLD_wxwidgets)
  string(REGEX REPLACE ";" " -I"
    wxwidgets_COMPILE_FLAGS
    "-I${wxWidgets_INCLUDE_DIRS}"
    )
  string(REGEX REPLACE ";" " -D"
    wxWidgets_DEFINITIONS_wD
    "-D${wxWidgets_DEFINITIONS}"
    )
  set(wxWidgets_DEFINITIONS_DEBUG_wD "")
  if(wxWidgets_DEFINITIONS_DEBUG AND CMAKE_BUILD_TYPE MATCHES "Debug")
    string(REGEX REPLACE ";" " -D"
      wxWidgets_DEFINITIONS_DEBUG_wD
      "-D${wxWidgets_DEFINITIONS_DEBUG}"
      )
  endif(wxWidgets_DEFINITIONS_DEBUG AND CMAKE_BUILD_TYPE MATCHES "Debug")
  string(REGEX REPLACE ";" " "
    wxwidgets_COMPILE_FLAGS
    ${wxwidgets_COMPILE_FLAGS}
    " ${wxWidgets_DEFINITIONS_wD} ${wxWidgets_DEFINITIONS_DEBUG_wD}"
    )
  message(STATUS "wxwidgets_COMPILE_FLAGS = ${wxwidgets_COMPILE_FLAGS}")
  message(STATUS "wxwidgets_LINK_FLAGS = ${wxwidgets_LINK_FLAGS}")
  set(wxwidgets_RPATH ${wxWidgets_LIBRARY_DIRS})
  #filter_rpath(wxwidgets_RPATH)
  if(wxwidgets_RPATH)
    message(STATUS "wxwidgets_RPATH = ${wxwidgets_RPATH}")
  endif(wxwidgets_RPATH)

  option(OLD_WXWIDGETS "Use old version of wxwidgets device driver, binding, and example" OFF)

    if(WITH_FREETYPE)
	  message(STATUS "WARNING: old wxwidgets driver and binding components depending on AGG library have been dropped.")
      set(
	wxwidgets_COMPILE_FLAGS
	"${wxwidgets_COMPILE_FLAGS} ${FREETYPE_INCLUDE_CFLAGS}"
	)
      set(
	wxwidgets_LINK_FLAGS
	${wxwidgets_LINK_FLAGS}
	${FREETYPE_LIBRARIES}
	)
    endif(WITH_FREETYPE)

  set(DRIVERS_LINK_FLAGS
    ${DRIVERS_LINK_FLAGS}
    ${wxwidgets_LINK_FLAGS}
    )

endif(PLD_wxwidgets)
