# cmake/modules/xwin.cmake
#
# Copyright (C) 2006  Alan W. Irwin
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

# Module for determining all configuration variables related to the xwin
# device driver.
# The following variables are set/modified:
# PLD_xwin		  - ON means the xwin device is enabled.
# xwin_COMPILE_FLAGS	  - individual COMPILE_FLAGS required to compile xwin
# 			    device.
# xwin_LINK_FLAGS	  - individual LINK_FLAGS for dynamic xwin device.
# DRIVERS_LINK_FLAGS	  - list of LINK_FLAGS for all static devices.
# PL_HAVE_PTHREAD         - ON means use pthreads with xwin driver.
# PLPLOT_MUTEX_RECURSIVE  - Portable definition for PTHREAD_MUTEX_RECURSIVE
if(PLD_xwin)
    set(xwin_COMPILE_FLAGS "${X11_COMPILE_FLAGS}")
    set(xwin_LINK_FLAGS "${X11_LIBRARIES}")
    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
      # turn PL_HAVE_PTHREAD OFF by default for Mac OS X since it doesn't
      # work for Mac OS X 10.4.  Werner says it does work for vanilla
      # XQuartz X11, but the official Apple version of X(Quartz) for 10.5
      # doesn't have all the fixes of the vanilla version so he doesn't trust
      # it.  This his advice for now is to be conservative until we can
      # get a clear report that official X works for 10.5.
      option(PL_HAVE_PTHREAD "Use pthreads with the xwin driver" OFF)
    else(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
      # Turn PL_HAVE_PTHREAD ON by default for other platforms now that
      # the tk segmentation fault has been cured.
      option(PL_HAVE_PTHREAD "Use pthreads with the xwin driver" ON)
    endif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    if(PL_HAVE_PTHREAD)
      find_package(Threads)
      if(CMAKE_USE_PTHREADS_INIT)
        set(xwin_LINK_FLAGS ${xwin_LINK_FLAGS} ${CMAKE_THREAD_LIBS_INIT})
	#cmake_link_flags(xwin_LINK_FLAGS "${xwin_LINK_FLAGS}")
	if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	  set(PLPLOT_MUTEX_RECURSIVE "PTHREAD_MUTEX_RECURSIVE_NP")
 	elseif(CMAKE_SYSTEM_NAME STREQUAL "kFreeBSD")
 	  set(PLPLOT_MUTEX_RECURSIVE "PTHREAD_MUTEX_RECURSIVE_NP")
	else(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	  set(PLPLOT_MUTEX_RECURSIVE "PTHREAD_MUTEX_RECURSIVE")
	endif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
      else(CMAKE_USE_PTHREADS_INIT)
        # I am being super-careful here to follow the autotools model.  In
        # fact, it is possible other thread systems will work as well as
	# pthreads.  So something to investigate for later.
        set(PL_HAVE_PTHREAD OFF)
      endif(CMAKE_USE_PTHREADS_INIT)
    endif(PL_HAVE_PTHREAD)
    set(DRIVERS_LINK_FLAGS ${DRIVERS_LINK_FLAGS} ${xwin_LINK_FLAGS})
endif(PLD_xwin)

