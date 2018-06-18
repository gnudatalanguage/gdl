#
# copyright : (c) 2010 Maxime Lenoir and Alain Coulais
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#

# tested with 2.4 and doesn't seem to run.
# SA: 2.6 supports simplified if constructs, e.g. endif()
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(GDL)

# X.X.X CVS becomes release X.X.X+1
set(VERSION "0.9.8 git")
enable_testing()

include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckLibraryExists)
include(CheckFunctionExists)
include(CheckSymbolExists)
include(CheckCSourceRuns)
include(CheckCXXSourceCompiles)
include(FindPkgConfig)
include(FindPackageHandleStandardArgs)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMakeModules)

#### Default cached values
set(PYTHON_MODULE OFF CACHE BOOL "GDL: Build GDL as a Python module ?")

set(X11 ON CACHE BOOL "GDL: Enable X11 ?") 
set(X11DIR "" CACHE PATH "GDL: Specify X11 directory tree")

# GNU Readline or BSD Editline ? if Readline OK, we don't look for Editline.
set(READLINE ON CACHE BOOL "GDL: Enable GNU Readline ?")
set(READLINEDIR "" CACHE PATH "GDL: Specify the GNU Readline directory tree")

set(EDITLINE ON CACHE BOOL "GDL: Enable BSD Editline ?")
set(EDITLINEDIR "" CACHE PATH "GDL: Specify the BSD Editline directory tree")

set(OLDPLPLOT OFF CACHE BOOL "GDL: Enable old plplot ?")
set(PLPLOTDIR "" CACHE PATH "GDL: Specify the plplot directory tree")

set(GSLDIR "" CACHE PATH "GDL: Specify the GSL directory tree")

set(ZLIBDIR "" CACHE PATH "GDL: Specify the Zlib directory tree")

set(WXWIDGETS ON CACHE BOOL "GDL: Enable WxWidgets ?")
set(WXWIDGETSDIR "" CACHE PATH "GDL: Specify WxWidgets directory tree")

set(UDUNITS OFF CACHE BOOL "GDL: Enable UDUNITS-2 ?")
set(UDUNITSDIR "" CACHE PATH "GDL: Specify the UDUNITS-2 directory tree")

set(EIGEN3 ON CACHE BOOL "GDL: Enable Eigen3 ?")
set(EIGEN3DIR "" CACHE PATH "GDL: Specify the Eigen3 directory tree")

set(PSLIB ON CACHE BOOL "GDL: Enable pslib ?")
set(PSLIBDIR "" CACHE PATH "GDL: Specify the pslib directory tree")

set(PNGLIB ON CACHE BOOL "GDL: Enable libpng ?")
set(PNGLIBDIR "" CACHE PATH "GDL: Specify the libpng directory tree")

set(GRIB OFF CACHE BOOL "GDL: Enable Grib ?")
set(GRIBDIR "" CACHE PATH "GDL: Specifiy the GRIB directory tree")

# set(QHULL OFF CACHE BOOL "GDL: Enable Qhull ?")
# set(QHULLDIR "" CACHE PATH "GDL: Specifiy the QHULL directory tree")

set(GLPK OFF CACHE BOOL "GDL: Enable GLPK (for function SIMPLEX) ?")
set(GLPKDIR "" CACHE PATH "GDL: Specify the GLPK directory tree")

# GraphicsMagick is a good alternative to ImageMagick, if GM OK, we don't look for IM
set(GRAPHICSMAGICK ON CACHE BOOL "GDL: Enable GraphicsMagick ?")
set(GRAPHICSMAGICKDIR "" CACHE PATH "GDL: Specify the GraphicsMagick directory tree")

set(MAGICK ON CACHE BOOL "GDL: Enable ImageMagick ?")
set(MAGICKDIR "" CACHE PATH "GDL: Specifiy the ImageMagick directory tree")

set(NETCDF ON CACHE BOOL "GDL: Enable NetCDF ?")
set(NETCDFDIR "" CACHE PATH "GDL: Specifiy the netCDF directory tree")

set(HDF ON CACHE BOOL "GDL: Enable Hdf ?")
set(HDFDIR "" CACHE PATH "GDL: Specifiy the HDF directory tree")

set(HDF5 ON CACHE BOOL "GDL: Enable Hdf5 ?")
set(HDF5DIR "" CACHE PATH "GDL: Specifiy the HDF5 directory tree")

set(FFTW ON CACHE BOOL "GDL: Enable FFTW ?")
set(FFTWDIR "" CACHE PATH "GDL: Specifiy the FFTW directory tree")

set(LIBPROJ4 OFF CACHE BOOL "GDL: Enable LIBPROJ4 ?")
set(LIBPROJ4DIR "" CACHE PATH "GDL: Specifiy the LIBPROJ4 directory tree")

set(MPICH OFF CACHE BOOL "GDL: Enable MPICH ?")
set(MPICHDIR "" CACHE PATH "GDL: Specify the MPICH (experimental) directory tree")

set(PYTHON ON CACHE BOOL "GDL: Enable Python ?")
set(PYTHONDIR "" CACHE PATH "GDL: Specifiy the use Python directory tree")
set(PYTHONVERSION "" CACHE STRING "GDL: Specify the Python version to use")

set(GSHHS ON CACHE BOOL "GDL: Enable GSHHS ?")
set(GSHHSDIR "" CACHE PATH "GDL: Specifiy the GSHHS directory tree")

if(NOT WIN32)
  set(NCURSESDIR "" CACHE PATH "GDL: Specify the ncurses (or curses) directory tree")
endif(NOT WIN32)

set(OPENMP ON CACHE BOOL "GDL: Enable OpenMP ?")

# Third party libraries
set(JASPERDIR "" CACHE PATH "GDL: Specify the JasPer directory tree")
set(JPEGDIR "" CACHE PATH "GDL: Specify the JPEG directory tree")
set(SZIPDIR "" CACHE PATH "GDL: Specify the SZip directory tree")

set(GDL_DATA_DIR "/share/gnudatalanguage" CACHE PATH "GDL: data directory relative to CMAKE_INSTALL_PREFIX")

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
#check_include_file(ext/stdio_filebuf.h HAVE_EXT_STDIO_FILEBUF_H)
#sometimes it compiles but stdio_filebuf is only in gcc-x-y-z directory tree...
# would be silly not to use this functionality if present, so recheck:
if (HAVE_EXT_STDIO_FILEBUF_H)
else (HAVE_EXT_STDIO_FILEBUF_H)
	check_cxx_source_compiles("
#include <ext/stdio_filebuf.h>
int main(int argc, char **argv) {
    int i=0;
    i+=1;
}" REALLY_HAVE_EXT_STDIO_FILEBUF_H)
endif (HAVE_EXT_STDIO_FILEBUF_H)
if (REALLY_HAVE_EXT_STDIO_FILEBUF_H)
message(STATUS "INFO: will use GNU extensions for STDIO (useful for compressed I/O) since it seems accepted by your c++ compiler.")
set (HAVE_EXT_STDIO_FILEBUF_H 1)
endif (REALLY_HAVE_EXT_STDIO_FILEBUF_H)
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

if (NOT WIN32)
	# Ncurses MANDATORY for readline on POSIX
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
				"Use -DNCURSESDIR=DIR to specify the curses directory tree.\n"
				"(suitable Debian/Ubuntu package: libncurses-dev)\n")
		endif(CURSES_FOUND)
	endif(CURSES_FOUND)
endif (NOT WIN32)

# GNU Readline (GNU/Linux or OSX) or BSD Editline (OSX)

# -DREADLINE=ON|OFF
# -DREADLINEDIR=DIR or -DEDITLINEDIR=DIR
if(READLINE)
  set(CMAKE_PREFIX_PATH ${READLINEDIR})
  find_package(Readline QUIET)
  set(HAVE_LIBREADLINE ${READLINE_FOUND})
  if(READLINE_FOUND)
    if(NOT WIN32)
      # readline needs (n)curses (not on Windows)
      set(CMAKE_REQUIRED_LIBRARIES ${CURSES_LIBRARIES})
    endif(NOT WIN32)
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
    if(EDITLINE)
      message(STATUS "WARNING: GNU readline was not found, now looking for BSD Editline.\n")
    else(EDITLINE)
      message(FATAL_ERROR "GNU readline was not found.\n"
	"Use -DREADLINEDIR=no to explicitely disable it.\n"
	"(suitable Debian/Ubuntu package: libreadline-gplv2-dev)\n"
	"(suitable Fedora package: readline-devel)")
    endif(EDITLINE)    
  endif(READLINE_FOUND)
endif(READLINE)
#
# if Readline found, we do not look for Editline
#
if(READLINE_FOUND)
  if (EDITLINE)
    message(STATUS "INFO: We prefer to use Readline than Editline.\n"
      "Feedback welcome on OSX Readline ...")
    set(EDITLINE OFF)
  endif(EDITLINE)
endif(READLINE_FOUND)

# BSD Editline (a BSD clone of GNU Readline, present by default on OSX)
# -DEDITLINE=ON|OFF
# -DEDITLINEDIR=DIR
if(EDITLINE)
  set(CMAKE_PREFIX_PATH ${EDITLINEDIR})
  find_package(Editline QUIET)
  set(HAVE_LIBEDITLINE ${EDITLINE_FOUND})
  if(EDITLINE_FOUND)
    if(NOT WIN32)
      # readline needs (n)curses (not on Windows)
      set(CMAKE_REQUIRED_LIBRARIES ${CURSES_LIBRARIES})
    endif(NOT WIN32)
    set(LIBRARIES ${LIBRARIES} ${EDITLINE_LIBRARIES})
    include_directories(${EDITLINE_INCLUDE_DIR})
    set(CMAKE_REQUIRED_LIBRARIES)
  else(EDITLINE_FOUND)
    message(FATAL_ERROR "requested BSD Editline was not found.\n"
      "Use -DEDITLINE=no to explicitely disable it.\n")
  endif(EDITLINE_FOUND)
endif(EDITLINE)

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
	"Use -DZLIBDIR=DIR to specify the zlib directory tree.\n"
        "(suitable Debian/Ubuntu package: zlib1g-dev)\n"
        "(suitable Fedora package: zlib-devel)")
endif(ZLIB_FOUND)

# libpng
# -DPNGLIB=ON|OFF
# -DPNGLIBDIR=DIR
if(PNGLIB)
	set(CMAKE_PREFIX_PATH ${PNGLIBDIR})
	find_package(PNG QUIET)
	set(USE_PNGLIB ${PNG_FOUND})
	if(PNG_FOUND)
		set(LIBRARIES ${LIBRARIES} ${PNG_LIBRARIES})
		set(LINK_DIRECTORIES ${LINK_DIRECTORIES} ${PNG_LIBRARY_DIRS})
		include_directories(${PNG_INCLUDE_DIRS})
	else(PNG_FOUND)
		message(FATAL_ERROR "libpng is required but was not found.\n"
		"Use -DPNGLIBDIR=DIR to specify the libpng directory tree.\n"
		"Use -DPNGLIB=OFF to not use it.\n"
                "(suitable Debian/Ubuntu package: libpng-dev)\n"
                "(suitable Fedora package: libpng-devel)")
	endif(PNG_FOUND)
endif(PNGLIB)

# openmp
# -DOPENMP=ON|OFF
if(OPENMP)
  find_package(OpenMP QUIET)
  set(USE_OPENMP ${OPENMP_FOUND})
  if(OPENMP_FOUND)
    add_definitions(${OpenMP_CXX_FLAGS})
    if(MSVC)
      set(LIBRARIES ${LIBRARIES} vcomp)
    elseif(WIN32)
      set(LIBRARIES ${LIBRARIES} gomp pthread)
    else()
      set(LIBRARIES ${LIBRARIES} ${OpenMP_CXX_FLAGS})
    endif()
  endif(OPENMP_FOUND)
else(OPENMP)
  # we need to define those semaphore posix symbols, do it with threads libs
  find_package(Threads)
  if(THREADS_FOUND)
    set(LIBRARIES ${LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
  else(THREADS_FOUND)
    check_library_exists(rt sem_unlink "" HAVE_RT)
    if(HAVE_RT)
      set(LIBRARIES ${LIBRARIES} rt)
    else(HAVE_RT)
      message(FATAL_ERROR "Missing a POSIX semaphore symbols (rt or threads).\n")
    endif(HAVE_RT)
  endif(THREADS_FOUND)
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
	"Use -DGSLDIR=DIR to specify the gsl directory tree.\n"
        "(suitable Debian/Ubuntu package: libgsl0-dev)\n"
        "(suitable Fedora package: gsl-devel)")
endif(GSL_FOUND)

if(WIN32 AND NOT CYGWIN)
set(CMAKE_PREFIX_PATH ${XDRDIR})
find_package(Xdr)
set(HAVE_LIBXDR ${XDR_FOUND})
if(XDR_FOUND)
	set(LIBRARIES ${LIBRARIES} ${XDR_LIBRARY})
	include_directories(${XDR_INCLUDE_DIR})
else(XDR_FOUND)
	message(FATAL_ERROR "bsd-xdr library is required but was not found.\n"
	"Use -DXDRDIR=DIR to specify the bsd-xdr directory tree.")
endif(XDR_FOUND)

set(CMAKE_PREFIX_PATH ${PCREDIR})
find_package(PCRE)
set(HAVE_LIBPCRE ${PCRE_FOUND})
if(PCRE_FOUND)
	set(LIBRARIES ${LIBRARIES} ${PCRE_LIBRARIES})
	include_directories(${PCRE_INCLUDE_DIR})
else(PCRE_FOUND)
	message(FATAL_ERROR "pcre library is required but was not found.\n"
	"Use -DPCREDIR=DIR to specify the pcre directory tree.")
endif(PCRE_FOUND)
LINK_LIBRARIES(shlwapi)
endif(WIN32 AND NOT CYGWIN)

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
	"Use -DPLPLOTDIR=DIR to specify the plplot directory tree.\n"
        "(suitable Debian/Ubuntu packages: libplplot-dev, [plplot9-driver-xwin])"
        "(suitable Fedora package: plplot-devel)")
endif(PLPLOT_FOUND)

if(PLPLOT_FOUND)
    message(STATUS "Warning, if you have plplot version > 5.11 and wxWidgets enabled, please read carefully file README")
	check_library_exists("${PLPLOT_LIBRARIES}" c_plslabelfunc "" HAVE_PLPLOT_SLABELFUNC)
	if(HAVE_PLPLOT_SLABELFUNC)
	  set(HAVE_PLPLOT_SLABELFUNC 1)
	else(HAVE_PLPLOT_SLABELFUNC)
	  message(STATUS "warning, due to old plplot library, [XYZ]TICKFORMAT option for plot axis will not be supported.\n"
	    "you should upgrade to plplot version > 5.9.6")
	endif(HAVE_PLPLOT_SLABELFUNC)
	set(CMAKE_REQUIRED_INCLUDES "${PLPLOT_INCLUDE_DIR}")
	set(CMAKE_REQUIRED_LIBRARIES "${PLPLOT_LIBRARIES}")

#the following does not work with clang (???)
	check_library_exists("${PLPLOT_LIBRARIES}" c_plwidth "" HAVE_PLPLOT_WIDTH)


#give it a 2nd try...
        if(NOT HAVE_PLPLOT_WIDTH)
	check_cxx_source_compiles("#include <plplot/plstream.h>
                                    int main(int argc, char **argv) {
                                      plstream *p = new plstream();
                                      PLFLT w = 0.5;
                                      p->width(w);
                                    }" HAVE_PLPLOT_WIDTH)

            if(HAVE_PLPLOT_WIDTH)
              set(HAVE_PLPLOT_WIDTH 1)
            endif(HAVE_PLPLOT_WIDTH)
        endif(NOT HAVE_PLPLOT_WIDTH)
	check_library_exists("${PLPLOT_LIBRARIES}" plstrl "" PLPLOT_PRIVATE_NOT_HIDDEN)
	if(PLPLOT_PRIVATE_NOT_HIDDEN)
	  set(PLPLOT_PRIVATE_NOT_HIDDEN 1)
	else(PLPLOT_PRIVATE_NOT_HIDDEN)
	  message(STATUS "Using a plplot library without private functions - workarounds will be used.")
	endif(PLPLOT_PRIVATE_NOT_HIDDEN)
	check_cxx_source_compiles("#include <plplot/plstream.h>
				   int main(){PLINT n; PLFLT x,y;plcallback::fill(n,&x,&y);}" PLPLOT_HAS_PLCALLBACK)
	if(PLPLOT_HAS_PLCALLBACK)
	  set(PLPLOT_HAS_PLCALLBACK 1)
	endif(PLPLOT_HAS_PLCALLBACK)
endif(PLPLOT_FOUND)
	
# GraphicsMagick (GM) is an alternative to the classical ImageMagick Lib (IM).
# It was experienced that GM was more stable in time than IM
#
# -DGRAPHICSMAGICK=ON|OFF
# -DGRAPHICSMAGICKDIR=DIR
#
if(GRAPHICSMAGICK)
	set(CMAKE_PREFIX_PATH ${GRAPHICSMAGICKDIR})
	find_package(GraphicsMagick QUIET)
	set(USE_MAGICK ${GRAPHICSMAGICK_FOUND})
	if(GRAPHICSMAGICK_FOUND)
		include_directories(${GRAPHICSMAGICK_INCLUDE_DIR})	
		set(LIBRARIES ${LIBRARIES} ${GRAPHICSMAGICK_LIBRARIES})		
	else(GRAPHICSMAGICK_FOUND)
		message( STATUS
		"GRAPHICSMAGICK is strongly suggested but was not found.
		Use -DGRAPHICSMAGICKDIR=DIR to specify the GraphicsMagick directory tree.
		Use -DGRAPHICSMAGICK=OFF to not use it.
                (suitable Fedora package: GraphicsMagick-c++-devel
		   Debian/Ubuntu package: libgraphicsmagick++1-dev)\n")
		message( STATUS 
		"Looking for ImageMagick")
	endif(GRAPHICSMAGICK_FOUND)
endif(GRAPHICSMAGICK)
#
# if GM found, we do not look for IM
#
if(GRAPHICSMAGICK_FOUND)
	if (MAGICK)
	   message(STATUS "INFO: We prefer to use GraphicsMagick than ImageMagick")
	   set(MAGICK OFF)
        endif(MAGICK)
endif(GRAPHICSMAGICK_FOUND)

# ImageMagick
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
	"Use -DMAGICK=OFF to not use it.\n"
        "(suitable Debian/Ubuntu package: libmagick++-dev)\n"
        "(suitable Fedora package: ImageMagick-c++-devel)")
    endif(MAGICKXXCONFIG)
    set(LIBRARIES ${LIBRARIES} ${ImageMagick_LIBRARIES})
    include_directories(${ImageMagick_INCLUDE_DIRS})
    set(MAGICK_LIBRARIES ${ImageMagick_LIBRARIES})
  else(ImageMagick_FOUND)
    message(FATAL_ERROR "ImageMagick is required but was not found.\n"
      "Use -DMAGICKDIR=DIR to specify the ImageMagick directory.\n"
      "Use -DMAGICK=OFF to not use it.\n"
      "(suitable Debian/Ubuntu package: libmagick++-dev)\n"
      "(suitable Fedora package: ImageMagick-c++-devel)")
  endif(ImageMagick_FOUND)
endif(MAGICK)

# if GM or IM activated, we check whether Plplot is OK for that
if(USE_MAGICK AND NOT WIN32)
  check_library_exists("${PLPLOT_LIBRARIES}" plGetDrvDir "" HAVE_PLGETDRVDIR)
  if(HAVE_PLGETDRVDIR)
    message(STATUS "WARNING: Dynamic drivers may have to be disabled in plplot.\n"
      "To make plplot work with ImageMagick:\n"
      "use -DENABLE_DYNDRIVERS=OFF cmake option when compiling plplot or\n"
      "use -DMAGICK=OFF here to not use ImageMagick in GDL.")
  endif(HAVE_PLGETDRVDIR)
endif(USE_MAGICK AND NOT WIN32)

# wxWidgets
# -DWXWIDGETS=ON|OFF
# -DWXWIDGETSDIR=DIR
set(wxWidgets_USE_DEBUG OFF) # Use release build
if(WXWIDGETS)
	#
	# we check whether plplot was compiled within wxwidgets
	#
# AC 2015/04/07 this is not working well, just on few cases :( 
#	check_library_exists("${PLPLOT_LIBRARIES}" plD_DEVICE_INFO_wxwidgets "" HAVE_PLPLOT_WXWIDGETS)
#	if(NOT HAVE_PLPLOT_WXWIDGETS)
#	       message(FATAL_ERROR  "wxWidgets are required but PLplot wxWidgets NOT found.\n"
#	       "If -DWXWIDGETS=ON (default) then PLplot must have be compiled with -DPLD_wxwidgets=ON\n"
#	       "use -DWXWIDGETS=OFF to not use wxWidgets since it is not available here in PLplot")
#        endif(NOT HAVE_PLPLOT_WXWIDGETS)
	#
	set(CMAKE_PREFIX_PATH ${WXWIDGETSDIR})
	find_package(wxWidgets COMPONENTS base core adv)
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
		"Use -DWXWIDGETS=OFF to not use it.\n"
                "(suitable Debian/Ubuntu package: libwxgtk2.8-dev)\n"
                "(suitable Fedora package: wxGTK-devel)")
	endif(wxWidgets_FOUND)
endif(WXWIDGETS)

# netCDF
# -DNETCDF=ON|OFF
# -DNETCDFDIR=DIR
if(NETCDF)
	set(CMAKE_PREFIX_PATH ${NETCDFDIR})
	find_package(NetCDF QUIET)
	if(NETCDF_FOUND)
		set(CMAKE_REQUIRED_INCLUDES ${NETCDF_INCLUDE_DIRS})
		check_include_file_cxx(netcdf.h HAVE_NETCDF_H)
		if(HAVE_NETCDF_H)
			set(LIBRARIES ${LIBRARIES} ${NETCDF_LIBRARIES})
			set(LINK_DIRECTORIES ${LINK_DIRECTORIES} ${NETCDF_LIBRARY_DIRS})
			include_directories(${NETCDF_INCLUDE_DIRS})
			set(USE_NETCDF 1)
		else(HAVE_NETCDF_H)
			message(FATAL_ERROR "NetCDF installation seems not to be usable.\n"
			"This suggests a conflicting netCDF-HDF4 installation e.g.\n"
			"- Uninstalling HDF4 after installation of NetCDF.\n"
			"- Installing NetCDF before HDF4.")
		endif(HAVE_NETCDF_H)
		set(CMAKE_REQUIRED_INCLUDES)
		#
		# are extensions NetCDF-4 available ??
		check_library_exists("${NETCDF_LIBRARIES}" nc_inq_grps "" HAVE_NETCDF4)
		if(HAVE_NETCDF4)
		  set(USE_NETCDF4 1)
		else(HAVE_NETCDF4)
		  message(STATUS "warning, you don't have NetCDF-4 version"
		    "some new NetCDF capabilities in NetCDF-4 (related to Groups) will not be usable")
		endif(HAVE_NETCDF4)
	else(NETCDF_FOUND)
		message(FATAL_ERROR "NetCDF version 3.5.1 or later is required but was not found.\n"
		"Use -DNETCDFDIR=DIR to specify the netcdf directory tree.\n"
		"Use -DNETCDF=OFF to not use it.\n"
                "(suitable Debian/Ubuntu package: libnetcdf-dev)\n"
                "(suitable Fedora package: netcdf-devel)")
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
		"Use -DHDF=OFF to not use it.\n"
                "(suitable Debian/Ubuntu package: libhdf4-alt-dev)\n"
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
		"Use -DHDF5=OFF to not use it.\n"
                "(suitable Debian/Ubuntu package: libhdf5-serial-dev)\n"
                "(suitable Fedora package: hdf5-devel)")
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
		"Use -DFFTW=OFF to not use it.\n"
                "(suitable Debian/Ubuntu package: libfftw3-dev)\n"
                "(suitable Fedora package: fftw-devel)")
	endif(FFTW_FOUND)
endif(FFTW)

# libproj4
# -DLIBPROJ4=ON|OFF
# -DLIBPROJ4DIR=DIR
if(LIBPROJ4)
	set(CMAKE_PREFIX_PATH ${LIBPROJ4DIR})
	find_package(Libproj4)
	if(LIBPROJ4_FOUND)	
	else(LIBPROJ4_FOUND)
	find_package(Libproj4new)
	endif(LIBPROJ4_FOUND)	

	if(LIBPROJ4_FOUND)	
		set(CMAKE_REQUIRED_LIBRARIES ${LIBPROJ4_LIBRARIES} ${GSL_LIBRARIES} m)

		check_library_exists("${LIBPROJ4_LIBRARIES}" pj_init "" USE_LIBPROJ4_NEW)
		if(USE_LIBPROJ4_NEW)
#		check_library_exists("${LIBPROJ4_LIBRARIES}" proj_init "" USE_LIBPROJ4)
#		if(USE_LIBPROJ4)

                        # define USE_LIB_PROJ4 in any case
			set(USE_LIBPROJ4 1)

		else(USE_LIBPROJ4_NEW)
			check_library_exists("${LIBPROJ4_LIBRARIES}" proj_init "" USE_LIBPROJ4)
			if(NOT USE_LIBPROJ4)
#		else(USE_LIBPROJ4)
#			check_library_exists("${LIBPROJ4_LIBRARIES}" pj_init "" USE_LIBPROJ4_NEW)
#			if(NOT USE_LIBPROJ4_NEW)

				message(FATAL_ERROR "Libproj4 is required but was not found.\n"
				"Use -DLIBPROJ4DIR=DIR to specify the Libproj4 directory tree.\n"
				"Use -DLIBPRJ4=OFF to not use it.")

			endif(NOT USE_LIBPROJ4)
		endif(USE_LIBPROJ4_NEW)
#			endif(NOT USE_LIBPROJ4_NEW)
#		endif(USE_LIBPROJ4)

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
	if(PYTHONDIR)
		find_library(PYTHONLIBS 
                  NAMES python${PYTHONVERSION}
                  PATHS ${PYTHONDIR} 
                  PATH_SUFFIXES lib lib/python${PYTHONVERSION}/config 
                  NO_DEFAULT_PATH NO_CMAKE_PATH
                )
                string(COMPARE NOTEQUAL "${PYTHONLIBS}" "PYTHONLIBS-NOTFOUND" PYTHONLIBS_FOUND) 
                if (NOT PYTHONLIBS_FOUND)
                  message(FATAL_ERROR "Python libraries not found in ${PYTHONDIR}.")
                else()
                  set(PYTHON_LIBRARIES "${PYTHONLIBS}")
                  message("-- Found Python libs: ${PYTHONLIBS}")
                endif()
                find_path(PYTHON_INCLUDE_DIRS 
                  NAMES Python.h 
                  PATHS ${PYTHONDIR}
                  PATH_SUFFIXES include include/python${PYTHONVERSION}
                  NO_DEFAULT_PATH NO_CMAKE_PATH
                )
                string(COMPARE NOTEQUAL "${PYTHONLIBS}" "PYTHON_INCLUDE_DIRS-NOTFOUND" PYTHONHEADERS_FOUND) 
                if (PYTHONHEADERS_FOUND)
                  message("-- Found Python headers: ${PYTHON_INCLUDE_DIRS}")
                  set(PYTHON_INCLUDES "${PYTHON_INCLUDE_DIRS}")
                else()
                  message(FATAL_ERROR "Python headers not found in ${PYTHONDIR}.")
                endif()
                find_program(PYTHON_EXECUTABLE NAMES python${PYTHONVERSION} PATHS ${PYTHONDIR} PATH_SUFFIXES bin NO_DEFAULT_PATH NO_CMAKE_PATH)
                string(COMPARE NOTEQUAL "${PYTHON_EXECUTABLE}" "PYTHON_EXECUTABLE-NOTFOUND" PYTHON_EXECUTABLE_FOUND) 
                if(NOT PYTHON_EXECUTABLE_FOUND)
                  message(FATAL_ERROR "Python executable not found in ${PYTHONDIR}.")
                else()
                  message("-- Found Python executable: ${PYTHON_EXECUTABLE}")
                endif()
	else()
		if(NOT PYTHONVERSION)
			set(PYTHONVERSION 2)
		endif()
		find_package(PythonInterp)
		find_package(PythonLibs ${PYTHONVERSION})
	endif()
	set(USE_PYTHON ${PYTHONLIBS_FOUND})
	if(PYTHONLIBS_FOUND)
		find_package(Numpy QUIET)
		if(NOT PYTHON_NUMPY_FOUND)
			message(FATAL_ERROR "Python numpy package was not found.\n"
			"${PYTHONMSG} ${PYTHON_MODULEMSG}")
		endif(NOT PYTHON_NUMPY_FOUND)
		set(LIBRARIES ${LIBRARIES} ${PYTHON_LIBRARIES})
		include_directories(${PYTHON_INCLUDE_DIRS} ${PYTHON_NUMPY_INCLUDE_DIR})	
	else(PYTHONLIBS_FOUND)
		message(FATAL_ERROR "Python is required but was not found.\n"
		"Use -DPYTHONDIR=DIR to specify the Python directory tree.\n"
                "Use -DPYTHONVERSION=VERSION to help searching for the right version.\n"
                "(suitable Debian/Ubuntu package: python-dev)\n"
                "(suitable Fedora package: python-devel)\n"
		"${PYTHONMSG} ${PYTHON_MODULEMSG}")
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
		"Use -DUDUNITS=OFF to not use it.\n"
                "(suitable Debian/Ubuntu package: libudunits2-dev)\n"
                "(suitable Fedora package: udunits2-devel)")
	endif(UDUNITS_FOUND)
endif(UDUNITS)

# eigen3
# -DEIGEN3=ON|OFF
# -DEIGEN3DIR=DIR
if(EIGEN3)
  # cleaning Cache ...
  if (EIGEN3DIR)
    unset(EIGEN3_INCLUDE_DIR CACHE)
  endif(EIGEN3DIR)
  #
  set(CMAKE_PREFIX_PATH ${EIGEN3DIR})
  find_package(Eigen3 3.2.4 QUIET)
  if(EIGEN3_TOO_OLD)
    # on iCore 3/5/7 we must have Eigen >=3.2.4 to avoid fatal error in "test_matrix_multiply.pro"
    message(FATAL_ERROR "\nEIGEN3 is required but the version found is TOO OLD."
          " Please download a recent version (>=3.2.4) in a local directory."
	  " Then use -DEIGEN3DIR=DIR to specify the Eigen3 local directory tree.\n")
      else(EIGEN3_TOO_OLD)	 
	set(USE_EIGEN ${EIGEN3_FOUND})
	if(EIGEN3_FOUND)
	  include_directories(${EIGEN3_INCLUDE_DIR})
	  if(MINGW)
	    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-ipa-cp-clone")
	  endif(MINGW)
	else(EIGEN3_FOUND)
	  message(FATAL_ERROR "EIGEN3 is required but was not found.\n"
	    "Use -DEIGEN3DIR=DIR to specify the Eigen3 directory tree.\n"
	    "Use -DEIGEN3=OFF to not use it.\n"
            "(suitable Fedora package: eigen3-devel)\n"
            "(suitable Debian/Ubuntu package: libeigen3-dev)")
	endif(EIGEN3_FOUND)
      endif(EIGEN3_TOO_OLD)
endif(EIGEN3)

# pslib
# -DPSLIB=ON|OFF
# -DPSLIBDIR=DIR
if(PSLIB)
	set(CMAKE_PREFIX_PATH ${PSLIBDIR})
	find_package(libps QUIET)
	set(USE_PSLIB ${LIBPS_FOUND})
	if(LIBPS_FOUND)
		set(LIBRARIES ${LIBRARIES} ${LIBPS_LIBRARIES})
		set(LINK_DIRECTORIES ${LINK_DIRECTORIES} ${LIBPS_LIBRARY_DIRS})
		include_directories(${LIBPS_INCLUDE_DIRS})
	else(LIBPS_FOUND)
		message(FATAL_ERROR "pslib is required but was not found.\n"
		"Use -DPSLIBDIR=DIR to specify the pslib directory tree.\n"
		"Use -DPSLIB=OFF to not use it.\n"
                "(suitable Debian/Ubuntu package: pslib-dev)\n"
                "(suitable Fedora package: pslib-devel)")
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
		"the directory trees of JasPer and openJPEG libraries.\n"
                "(suitable Debian/Ubuntu package: libgrib-api-dev)\n"
                "(suitable Fedora package: grib_api-devel)")
	endif(GRIB_FOUND)
endif(GRIB)

# QHULL IS NOT NEEDED YET!
# # Qhull (enable TRIANGULATE and QHULL commands)
# # -DQHULL=ON|OFF
# # -DQHULLDIR=DIR
# if(QHULL)
# 	set(CMAKE_PREFIX_PATH ${QHULLDIR})
# 	find_package(QHULL QUIET)
# 	set(PL_HAVE_QHULL ${QHULL_FOUND})
# 	if(QHULL_FOUND)
# 		set(LIBRARIES ${LIBRARIES} ${QHULL_LIBRARIES})
# 		include_directories(${QHULL_INCLUDE_DIR})
# 	else(QHULL_FOUND)
# 		message(FATAL_ERROR "QHULL is required but was not found.\n"
# 		"Use -DQHULLDIR=DIR to specify the QHULL directory tree.\n"
# 		"Use -DQHULL=OFF to not use it.\n"
#                 "(suitable Fedora package: libqhull-devel)")
# 	endif(QHULL_FOUND)
# endif(QHULL)

# GLPK for Simplex ( Linear programming )
# GLPK (enable SIMPLEX command)
# -DGLPK=ON|OFF
# -DGLPKDIR=DIR
if(GLPK)
  set(CMAKE_PREFIX_PATH ${GLPKDIR})
  find_package(GLPK QUIET)
  set(USE_GLPK ${GLPK_FOUND})
  if(GLPK_FOUND)
    set(LIBRARIES ${LIBRARIES} ${GLPK_LIBRARIES})
    include_directories(${GLPK_INCLUDE_DIR})
  else(GLPK_FOUND)
    message(FATAL_ERROR "GLPK (Gnu Linear Programming Kit) is required but was not found.\n"
      "Use -DGLPKDIR=DIR to specify the GLPK-devel directory tree.\n"
      "Use -DGLPK=OFF to not use it.\n")
  endif(GLPK_FOUND)
endif(GLPK)
#check_include_file(glpk.h HAVE_GLPK)

# gshhs
# -DGSHHS=ON|OFF
if(GSHHS) 	
    set(USE_GSHHS 1)
endif(GSHHS)
# -DGSHHSDIR=DIR
#not needed anymore, gshhs include is in src/
#	set(CMAKE_PREFIX_PATH ${GSHHSDIR} src/)
#	find_path(GSHHS_INCLUDE_DIR gshhs.h)
#	mark_as_advanced(GSHHS_INCLUDE_DIR)
#	set(USE_GSHHS ${GSHHS_INCLUDE_DIR})
#	if(USE_GSHHS)
#		include_directories(${GSHHS_INCLUDE_DIR})
#	else(USE_GSHHS)
#		message(FATAL_ERROR "GSHHS is required but gshhs.h was not found.\n"
#		"Use -DGSHHSDIR=DIR to specify the GSHHS directory tree.\n"
#		"Use -DGSHHS=OFF to not use it.")
#	endif(USE_GSHHS)
#endif(GSHHS)
#
# X11
if(X11)
	set(CMAKE_PREFIX_PATH ${X11DIR})
	if(WIN32 AND NOT CYGWIN)
		find_package(Xportmingw QUIET)
		set(HAVE_X ${XPORTMINGW_FOUND})
		set(USE_X ${XPORTMINGW_FOUND})
		if(XPORTMINGW_FOUND)
			set(LIBRARIES ${LIBRARIES} ${XPORTMINGW_LIBRARIES})
			include_directories(${XPORTMINGW_INCLUDE_DIR})
		else(XPORTMINGW_FOUND)
			message("X11 was not found.\n"
			"Use -DX11DIR=DIR to specify the X11 directory tree.")
		endif(XPORTMINGW_FOUND)
	else(WIN32 AND NOT CYGWIN)
		find_package(X11 QUIET)
		set(HAVE_X ${X11_FOUND})
		set(USE_X ${X11_FOUND})
		if(X11_FOUND)
			set(LIBRARIES ${LIBRARIES} ${X11_LIBRARIES})
			include_directories(${X11_INCLUDE_DIR})
		else(X11_FOUND)
			message(FATAL_ERROR "X11 is required but was not found.\n"
			"Use -DX11DIR=DIR to specify the X11 directory tree.\n"
			"Use -DX11=OFF to not use it.")
		endif(X11_FOUND)
	endif(WIN32 AND NOT CYGWIN)
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
# for windows, just copy the file
if(NOT PYTHON_MODULE)
	get_target_property(GDLLOCATION gdl LOCATION)
	if(WIN32)
	add_custom_target(copy_gdl ALL ${CMAKE_COMMAND} -E copy ${GDLLOCATION} ${CMAKE_SOURCE_DIR}/src/gdl DEPENDS gdl)
	else(WIN32)
	add_custom_target(symlink_gdl ALL ${CMAKE_COMMAND} -E create_symlink ${GDLLOCATION} ${CMAKE_SOURCE_DIR}/src/gdl DEPENDS gdl)
	endif(WIN32)
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

if(MSVC)
	SET(MACHINE_ARCH ${MSVC_C_ARCHITECTURE_ID})
	IF(NOT MACHINE_ARCH)
  SET(MACHINE_ARCH ${MSVC_CXX_ARCHITECTURE_ID})
	ENDIF(NOT MACHINE_ARCH)
	set_target_properties(gdl PROPERTIES LINK_FLAGS "/machine:${MACHINE_ARCH}")
endif(MSVC)

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
set(EIGEN3_LIBRARIES ${EIGEN3_INCLUDE_DIR})
module(EIGEN3    "EIGEN3     ")
module(GRAPHICSMAGICK "GRAPHICSMAGICK")
module(GRIB      "GRIB       ")
# set(QHULL_LIBRARIES ${QHULL_LIBRARIES})
# module(QHULL     "QHULL      ")
set(GLPK_LIBRARIES ${GLPK_LIBRARIES})
module(GLPK      "GLPK       ")
set(GSHHS_LIBRARIES ${GSHHS_INCLUDE_DIR})
module(GSHHS     "GSHHS      ")
module(PSLIB	 "pslib      ")
if(WIN32 AND NOT CYGWIN)
  module(XPORTMINGW	 "XPortMinGW (Win32 Xlib)       ")
else(WIN32 AND NOT CYGWIN)
  module(X11	 "Xlib       ")
endif(WIN32 AND NOT CYGWIN)
module(PNGLIB    "libpng     ")
message("")
message(STATUS "Mandatory modules")
set(PLPLOT ON)
set(READLINE ON)
set(GSL ON)
set(ZLIB ON)
if(NOT WIN32)
  set(CURSES ON)
endif(NOT WIN32)
module(PLPLOT    "Plplot     ")
module(OLDPLPLOT "Old Plplot ")
module(READLINE  "GNU Readline   ")
module(EDITLINE  "BSD Editline   ")
module(GSL       "GSL        ")
module(ZLIB      "Zlib       ")
if(NOT WIN32)
  module(CURSES    "(N)curses    ")
endif(NOT WIN32)
message("")
