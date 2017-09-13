# Find GLPK header and library.
#

# This module defines the following uncached variables:
#  HAVE_GLPK, if false, do not try to use GLPK.
#  GLPK_INCLUDE_DIRS, where to find glpk.h.
#  GLPK_LIBRARIES, the libraries to link against to use the GLPK library
#  GLPK_LIBRARY_DIRS, the directory where the GLPK library is found.

find_path(
  GLPK_INCLUDE_DIR
  glpk.h
  PATHS /usr/local/include /usr/include
)

if( GLPK_INCLUDE_DIR )
  find_library(
    GLPK_LIBRARY
    NAMES libglpk.a libglpk.so
    PATHS /usr/lib64 /usr/local/lib /usr/lib
  )
  if( GLPK_LIBRARY )
    set(GLPK_LIBRARY_DIR "")
    get_filename_component(GLPK_LIBRARY_DIRS ${GLPK_LIBRARY} PATH)
    # Set uncached variables as per standard.
    set(HAVE_GLPK ON)
    set(GLPK_INCLUDE_DIRS ${GLPK_INCLUDE_DIR})
    set(GLPK_LIBRARIES ${GLPK_LIBRARY})
  endif(GLPK_LIBRARY)
else(GLPK_INCLUDE_DIR)
  message(FATAL_ERROR "FindGLPK: Could not find glpk.h")
endif(GLPK_INCLUDE_DIR)
	    
if(HAVE_GLPK)
  if(NOT GLPK_FIND_QUIETLY)
    message(STATUS "FindGLPK: Found both glpk.h and libglpk.a")
  endif(NOT GLPK_FIND_QUIETLY)
else(HAVE_GLPK)
  if(GLPK_FIND_REQUIRED)
    message(FATAL_ERROR "FindGLPK: Could not find glpk.h and/or libglpk.a")
  endif(GLPK_FIND_REQUIRED)
endif(HAVE_GLPK)
