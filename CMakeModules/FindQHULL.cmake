# Find qhull header and library.
#

# This module defines the following uncached variables:
#  QHULL_FOUND, if false, do not try to use qhull.
#  QHULL_INCLUDE_DIRS, where to find qhull/qhull_a.h.
#  QHULL_LIBRARIES, the libraries to link against to use the qhull library
#  QHULL_LIBRARY_DIRS, the directory where the qhull library is found.

find_path(
  QHULL_INCLUDE_DIR
  qhull/qhull_a.h libqhull/qhull_a.h
  PATHS /usr/local/include /usr/include
)

if( QHULL_INCLUDE_DIR )
  find_library(
    QHULL_LIBRARY
    NAMES libqhullstatic libqhull qhullstatic qhull 
    PATHS /usr/lib64 /usr/local/lib /usr/lib
  )
  if( QHULL_LIBRARY )
    set(QHULL_LIBRARY_DIR "")
    get_filename_component(QHULL_LIBRARY_DIRS ${QHULL_LIBRARY} PATH)
    # Set uncached variables as per standard.
    set(QHULL_FOUND ON)
    set(QHULL_INCLUDE_DIRS ${QHULL_INCLUDE_DIR})
    set(QHULL_LIBRARIES ${QHULL_LIBRARY})
  endif(QHULL_LIBRARY)
else(QHULL_INCLUDE_DIR)
  message(FATAL_ERROR "FindQHull: Could not find qhull_a.h")
endif(QHULL_INCLUDE_DIR)
	    
if(QHULL_FOUND)
  if(NOT QHULL_FIND_QUIETLY)
    message(STATUS "FindQHull: Found both qhull_a.h and libqhull.a")
  endif(NOT QHULL_FIND_QUIETLY)
else(QHULL_FOUND)
  if(QHULL_FIND_REQUIRED)
    message(FATAL_ERROR "FindQHull: Could not find qhull_a.h and/or libqhull.a")
  endif(QHULL_FIND_REQUIRED)
endif(QHULL_FOUND)
