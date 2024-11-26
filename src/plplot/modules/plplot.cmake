# cmake/modules/plplot.cmake
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
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

# Module for determining all configuration variables for PLplot.

# The "certain devices" I have in mind at the moment are wxwidgets and wingcc since both
# of those devices simply show a blank screen rather than the plot actively being rendered when -np is
# used.  So once those -np issues are fixed for wxwidgets and wingcc, this option will no longer
# be necessary.

  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

  set(NON_TRANSITIVE OFF CACHE BOOL "Option to use non-transitive linking" FORCE)

  set(DEFAULT_CMAP0_FILE "cmap0_default.pal")

  set(DEFAULT_CMAP1_FILE "cmap1_default.pal")

# Deprecated functions are not included by default. Set to ON if you
# require this for backwards compatibility.
option(PL_DEPRECATED "Include support for deprecated functions" OFF)

# Set to ON if want to use general fill_intersection_polygon approach
# rather than the traditional code to fill the intersection of a
# polygon with the clipping limits.

option(USE_FILL_INTERSECTION_POLYGON "use fill_intersection_polygon" OFF)

# Need these modules to do subsequent checks.
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckSymbolExists)
include(CheckPrototypeExists)
# Required for the cmake_push_check_state and cmake_pop_check_state
# macros used to store and retrieve CMAKE_REQUIRED_* variables used in
# conjunction with some of the CMake check_* functions that we call.
include(CMakePushCheckState)

# =======================================================================
# Compilation and build options (PLFLT, install locations, and rpath)
# Note, must come before java since that depends on, e.g., LIB_DIR.
# =======================================================================

include(c++)
include(double)
include(rpath)
# Common CMakeLists.txt files are used to build the examples in the build
# tree and also to build the installed examples by the new CMake-based build
# system devoted to that purpose. Set this fundamental identification to
# distinguish the two cases.
set(CORE_BUILD ON)

option(BUILD_TEST "Compile examples in the build tree and enable ctest" OFF)

# =======================================================================
# Headers
# =======================================================================

# AC_HEADER_STDC is gross overkill since the current PLplot code only uses
# this for whether or not atexit can be used.  But implement the full suite
# of AC_HEADER_STDC checks to keep the cmake version in synch with autotools
# and just in case some PLplot developer assumes the complete check for
# standard headers is done for a future programming change.
#
# From info autoconf....
# Define STDC_HEADERS if the system has ANSI C header files.
# Specifically, this macro checks for stdlib.h', stdarg.h',
# string.h', and float.h'; if the system has those, it probably
# has the rest of the ANSI C header files.  This macro also checks
# whether string.h' declares memchr' (and thus presumably the
# other mem' functions), whether stdlib.h' declare free' (and
# thus presumably malloc' and other related functions), and whether
# the ctype.h' macros work on characters with the high bit set, as
# ANSI C requires.

message(STATUS "Checking whether system has ANSI C header files")
check_include_files("stdlib.h;stdarg.h;string.h;float.h" StandardHeadersExist)
if(StandardHeadersExist)
  check_prototype_exists(memchr string.h memchrExists)
  if(memchrExists)
    check_prototype_exists(free stdlib.h freeExists)
    if(freeExists)
      include(TestForHighBitCharacters)
      if(CMAKE_HIGH_BIT_CHARACTERS)
        message(STATUS "ANSI C header files - found")
        set(STDC_HEADERS 1 CACHE INTERNAL "System has ANSI C header files")
      endif(CMAKE_HIGH_BIT_CHARACTERS)
    endif(freeExists)
  endif(memchrExists)
endif(StandardHeadersExist)
if(NOT STDC_HEADERS)
  message(STATUS "ANSI C header files - not found")
  set(STDC_HEADERS 0 CACHE INTERNAL "System has ANSI C header files")
endif(NOT STDC_HEADERS)

# AC_CHECK_HEADERS(unistd.h termios.h stdint.h)
check_include_files(unistd.h PL_HAVE_UNISTD_H)
check_include_files(termios.h HAVE_TERMIOS_H)
check_include_files(stdint.h PL_HAVE_STDINT_H)
check_include_file(crt_externs.h HAVE_CRT_EXTERNS_H)

# AC_HEADER_SYS_WAIT
include(TestForStandardHeaderwait)

# Reasonable approximation to AC_HEADER_DIRENT without the SCO stuff.
include(CheckDIRSymbolExists)
check_dirsymbol_exists("sys/types.h;dirent.h" HAVE_DIRENT_H)
if(NOT HAVE_DIRENT_H)
  check_dirsymbol_exists("sys/types.h;sys/ndir.h" HAVE_SYS_NDIR_H)
  if(NOT HAVE_SYS_NDIR_H)
    check_dirsymbol_exists("sys/types.h;sys/dir.h" HAVE_SYS_DIR_H)
    if(NOT HAVE_SYS_DIR_H)
      check_dirsymbol_exists("sys/types.h;ndir.h" HAVE_NDIR_H)
      if(NOT HAVE_NDIR_H AND UNIX)
        message(FATAL_ERROR
        "FATAL_ERROR for plplot.cmake: "
        "DIR symbol must be defined by Unix system headers."
        )
      endif(NOT HAVE_NDIR_H AND UNIX)
    endif(NOT HAVE_SYS_DIR_H)
  endif(NOT HAVE_SYS_NDIR_H)
endif(NOT HAVE_DIRENT_H)
# Note the above tests #include <sys/types.h> to follow how
# AC_HEADER_DIRENT does its testing.  Therefore, always do our
# own #defines that way for the cmake build system.  Note, this
# sys/types.h requirement occurs for Mac OS X and possibly other systems.
# It is possible it will go away in the future, but we will follow whatever
# is done by AC_HEADER_DIRENT here until that changes.
set(NEED_SYS_TYPE_H ON)

#=======================================================================
# Typedefs
#=======================================================================

# In the past, some X11 headers required "caddr_t" even on systems that
# claimed POSIX.1 compliance, which was illegal.  This made it impossible
# to compile programs that included X11 headers if _POSIX_SOURCE was
# defined.  We used to work around this potential problem by just defining
# caddr_t to 'char *' on all systems (unless it is set already), whether
# it was needed or not. Now we ignore the issue because we don't expect
# such broken X behaviour any more and because this kind of argument list
# for AC_CHECK_TYPE is now deprecated in the autoconf documentation.

# Do not implement the equivalent of this since commented out in the ABS
# system.
# AC_CHECK_TYPE(caddr_t, char *)

# Test signal handler return type (mimics AC_TYPE_SIGNAL)
include(TestSignalType)

include(CheckFunctionExists)
check_function_exists(popen HAVE_POPEN)
check_function_exists(usleep PL_HAVE_USLEEP)
check_function_exists(nanosleep PL_HAVE_NANOSLEEP)
check_function_exists(mkstemp PL_HAVE_MKSTEMP)
check_function_exists(mkdtemp PL_HAVE_MKDTEMP)
check_function_exists(mkfifo PL_HAVE_MKFIFO)
check_function_exists(unlink PL_HAVE_UNLINK)
check_function_exists(_NSGetArgc HAVE_NSGETARGC)

# Check for FP functions, including underscored version which
# are sometimes all that is available on windows

check_symbol_exists(isfinite "math.h" HAVE_ISFINITE_SYMBOL)
check_symbol_exists(finite "math.h" HAVE_FINITE_SYMBOL)
if(HAVE_ISFINITE_SYMBOL)
  set(PL_HAVE_ISFINITE ON)
else(HAVE_ISFINITE_SYMBOL)
  if(HAVE_FINITE_SYMBOL)
    set(PL_HAVE_FINITE ON)
  else(HAVE_FINITE_SYMBOL)
    check_function_exists(finite HAVE_FINITE_FUNCTION)
    if(HAVE_FINITE_FUNCTION)
      set(PL_HAVE_FINITE ON)
    else(HAVE_FINITE_FUNCTION)
      check_symbol_exists(_finite "math.h" HAVE__FINITE_SYMBOL)
      if(HAVE__FINITE_SYMBOL)
        set(PL__HAVE_FINITE ON)
      else(HAVE__FINITE_SYMBOL)
        check_function_exists(_finite HAVE__FINITE_FUNCTION)
        if(HAVE__FINITE_FUNCTION)
          set(PL__HAVE_FINITE ON)
        endif(HAVE__FINITE_FUNCTION)
      endif(HAVE__FINITE_SYMBOL)
    endif(HAVE_FINITE_FUNCTION)
  endif(HAVE_FINITE_SYMBOL)
endif(HAVE_ISFINITE_SYMBOL)
if(PL_HAVE_FINITE)
  set(PL_HAVE_ISFINITE ON)
endif(PL_HAVE_FINITE)
if(PL__HAVE_FINITE)
  set(PL_HAVE_ISFINITE ON)
endif(PL__HAVE_FINITE)

check_symbol_exists(isnan "math.h" HAVE_ISNAN_SYMBOL)
if(HAVE_ISNAN_SYMBOL)
  set(PL_HAVE_ISNAN ON)
else(HAVE_ISNAN_SYMBOL)
  check_function_exists(isnan HAVE_ISNAN_FUNCTION)
  if(HAVE_ISNAN_FUNCTION)
    set(PL_HAVE_ISNAN ON)
  else(HAVE_ISNAN_FUNCTION)
    check_symbol_exists(_isnan "math.h" HAVE__ISNAN_SYMBOL)
    if(HAVE__ISNAN_SYMBOL)
      set(PL__HAVE_ISNAN ON)
    else(HAVE__ISNAN_SYMBOL)
      check_function_exists(_isnan HAVE__ISNAN_FUNCTION)
      if(HAVE__ISNAN_FUNCTION)
        set(PL__HAVE_ISNAN ON)
      endif(HAVE__ISNAN_FUNCTION)
    endif(HAVE__ISNAN_SYMBOL)
  endif(HAVE_ISNAN_FUNCTION)
endif(HAVE_ISNAN_SYMBOL)
if(PL__HAVE_ISNAN)
  set(PL_HAVE_ISNAN ON)
endif(PL__HAVE_ISNAN)

check_symbol_exists(isinf "math.h" HAVE_ISINF_SYMBOL)
if(HAVE_ISINF_SYMBOL)
  set(PL_HAVE_ISINF ON)
else(HAVE_ISINF_SYMBOL)
  check_function_exists(isinf HAVE_ISINF_FUNCTION)
  if(HAVE_ISINF_FUNCTION)
    set(PL_HAVE_ISINF ON)
  else(HAVE_ISINF_FUNCTION)
    check_symbol_exists(_isinf "math.h" HAVE__ISINF_SYMBOL)
    if(HAVE__ISINF_SYMBOL)
      set(PL__HAVE_ISINF ON)
    else(HAVE__ISINF_SYMBOL)
      check_function_exists(_isinf HAVE__ISINF_FUNCTION)
      if(HAVE__ISINF_FUNCTION)
        set(PL__HAVE_ISINF ON)
      endif(HAVE__ISINF_FUNCTION)
    endif(HAVE__ISINF_SYMBOL)
  endif(HAVE_ISINF_FUNCTION)
endif(HAVE_ISINF_SYMBOL)
if(PL__HAVE_ISINF)
  set(PL_HAVE_ISINF ON)
endif(PL__HAVE_ISINF)


check_symbol_exists(snprintf stdio.h PL_HAVE_SNPRINTF)
if(NOT PL_HAVE_SNPRINTF)
  check_symbol_exists(_snprintf stdio.h _PL_HAVE_SNPRINTF)
  set(PL_HAVE_SNPRINTF ${_PL_HAVE_SNPRINTF} CACHE INTERNAL "Have function _sprintf")
endif(NOT PL_HAVE_SNPRINTF)


include(freetype)

# =======================================================================
# Device drivers
# =======================================================================
# drivers must come after c++ and fortran because of use of filter_rpath
include(drivers)

# =======================================================================
# Miscellaneous other features 
# =======================================================================
include(summary)



