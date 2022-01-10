# Find QHULL
#
# This sets the following variables:
# QHULL_FOUND - True if QHULL was found.
# QHULL_INCLUDE_DIRS - Directories containing the QHULL include files.
# QHULL_LIBRARIES - Libraries needed to use QHULL.

find_path(QHULL_INCLUDE_DIR NAMES libqhullcpp/Qhull.h
        HINTS ${CMAKE_PREFIX_PATH}/src /usr/local/include /usr/include
        )

if(QHULL_INCLUDE_DIR)
    find_library(QHULL_R_STATIC_LIBRARY NAMES qhullstatic_r HINTS ${CMAKE_PREFIX_PATH})
    find_library(QHULL_R_DYNAMIC_LIBRARY NAMES qhull_r HINTS ${CMAKE_PREFIX_PATH})
    find_library(QHULL_CPP_LIBRARY NAMES qhullcpp HINTS ${CMAKE_PREFIX_PATH})

    # include(FindPackageHandleStandardArgs)
    # find_package_handle_standard_args(
    #     QHULL
    #     REQUIRED_VARS QHULL_R_LIBRARY QHULL_CPP_LIBRARY QHULL_INCLUDE_DIR
    #     )
    if (QHULL_R_STATIC_LIBRARY)
      SET(QHULL_R_LIBRARY ${QHULL_R_STATIC_LIBRARY})
    endif (QHULL_R_STATIC_LIBRARY)
      
    if (QHULL_R_DYNAMIC_LIBRARY)
      SET(QHULL_R_LIBRARY ${QHULL_R_DYNAMIC_LIBRARY})
    endif (QHULL_R_DYNAMIC_LIBRARY)

    if(QHULL_R_LIBRARY AND QHULL_CPP_LIBRARY)
        SET(QHULL_FOUND TRUE)
        SET(QHULL_INCLUDE_DIRS ${QHULL_INCLUDE_DIR})
        SET(QHULL_LIBRARIES ${QHULL_CPP_LIBRARY} ${QHULL_R_LIBRARY})
    elseif(NOT QHULL_CPP_LIBRARY)
        message(STATUS "FindQhull: Could not find QHULL C++ library libqhullcpp")
    else()
        message(STATUS "FindQhull: Could not find QHULL Reentrant library libqhull[static]_r")
    endif()
else(QHULL_INCLUDE_DIR)
    message(STATUS "FindQhull: Could not find QHULL include directories.")
endif(QHULL_INCLUDE_DIR)
