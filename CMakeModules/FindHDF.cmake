#
# copyright : (c) 2010 Maxime Lenoir, Alain Coulais,
#                      Sylwester Arabas and Orion Poplawski
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#


# HDF_LIBRARIES
# HDF_EXTRA_LIBRARIES
find_package(JPEG)
find_library(HDF_LIBRARY NAMES df dfalt PATH_SUFFIXES hdf)
find_library(MFHDF_LIBRARY NAMES mfhdf mfhdfalt PATH_SUFFIXES hdf)
set(HDF_LIBRARIES ${MFHDF_LIBRARY} ${HDF_LIBRARY})
find_path(HDF_INCLUDE_DIR NAMES hdf.h PATH_SUFFIXES hdf)
include(CheckLibraryExists)
include(FindPackageHandleStandardArgs)
if(HDF_LIBRARIES)
	set(CMAKE_REQUIRED_LIBRARIES z ${JPEG_LIBRARIES})
	check_library_exists("${HDF_LIBRARIES}" Hopen "" HDF_WO_SZIP)
	if(HDF_WO_SZIP)
		set(HDF_EXTRA_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})
		find_package_handle_standard_args(HDF DEFAULT_MSG HDF_LIBRARIES HDF_EXTRA_LIBRARIES HDF_INCLUDE_DIR)
	else(HDF_WO_SZIP)
		find_library(SZIP_LIBRARIES NAMES sz szip)
		if(SZIP_LIBRARIES)
			set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${SZIP_LIBRARIES})
			check_library_exists("${HDF_LIBRARIES}" Hopen "" HDF_W_SZIP)	
			if(HDF_W_SZIP)
				set(HDF_EXTRA_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})
				find_package_handle_standard_args(HDF DEFAULT_MSG HDF_LIBRARIES HDF_EXTRA_LIBRARIES HDF_INCLUDE_DIR)
			endif(HDF_W_SZIP)
		endif(SZIP_LIBRARIES)
	endif(HDF_WO_SZIP)
	set(CMAKE_REQUIRED_LIBRARIES)
endif(HDF_LIBRARIES)

mark_as_advanced(
HDF_LIBRARY
MFHDF_LIBRARY
HDF_LIBRARIES
HDF_INCLUDE_DIR
HDF_WO_SZIP
HDF_EXTRA_LIBRARIES
SZIP_LIBRARIES
HDF_W_SZIP
)
