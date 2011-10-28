#
# copyright : (c) 2010 Maxime Lenoir and Alain Coulais
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#

# tested with 2.4 and doesn't seem to run.
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(GDL)

set(VERSION "0.9.1 CVS")
enable_testing()

include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckLibraryExists)
include(CheckFunctionExists)
include(CheckCSourceRuns)
include(FindPkgConfig)
include(FindPackageHandleStandardArgs)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMakeModules)

#### Default cached values
set(PYTHON_MODULE OFF CACHE BOOL "Build GDL as a Python module ?")

set(X11 ON CACHE BOOL "Enable X11 ?") 
set(X11DIR "" CACHE PATH "Specify X11 directory tree")

set(READLINE ON CACHE BOOL "Enable GNU Readline ?")
set(READLINEDIR "" CACHE PATH "Specify the GNU readline directory tree")

set(OLDPLPLOT OFF CACHE BOOL "Enable old plplot ?")
set(PLPLOTDIR "" CACHE PATH "Specify the plplot directory tree")

set(GSLDIR "" CACHE PATH "Specify the GSL directory tree")

set(ZLIBDIR "" CACHE PATH "specify the Zlib directory tree")

set(WXWIDGETS OFF CACHE BOOL "Enable WxWidgets ?")
set(WXWIDGETSDIR "" CACHE PATH "Specify WxWidgets directory tree")

set(UDUNITS OFF CACHE BOOL "Enable UDUNITS-2 ?")
set(UDUNITSDIR "" CACHE PATH "Specify the UDUNITS-2 directory tree")

set(PSLIB ON CACHE BOOL "Enable pslib ?")
set(PSLIBDIR "" CACHE PATH "Specify the pslib directory tree")

set(GRIB OFF CACHE BOOL "Enable Grib ?")
set(GRIBDIR "" CACHE PATH "Specifiy the GRIB directory tree")

set(MAGICK ON CACHE BOOL "Enable Magick ?")
set(MAGICKDIR "" CACHE PATH "Specifiy the ImageMagick directory tree")

set(NETCDF ON CACHE BOOL "Enable NetCDF ?")
set(NETCDFDIR "" CACHE PATH "Specifiy the netCDF directory tree")

set(HDF ON CACHE BOOL "Enable Hdf ?")
set(HDFDIR "" CACHE PATH "Specifiy the HDF directory tree")

set(HDF5 ON CACHE BOOL "Enable Hdf5 ?")
set(HDF5DIR "" CACHE PATH "Specifiy the HDF5 directory tree")

set(FFTW ON CACHE BOOL "Enable FFTW ?")
set(FFTWDIR "" CACHE PATH "Specifiy the FFTW directory tree")

set(LIBPROJ4 OFF CACHE BOOL "Enable LIBPROJ4 ?")
set(LIBPROJ4DIR "" CACHE PATH "Specifiy the LIBPROJ4 directory tree")

set(MPICH OFF CACHE BOOL "Enable MPICH ?")
set(MPICHDIR "" CACHE PATH "Specify the MPICH (experimental) directory tree")

set(PYTHON ON CACHE BOOL "Enable Python ?")
set(PYTHONDIR "" CACHE PATH "Specifiy the use Python directory tree")
set(PYTHONVERSION "" CACHE STRING "Specify the Python version to use")

set(GSHHS OFF CACHE BOOL "Enable GSHHS ?")
set(GSHHSDIR "" CACHE PATH "Specifiy the GSHHS directory tree")

set(NCURSESDIR "" CACHE PATH "Specify the ncurses (or curses) directory tree")

set(OPENMP ON CACHE BOOL "Enable OpenMP ?")

# Third party libraries
set(JASPERDIR "" CACHE PATH "Specify the JasPer directory tree")
set(JPEGDIR "" CACHE PATH "Specify the JPEG directory tree")
set(SZIPDIR "" CACHE PATH "Specify the SZip directory tree")

set(GDL_DATA_DIR "/share/gnudatalanguage" CACHE PATH "Choose data install directory relative to CMAKE_INSTALL_PREFIX")


# check for 64-bit OS
if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
	set(HAVE_64BIT_OS 1)
endif(${CMAKE_SIZEOF_VOID_P} EQUAL 8)


#### check headers and libraries

# dl
check_library_exists(dl dlopen "" HAVE_DL)
if(HAVE_DL)
	set(LIBRARIES ${LIBRARIES} dl)
endif(HAVE_DL)

# malloc stats
check_function_exists(malloc_zone_statistics HAVE_MALLOC_ZONE_STATISTICS)
check_function_exists(sbrk HAVE_SBRK)
check_function_exists(mallinfo HAVE_MALLINFO)

# mallocs
check_include_file(malloc.h HAVE_MALLOC_H)
check_include_file(malloc/malloc.h HAVE_MALLOC_MALLOC_H)

# locale
check_include_file(locale.h HAVE_LOCALE_H)

# std includes..
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(string.h HAVE_STRING_H)
check_include_file(strings.h HAVE_STRINGS_H)
check_include_file(sys/stat.h HAVE_SYS_STAT_H)
check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(ext/stdio_filebuf.h HAVE_EXT_STDIO_FILEBUF_H)

# dlfcn.h
check_include_file(dlfcn.h HAVE_DLFCN_H)

# inttypes.h
check_include_file(inttypes.h HAVE_INTTYPES_H)

# nexttoward
check_library_exists(m nexttoward "" HAVE_NEXTTOWARD)

# mpi
check_include_file(mpi.h HAVE_MPI_H)

# SA: whithout it compilation of antlr fails if there's a conflicting 
#     version of antlr in system-wide directories
include_directories(src)

# Ncurses MANDATORY
# -DNCURSESDIR=DIR
set(CMAKE_PREFIX_PATH ${NCURSESDIR})	
set(CURSES_NEED_NCURSES TRUE)
find_package(Curses QUIET COMPONENTS initscr)
mark_as_advanced(CURSES_CURSES_H_PATH CURSES_FORM_LIBRARY CURSES_HAVE_CURSES_H)
if(CURSES_FOUND)
	set(HAVE_LIBNCURSES 1)
	set(LIBRARIES ${LIBRARIES} ${CURSES_LIBRARIES})
	include_directories(${CURSES_INCLUDE_DIR})
else(CURSES_FOUND)
	# search for curses
	set(CURSES_NEED_NCURSES FALSE)
	find_package(Curses COMPONENTS initscr)
	set(HAVE_LIBCURSES ${CURSES_FOUND})
	if(CURSES_FOUND)
		set(LIBRARIES ${LIBRARIES} ${CURSES_LIBRARIES})
		include_directories(${CURSES_INCLUDE_DIR})
	else(CURSES_FOUND)
		message(FATAL_ERROR "(N)Curses was not found.\n"
		"Use -DNCURSESDIR=DIR to specify the (n)curses directory tree.")
	endif(CURSES_FOUND)
endif(CURSES_FOUND)

# Readline
# -DREADLINE=ON|OFF
# -DREADLINEDIR=DIR
if(READLINE)
	# readline needs (n)curses
	set(CMAKE_PREFIX_PATH ${READLINEDIR})
	find_package(Readline QUIET)
	set(HAVE_LIBREADLINE ${READLINE_FOUND})
	if(READLINE_FOUND)
		set(CMAKE_REQUIRED_LIBRARIES ${CURSES_LIBRARIES})
		check_library_exists("${READLINE_LIBRARIES}" rl_get_screen_size "" RL_GET_SCREEN_SIZE)
		if(NOT RL_GET_SCREEN_SIZE)
			message(STATUS "WARNING: Older GNU readline without rl_get_screen_size was found.\n"
			"For resized terminals the size might not be updated correctly.\n"
			"If this is a problem please install a recent version of readline.")
		endif(NOT RL_GET_SCREEN_SIZE)
		set(LIBRARIES ${LIBRARIES} ${READLINE_LIBRARIES})
		include_directories(${READLINE_INCLUDE_DIR})
		set(CMAKE_REQUIRED_LIBRARIES)
	else(READLINE_FOUND)
		message(FATAL_ERROR "GNU readline was not found.\n"
		"Use -DREADLINEDIR=no to explicitely disable it.")
	endif(READLINE_FOUND)
endif(READLINE)

# zlib MANDATORY
# -DZLIBDIR=DIR
set(CMAKE_PREFIX_PATH ${ZLIBDIR})
find_package(ZLIB QUIET)
set(HAVE_LIBZ ${ZLIB_FOUND})
if(ZLIB_FOUND)
	set(LIBRARIES ${LIBRARIES} ${ZLIB_LIBRARIES})
	include_directories(${ZLIB_INCLUDE_DIR})
else(ZLIB_FOUND)
	message(FATAL_ERROR "ZLib library is required but was not found.\n"
	"Use -DZLIBDIR=DIR to specify the zlib directory tree.")
endif(ZLIB_FOUND)

# openmp
# -DOPENMP=ON|OFF
if(OPENMP)
	find_package(OpenMP QUIET)
	if(OPENMP_FOUND)
		add_definitions(${OpenMP_CXX_FLAGS})
		set(LIBRARIES ${LIBRARIES} ${OpenMP_CXX_FLAGS})
	endif(OPENMP_FOUND)
endif(OPENMP)

# GSL MANDATORY
# -DGSLDIR=DIR
set(CMAKE_PREFIX_PATH ${GSLDIR})
find_package(GSL QUIET)
set(HAVE_LIBGSL ${GSL_FOUND})
set(HAVE_LIBGSLCBLAS ${GSL_FOUND})
if(GSL_FOUND)
	set(LIBRARIES ${LIBRARIES} ${GSL_LIBRARIES})
	include_directories(${GSL_INCLUDE_DIR})
else(GSL_FOUND)
	message(FATAL_ERROR "Gnu Scientific library (1.7 or higher) and libgslcblas are mandatory.\n"
	"Use -DGSLDIR=DIR to specify the gsl directory tree.")
endif(GSL_FOUND)

# PLplot MANDATORY
# -DOLDPLPLOT=ON|OFF
# -DPLPLOTDIR=DIR
set(HAVE_OLDPLPLOT ${OLDPLPLOT})
set(CMAKE_PREFIX_PATH ${PLPLOTDIR})
find_package(Plplot QUIET)
set(HAVE_LIBPLPLOTCXXD ${PLPLOT_FOUND})
if(PLPLOT_FOUND)
	set(LIBRARIES ${LIBRARIES} ${PLPLOT_LIBRARIES})
	include_directories(${PLPLOT_INCLUDE_DIR})
else(PLPLOT_FOUND)
	message(FATAL_ERROR "plplot library is required but was not found.\n"
	"Use -DPLPLOTDIR=DIR to specify the plplot directory tree.")
endif(PLPLOT_FOUND)
if(MAGICK) # support Magick
	check_library_exists("${PLPLOT_LIBRARIES}" plGetDrvDir "" HAVE_PLGETDRVDIR)
	if(HAVE_PLGETDRVDIR)
		message(STATUS "WARNING: Dynamic drivers have to be disabled in plplot.\n"
		"To make plplot work with ImageMagick:\n"
		"use -DENABLE_DYNDRIVERS=OFF cmake option when compiling plplot or\n"
		"use -DMAGICK=OFF here to not use ImageMagick in GDL.")
	endif(HAVE_PLGETDRVDIR)
endif(MAGICK)

# wxWdigets
# -DWXWIDGET=ON|OFF
# -DWXWIDGETDIR=DIR
if(WXWIDGETS)
	set(CMAKE_PREFIX_PATH ${WXWIDGETSDIR})
	find_package(wxWidgets QUIET)
	set(HAVE_LIBWXWIDGETS ${wxWidgets_FOUND})
	if(wxWidgets_FOUND)
		set(LIBRARIES ${LIBRARIES} ${wxWidgets_LIBRARIES})
		set(LINK_DIRECTORIES ${LINK_DIRECTORIES} ${wxWidgets_LIBRARY_DIRS})
		foreach(WXDEF ${wxWidgets_DEFINITIONS})
			add_definitions(-D${WXDEF})
		endforeach(WXDEF ${wxWidgets_DEFINITIONS})
		include_directories(${wxWidgets_INCLUDE_DIRS})
	else(wxWidgets_FOUND)
		message(FATAL_ERROR "wxWidgets are required but were not found.\n"
		"Use -DWXWIDGETSDIR=DIR to specify the wxWidgets directory tree.\n"
		"Use -DWXWIDGETS=OFF to not use it.")
	endif(wxWidgets_FOUND)
endif(WXWIDGETS)

# Magick
# -DMAGICK=ON|OFF
# -DMAGICKDIR=DIR
if(MAGICK)
	set(CMAKE_PREFIX_PATH ${MAGICKDIR})
	find_package(ImageMagick QUIET COMPONENTS Magick++ MagickWand MagickCore)
	mark_as_advanced(ImageMagick_EXECUTABLE_DIR ImageMagick_Magick++_INCLUDE_DIR ImageMagick_Magick++_LIBRARY
	ImageMagick_MagickCore_INCLUDE_DIR ImageMagick_MagickCore_LIBRARY ImageMagick_MagickWand_INCLUDE_DIR ImageMagick_MagickWand_LIBRARY)
	set(USE_MAGICK ${ImageMagick_FOUND})
	if(ImageMagick_FOUND)
		find_program(MAGICKXXCONFIG Magick++-config)
		if(MAGICKXXCONFIG)
			execute_process(COMMAND ${MAGICKXXCONFIG} "--libs" OUTPUT_VARIABLE MAGICKXXCONFIGLIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
			set(LIBRARIES ${LIBRARIES} ${MAGICKXXCONFIGLIBS})		
		else(MAGICKXXCONFIG)
			message(FATAL_ERROR "ImageMagick is required but was not found (Magick++-config).\n"
			"Use -DMAGICKDIR=DIR to specify the ImageMagick directory.\n"
			"Use -DMAGICK=OFF to not use it.")
		endif(MAGICKXXCONFIG)
		set(LIBRARIES ${LIBRARIES} ${ImageMagick_LIBRARIES})
		include_directories(${ImageMagick_INCLUDE_DIRS})
		set(MAGICK_LIBRARIES ${ImageMagick_LIBRARIES})
	else(ImageMagick_FOUND)
		message(FATAL_ERROR "ImageMagick is required but was not found.\n"
		"Use -DMAGICKDIR=DIR to specify the ImageMagick directory.\n"
		"Use -DMAGICK=OFF to not use it.")
	endif(ImageMagick_FOUND)
endif(MAGICK)

# netCDF
# -DNETCDF=ON|OFF
# -DNETCDFDIR=DIR
if(NETCDF)
	set(CMAKE_PREFIX_PATH ${NETCDFDIR})
	find_package(NetCDF QUIET)
	if(NETCDF_FOUND)
		set(CMAKE_REQUIRED_INCLUDES ${NETCDF_INCLUDE_DIRS})
		check_include_file_cxx(netcdfcpp.h HAVE_NETCDFCPP_H)
		if(HAVE_NETCDFCPP_H)
			set(LIBRARIES ${LIBRARIES} ${NETCDF_LIBRARIES})
			set(LINK_DIRECTORIES ${LINK_DIRECTORIES} ${NETCDF_LIBRARY_DIR})
			include_directories(${NETCDF_INCLUDE_DIRS})
			set(USE_NETCDF 1)
		else(HAVE_NETCDFCPP_H)
			message(FATAL_ERROR "NetCDF installation seems not to be usable.\n"
			"This suggests a conflicting netCDF-HDF4 installation e.g.\n"
			"- Uninstalling HDF4 after installation of NetCDF.\n"
			"- Installing NetCDF before HDF4.")
		endif(HAVE_NETCDFCPP_H)
		set(CMAKE_REQUIRED_INCLUDES)
	else(NETCDF_FOUND)
		message(FATAL_ERROR "NetCDF version 3.5.1 or later is required but was not found.\n"
		"Use -DNETCDFDIR=DIR to specify the netcdf directory tree.\n"
		"Use -DNETCDF=OFF to not use it.")	
	endif(NETCDF_FOUND)
endif(NETCDF)

# hdf4
# -DHDF=ON|OFF
# -DHDFDIR=DIR
if(HDF)
	set(CMAKE_PREFIX_PATH ${HDFDIR} ${JPEGDIR} ${SZIPDIR})
	find_package(HDF QUIET)
	set(USE_HDF ${HDF_FOUND})
	if(HDF_FOUND)
		if(NETCDF)
			set(CMAKE_REQUIRED_LIBRARIES ${HDF_EXTRA_LIBRARIES})
                        check_library_exists("${HDF_LIBRARIES}" sd_nccreate "" SD_NCCREATE)
			if(NOT SD_NCCREATE)
				message(FATAL_ERROR "HDF4 needs to be configured with the --disable-netcdf option "
				"in order to be used with the original netCDF library.")
			endif(NOT SD_NCCREATE)
			set(CMAKE_REQUIRED_LIBRARIES)
		endif(NETCDF)
		set(HDF_LIBRARIES ${HDF_LIBRARIES} ${HDF_EXTRA_LIBRARIES})
		set(LIBRARIES ${LIBRARIES} ${HDF_LIBRARIES})
		include_directories(${HDF_INCLUDE_DIR})
	else(HDF_FOUND)
		message(FATAL_ERROR "HDF4 libraries were not found.\n"
		"Use -DHDFDIR=DIR to specify the HDF directory tree.\n"
		"Use -DHDF=OFF to not use it. "
		"You can use -DJPEGDIR=DIR to specify the JPEG directory tree. "
		"You can also use -DSZIPDIR=DIR to specify SZip directory tree if "
		"HDF was compiled with SZip support.")
	endif(HDF_FOUND)
endif(HDF)

# hdf5
# -DHDF5=ON|OFF
# -DHDF5DIR=DIR
if(HDF5)
	set(CMAKE_PREFIX_PATH ${HDF5DIR} ${SZIPDIR})
	find_package(HDF5 QUIET)
	set(USE_HDF5 ${HDF5_FOUND})
	if(HDF5_FOUND)
		set(LIBRARIES ${LIBRARIES} ${HDF5_LIBRARIES})
		include_directories(${HDF5_INCLUDE_DIRS})
		find_package(MPI QUIET)
		if(MPI_FOUND)
			include_directories(${MPI_INCLUDE_PATH})
			set(LIBRARIES ${LIBRARIES} ${MPI_LIBRARIES})	
		endif(MPI_FOUND)
	else(HDF5_FOUND)
		message(FATAL_ERROR "HDF version 5 is required but was not found.\n"
		"Use -DHDF5DIR=DIR to specify the HDF5 directory tree.\n"
		"Use -DHDF5=OFF to not use it.")
	endif(HDF5_FOUND)
endif(HDF5)

# fftw
# -DFFTW=ON|OFF
# -DFFTWDIR=DIR
if(FFTW)
	set(CMAKE_PREFIX_PATH ${FFTWDIR})
	find_package(FFTW QUIET)
	set(USE_FFTW ${FFTW_FOUND})
	if(FFTW_FOUND)
		set(LIBRARIES ${LIBRARIES} ${FFTW_LIBRARIES})
		include_directories(${FFTW_INCLUDE_DIR})
	else(FFTW_FOUND)
		message(FATAL_ERROR "FFTW3 is required but was not found.\n"
		"Use -DFFTWDIR=DIR to specify the FFTW directory tree.\n"
		"Use -DFFTW=OFF to not use it.")
	endif(FFTW_FOUND)
endif(FFTW)

# libproj4
# -DLIBPROJ4=ON|OFF
# -DLIBPROJ4DIR=DIR
if(LIBPROJ4)
	set(CMAKE_PREFIX_PATH ${LIBPROJ4DIR})
	find_package(Libproj4 QUIET)
	if(LIBPROJ4_FOUND)	
		set(CMAKE_REQUIRED_LIBRARIES ${LIBPROJ4_LIBRARIES} ${GSL_LIBRARIES} m)
		check_library_exists("${LIBPROJ4_LIBRARIES}" proj_init "" USE_LIBPROJ4_NEW)
		if(USE_LIBPROJ4_NEW)
			set(USE_LIBPROJ4 1)
		else(USE_LIBPROJ4_NEW)
			check_library_exists("${LIBPROJ4_LIBRARIES}" pj_init "" USE_LIBPROJ4)
			if(NOT USE_LIBPROJ4)
				message(FATAL_ERROR "Libproj4 is required but was not found.\n"
				"Use -DLIBPROJ4DIR=DIR to specify the Libproj4 directory tree.\n"
				"Use -DLIBPRJ4=OFF to not use it.")
			endif(NOT USE_LIBPROJ4)
		endif(USE_LIBPROJ4_NEW)
		include_directories(${LIBPROJ4_INCLUDE_DIR})
		set(LIBRARIES ${LIBRARIES} ${LIBPROJ4_LIBRARIES})
		set(CMAKE_REQUIRED_LIBRARIES)
	else(LIBPROJ4_FOUND)
		message(FATAL_ERROR "Libproj4 is required but was not found.\n"
		"Use -DLIBPROJ4DIR=DIR to specify the Libproj4 directory tree.\n"
		"Use -DLIBPRJ4=OFF to not use it.")
	endif(LIBPROJ4_FOUND)	
endif(LIBPROJ4)

# mpich (experimental)
# -DMPICH=ON|OFF
# -DMPICHDIR=DIR
if(MPICH)
	set(CMAKE_PREFIX_PATH ${MPICHDIR})
	find_package(MPI QUIET)
	set(USE_MPI ${MPI_FOUND})
	if(MPI_FOUND)
		include_directories(${MPI_INCLUDE_PATH})	
		set(LIBRARIES ${LIBRARIES} ${MPI_LIBRARIES})
	else(MPI_FOUND)
		message(FATAL_ERROR "MPICH is required but was not found.\n"
		"Use -DMPICHDIR=DIR to specify the MPICH directory tree.\n"
		"Use -DMPICH=OFF to not use it.")
	endif(MPI_FOUND)	
endif(MPICH)

# python
# -DPYTHON=ON|OFF
# -DPYTHON_MODULE
# -DPYTHONDIR=DIR
# -DPYTHONVERSION=VERSION
if(PYTHON OR PYTHON_MODULE)
	if(PYTHON)
		set(PYTHONMSG "Use -DPYTHON=OFF to disable Python support.\n")
	endif(PYTHON)
	if(PYTHON_MODULE)
		set(PYTHON_MODULEMSG "Use -DPYTHON_MODULE=OFF to disable Python module.\n")
	endif(PYTHON_MODULE)
	set(CMAKE_PREFIX_PATH ${PYTHONDIR})
	if(PYTHONVERSION)
		find_package(PythonLibs ${PYTHONVERSION} EXACT QUIET)
	else(PYTHONVERSION)
		find_package(PythonLibs QUIET)
	endif(PYTHONVERSION)
	set(USE_PYTHON ${PYTHONLIBS_FOUND})
	if(PYTHONLIBS_FOUND)
		find_package(Numpy QUIET)
		if(NOT PYTHON_NUMPY_FOUND)
			message(FATAL_ERROR "Python numpy package was not found.\n"
			"${PYTHONMSG} ${PYTHON_MODULEMSG}")
		endif(NOT PYTHON_NUMPY_FOUND)
		set(LIBRARIES ${LIBRARIES} ${PYTHON_LIBRARIES})
		include_directories(${PYTHON_INCLUDE_DIRS})	
	else(PYTHONLIBS_FOUND)
		message(FATAL_ERROR "Python is required but was not found.\n"
		"Use -DPYTHONDIR=DIR to specify the Python directory tree.\n"
		"${PYTHONMSG} ${PYTHON_MODULEMSG}"
		"Use -DPYTHONVERSION=VERSION to help searching for the right version.")
	endif(PYTHONLIBS_FOUND)
endif(PYTHON OR PYTHON_MODULE)

# udunits
# -DUDUNITS=ON|OFF
# -DUDUNITSDIR=DIR
if(UDUNITS)
	set(CMAKE_PREFIX_PATH ${UDUNITSDIR})
	find_package(Udunits QUIET)
	set(USE_UDUNITS ${UDUNITS_FOUND})
	if(UDUNITS_FOUND)
		set(LIBRARIES ${LIBRARIES} ${UDUNITS_LIBRARIES})
		include_directories(${UDUNITS_INCLUDE_DIR})	
	else(UDUNITS_FOUND)
		message(FATAL_ERROR "UDUNITS-2 is required but was not found.\n"
		"Use -DUDUNITSDIR=DIR to specify the Udunits directory tree.\n"
		"Use -DUDUNITS=OFF to not use it.")
	endif(UDUNITS_FOUND)
endif(UDUNITS)

# pslib
# -DPSLIB=ON|OFF
# -DPSLIBDIR=DIR
if(PSLIB)
	set(CMAKE_PREFIX_PATH ${PSLIBDIR})
	find_package(libps QUIET)
	set(USE_PSLIB ${LIBPS_FOUND})
	if(LIBPS_FOUND)
		set(LIBRARIES ${LIBRARIES} ${LIBPS_LIBRARIES})
		include_directories(${LIBPS_INCLUDE_DIR})	
	else(LIBPS_FOUND)
		message(FATAL_ERROR "pslib is required but was not found.\n"
		"Use -DPSLIBDIR=DIR to specify the pslib directory tree.\n"
		"Use -DPSLIB=OFF to not use it.")
	endif(LIBPS_FOUND)
endif(PSLIB)

# grib
# -DGRIB=ON|OFF
# -DGRIBDIR=DIR
if(GRIB)
	set(CMAKE_PREFIX_PATH ${GRIBDIR} ${JASPERDIR} ${JPEGDIR})
	find_package(Grib QUIET)
	set(USE_GRIB ${GRIB_FOUND})
	if(GRIB_FOUND)
		set(LIBRARIES ${LIBRARIES} ${GRIB_LIBRARIES})
		include_directories(${GRIB_INCLUDE_DIR})
	else(GRIB_FOUND)
		message(FATAL_ERROR "GRIB is required but was not found.\n"
		"Use -DGRIBDIR=DIR to specify the GRIB directory tree.\n"
		"Use -DGRIB=OFF to not use it.\n"
		"Use -DJASPERDIR=DIR and|or -DJPEGDIR=DIR to specify "
		"the directory trees of JasPer and openJPEG libraries.")
	endif(GRIB_FOUND)
endif(GRIB)

# gshhs
# -DGSHHS=ON|OFF
# -DGSHHSDIR=DIR
if(GSHHS)
	set(CMAKE_PREFIX_PATH ${GSHHSDIR})
	find_path(GSHHS_INCLUDE_DIR gshhs.h)
	mark_as_advanced(GSHHS_INCLUDE_DIR)
	set(USE_GSHHS ${GSHHS_INCLUDE_DIR})
	if(USE_GSHHS)
		include_directories(${GSHHS_INCLUDE_DIR})
	else(USE_GSHHS)
		message(FATAL_ERROR "GSHHS is required but gshhs.h was not found.\n"
		"Use -DGSSHSDIR=DIR to specify the GSHHS directory tree.\n"
		"Use -DGSHHS=OFF to not use it.")
	endif(USE_GSHHS)
endif(GSHHS)

# X11
if(X11)
	set(CMAKE_PREFIX_PATH ${X11DIR})
	find_package(X11 QUIET)
	set(HAVE_X ${X11_FOUND})
	if(X11_FOUND)
		set(LIBRARIES ${LIBRARIES} ${X11_LIBRARIES})
		include_directories(${X11_INCLUDE_DIR})
	else(X11_FOUND)
		message(FATAL_ERROR "X11 is required but was not found.\n"
		"Use -DX11DIR=DIR to specify the X11 directory tree.\n"
		"Use -DX11=OFF to not use it.")
	endif(X11_FOUND)
endif(X11)

add_subdirectory(src)
if(NOT PYTHON_MODULE)
	add_subdirectory(testsuite)
endif(NOT PYTHON_MODULE)

install(FILES ${CMAKE_SOURCE_DIR}/AUTHORS ${CMAKE_SOURCE_DIR}/README DESTINATION ${CMAKE_INSTALL_PREFIX}/${GDL_DATA_DIR})
install(FILES ${CMAKE_SOURCE_DIR}/doc/gdl.1 DESTINATION ${CMAKE_INSTALL_PREFIX}/share/man/man1)

# substitute variables in configure.h.cmake and move it to configure.h
configure_file(${CMAKE_SOURCE_DIR}/config.h.cmake ${CMAKE_BINARY_DIR}/config.h)

# create a link src/gdl -> ${BUILD}/.../gdl
if(NOT PYTHON_MODULE)
	get_target_property(GDLLOCATION gdl LOCATION)
	add_custom_target(symlink_gdl ALL ${CMAKE_COMMAND} -E create_symlink ${GDLLOCATION} ${CMAKE_SOURCE_DIR}/src/gdl DEPENDS gdl)
endif(NOT PYTHON_MODULE)

# display macro
macro(MODULE MOD NAME)
	if(${MOD})
		message("${NAME}         ON ${${MOD}_LIBRARIES}")
	else(${MOD})
		message("${NAME}         OFF")
	endif(${MOD})
endmacro(MODULE)

# python
if(PYTHON_MODULE)
	set(BUILDTYPE "Python Module")
else(PYTHON_MODULE)
	set(BUILDTYPE "Standalone")
endif(PYTHON_MODULE)


#
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

# AC, 12-oct-2011, solved by Marc 
# set_target_properties(gdl PROPERTIES LINK_FLAGS "-Wl,-z,muldefs")
# set_target_properties(gdl PROPERTIES LINK_FLAGS "-z muldefs")

#
if(CMAKE_BUILD_TYPE STREQUAL None OR NOT CMAKE_BUILD_TYPE)
	set(FLAGS ${CMAKE_CXX_FLAGS})
elseif(CMAKE_BUILD_TYPE STREQUAL Debug)
	set(FLAGS ${CMAKE_CXX_FLAGS_DEBUG})
elseif(CMAKE_BUILD_TYPE STREQUAL Release)
	set(FLAGS ${CMAKE_CXX_FLAGS_RELEASE})
elseif(CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
	set(FLAGS ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
elseif(CMAKE_BUILD_TYPE STREQUAL MinSizeRel)
	set(FLAGS ${CMAKE_CXX_FLAGS_MINSIZEREL})
endif(CMAKE_BUILD_TYPE STREQUAL None OR NOT CMAKE_BUILD_TYPE)
message(STATUS
"Summary

GDL - GNU DATA LANGUAGE [${BUILDTYPE}]
System              ${CMAKE_HOST_SYSTEM}
Files generated     ${CMAKE_GENERATOR}
GDL output          ${GDLLOCATION}
Installation prefix ${CMAKE_INSTALL_PREFIX}
C++ compiler        ${CMAKE_CXX_COMPILER} ${FLAGS}")
message("")
message(STATUS "Options")
if(OPENMP AND OPENMP_FOUND)
	message("OpenMP support      ON ${OpenMP_CXX_FLAGS}")
else(OPENMP AND OPENMP_FOUND)
	message("OpenMP support      OFF")
endif(OPENMP AND OPENMP_FOUND)
set(WXWIDGETS_LIBRARIES ${wxWidgets_LIBRARIES})
module(WXWIDGETS "WxWidgets  ")
module(MAGICK    "ImageMagick")
module(NETCDF    "NetCDF     ")
module(HDF       "HDF4       ")
module(HDF5      "HDF5       ")
module(FFTW      "FFTW       ")
module(LIBPROJ4  "Libproj4   ")
set(MPICH_LIBRARIES ${MPI_LIBRARIES})
module(MPICH     "MPICH      ")
module(PYTHON    "Python     ")
module(UDUNITS   "UDUNITS-2  ")
module(GRIB      "GRIB       ")
set(GSHHS_LIBRARIES ${GSHHS_INCLUDE_DIR})
module(GSHHS     "GSHHS      ")
module(PSLIB	 "pslib      ")
module(X11	 "Xlib       ")
message("")
message(STATUS "Mandatory modules")
set(PLPLOT ON)
set(READLINE ON)
set(GSL ON)
set(ZLIB ON)
set(CURSES ON)
module(PLPLOT    "Plplot     ")
module(OLDPLPLOT "Old Plplot ")
module(READLINE  "Readline   ")
module(GSL       "GSL        ")
module(ZLIB      "Zlib       ")
module(CURSES    "Ncurses    ")
message("")
