# src/CMakeLists.txt for PLplot
# Copyright (C) 2006-2019 Alan W. Irwin
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

set(plplot_LIB_SRCS
  pdfutils.c
  plmem.c
  plaffine.c
  plarc.c
  plargs.c
  plbox.c
  plcont.c
  plcore.c
  plctrl.c
  plcvt.c
  pldtik.c
  plf2ops.c
  plfill.c
  plfreetype.c
  plgradient.c
  plhist.c
  plimage.c
  plline.c
  plmetafile.c
  plot3d.c
  plpage.c
  plsdef.c
  plshade.c
  plstdio.c
  plstripc.c
  plsym.c
  pltick.c
  plvpor.c
  plwind.c
  plbuf.c
  plgridd.c
  plvect.c
#  mt19937ar.c
#  pltime.c
  pllegend.c
# added
  plstream.cc
  )

option(HERSHEY_FALLBACK "Hershey fallback for Unicode fonts" ON)

if(NOT HERSHEY_FALLBACK)
  get_source_file_property(PLSYM_COMPILE_PROPS plsym.c COMPILE_FLAGS)
  if(NOT PLSYM_COMPILE_PROPS)
    set(PLSYM_COMPILE_PROPS)
  endif(NOT PLSYM_COMPILE_PROPS)
  set_source_files_properties(
    plsym.c
    PROPERTIES COMPILE_FLAGS "${PLSYM_COMPILE_PROPS} -DPL_TEST_FOR_MISSING_GLYPHS"
    )
endif(NOT HERSHEY_FALLBACK)

set(pc_libplplot_COMPILE_FLAGS)

get_source_file_property(PLCORE_COMPILE_PROPS plcore.c COMPILE_FLAGS)
# Deal with NOTFOUND case.
if(NOT PLCORE_COMPILE_PROPS)
  set(PLCORE_COMPILE_PROPS)
endif(NOT PLCORE_COMPILE_PROPS)
if(WIN32_OR_CYGWIN)
  set(PLCORE_COMPILE_PROPS "${PLCORE_COMPILE_PROPS} -DIGNORECASE")
endif(WIN32_OR_CYGWIN)

set_source_files_properties(
  plcore.c
  PROPERTIES COMPILE_FLAGS "${PLCORE_COMPILE_PROPS}"
  )


  # Nondynamic device drivers must be included in the core PLplot library.
  set(DRIVERS_RPATH)

    message("DRIVERS_LIST = ${DRIVERS_LIST}")
  foreach(SOURCE_ROOT_NAME ${DRIVERS_LIST})
    message("${SOURCE_ROOT_NAME}_SOURCE = ${${SOURCE_ROOT_NAME}_SOURCE}")
    
    list(APPEND plplot_LIB_SRCS ${${SOURCE_ROOT_NAME}_SOURCE})
    message("plplot_LIB_SRCS = ${plplot_LIB_SRCS}")
    if(${SOURCE_ROOT_NAME}_COMPILE_FLAGS)
      set_source_files_properties(
	${${SOURCE_ROOT_NAME}_SOURCE}
	PROPERTIES COMPILE_FLAGS ${${SOURCE_ROOT_NAME}_COMPILE_FLAGS}
	)
    
    list(APPEND pc_libplplot_COMPILE_FLAGS ${${SOURCE_ROOT_NAME}_COMPILE_FLAGS})

    endif(${SOURCE_ROOT_NAME}_COMPILE_FLAGS)
    if(${SOURCE_ROOT_NAME}_RPATH)
      # Accumulate driver rpath information into DRIVERS_RPATH.
      list(APPEND DRIVERS_RPATH ${${SOURCE_ROOT_NAME}_RPATH})
    endif(${SOURCE_ROOT_NAME}_RPATH)
  endforeach(SOURCE_ROOT_NAME ${DRIVERS_LIST})

include_directories(
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/drivers
  ${CMAKE_BINARY_DIR}
  )

# The plplot library depends on the "lib subdirectory" libraries such
# as csironn, csirocsa, qsastime, and (possibly) nistcd.
set(LIB_INSTALL_RPATH ${LIB_DIR})
set(libplplot_LINK_LIBRARIES)

  list(APPEND libplplot_LINK_LIBRARIES ${DRIVERS_LINK_FLAGS})
  # For this case the plplot library depends on external libraries required
  # by the device driver code that is in the plplot library.
  list(APPEND LIB_INSTALL_RPATH ${DRIVERS_RPATH})

message("DEBUG: LIB_INSTALL_RPATH = ${LIB_INSTALL_RPATH}")

if(MATH_LIB)
  list(APPEND libplplot_LINK_LIBRARIES ${MATH_LIB})
endif(MATH_LIB)

if(WITH_FREETYPE)
  get_source_file_property(PLFREETYPE_COMPILE_PROPS plfreetype.c COMPILE_FLAGS)
  # Deal with NOTFOUND case.
  if(NOT PLFREETYPE_COMPILE_PROPS)
    set(PLFREETYPE_COMPILE_PROPS)
  endif(NOT PLFREETYPE_COMPILE_PROPS)
  set_source_files_properties(plfreetype.c PROPERTIES COMPILE_FLAGS "${PLFREETYPE_COMPILE_PROPS} ${FREETYPE_INCLUDE_CFLAGS}")
  list(APPEND libplplot_LINK_LIBRARIES ${FREETYPE_LIBRARIES})
endif(WITH_FREETYPE)

# Copy current information in libplplot_LINK_LIBRARIES
# into pc_libplplot_LINK_FLAGS, which contains the equivalent list information
# in a combination of library list form and pkg-config form that will
# eventually be transformted into pure pkg-config form.  Both variables
# will have data added to them afterward due to the internal PLplot libraries
# which have a different pkg-config treatment than the external libraries
# dealt with here.
set(pc_libplplot_LINK_FLAGS ${libplplot_LINK_LIBRARIES})


# The above completes dealing with the external libraries.  Now
# deal with the internal libraries that are built by the PLplot
# build system.

if(PLD_wxwidgets AND RT_LIB)
  list(APPEND libplplot_LINK_LIBRARIES ${RT_LIB})
  list(APPEND pc_libplplot_LINK_FLAGS ${RT_LIB})
endif(PLD_wxwidgets AND RT_LIB)

message(STATUS "libplplot_LINK_LIBRARIES = ${libplplot_LINK_LIBRARIES}")

#configure_library_build(plplot "" "${plplot_LIB_SRCS}" "${libplplot_LINK_LIBRARIES}" "${LIB_INSTALL_RPATH}")
#add_dependencies(${WRITEABLE_TARGET}plplot plhershey-unicode.h_built)

message("pc_libplplot_LINK_FLAGS = ${pc_libplplot_LINK_FLAGS}")

string(REGEX REPLACE ";" " "
  pc_libplplot_COMPILE_FLAGS
  "${pc_libplplot_COMPILE_FLAGS}"
  )
message("pc_libplplot_COMPILE_FLAGS = ${pc_libplplot_COMPILE_FLAGS}")

#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PLPLOTSOURCES)
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src/plplot ${CMAKE_SOURCE_DIR}/src/plplot/include)

if(PYTHON_MODULE)
	add_library(plplot SHARED ${plplot_LIB_SRCS})
	install(TARGETS plplot DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
else(PYTHON_MODULE)
	add_library(plplot STATIC ${plplot_LIB_SRCS})
endif(PYTHON_MODULE)
