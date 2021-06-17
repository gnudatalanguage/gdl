# Find QHULL
#
# This sets the following variables:
# QHULL_FOUND - True if QHULL was found.
# QHULL_INCLUDE_DIRS - Directories containing the QHULL include files.
# QHULL_LIBRARIES - Libraries needed to use QHULL.

find_path(
        QHULL_INCLUDE_DIR
        libqhullcpp/Qhull.h
        PATHS /usr/local/include /usr/include
        )

if(QHULL_INCLUDE_DIR)
    find_library(
        QHULL_R_LIBRARY
        qhull_r
        PATHS /usr/lib64 /usr/local/lib /usr/lib
        )

    find_library(
        QHULL_CPP_LIBRARY
        qhullcpp
        PATHS /usr/lib64 /usr/local/lib /usr/lib
        )

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(
        QHULL
        REQUIRED_VARS QHULL_R_LIBRARY QHULL_INCLUDE_DIR
        )

    if(QHULL_R_LIBRARY AND QHULL_CPP_LIBRARY)
        SET(QHULL_FOUND ON)
        SET(QHULL_INCLUDE_DIRS ${QHULL_INCLUDE_DIR})
        SET(QHULL_LIBRARIES ${QHULL_R_LIBRARY} ${QHULL_CPP_LIBRARY})
    endif()
else(QHULL_INCLUDE_DIR)
    message(FATAL_ERROR "FindQHull: Could not find Qhull.h")
endif(QHULL_INCLUDE_DIR)
