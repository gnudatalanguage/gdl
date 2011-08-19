#
# copyright : (c) 2010 Maxime Lenoir, Alain Coulais,
#                      Sylwester Arabas and Orion Poplawski
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#


find_library(GRIB_LIBRARIES NAMES grib_api)
find_path(GRIB_INCLUDE_DIR NAMES grib_api.h)
include(FindPackageHandleStandardArgs)
# since there's no grib_api.pc let's check if this installation of grib required jasper and jpeg
include(CheckLibraryExists)
set(CMAKE_REQUIRED_LIBRARIES m)
check_library_exists(${GRIB_LIBRARIES} grib_index_new_from_file "" GRIB_COMPILES)
if(GRIB_COMPILES)
	find_package_handle_standard_args(GRIB DEFAULT_MSG GRIB_LIBRARIES GRIB_INCLUDE_DIR)
else(GRIB_COMPILES)
	find_package(Jasper)
	if(JASPER_FOUND)
		set(CMAKE_REQUIRED_LIBRARIES ${JASPER_LIBRARIES} m)
		check_library_exists(${GRIB_LIBRARIES} grib_index_new_from_file "" GRIB_COMPILES_JASPER)
		if(GRIB_COMPILES_JASPER)
			set(GRIB_LIBRARIES ${GRIB_LIBRARIES} ${JASPER_LIBRARIES})
			set(GRIB_INCLUDE_DIR ${GRIB_INCLUDE_DIR} ${JASPER_INCLUDE_DIR})
			find_package_handle_standard_args(GRIB DEFAULT_MSG GRIB_LIBRARIES GRIB_INCLUDE_DIR)
		endif(GRIB_COMPILES_JASPER)
	endif(JASPER_FOUND)
endif(GRIB_COMPILES)
set(CMAKE_REQUIRED_LIBRARIES)

mark_as_advanced(
GRIB_LIBRARIES
GRIB_INCLUDE_DIR
GRIB_COMPILES
GRIB_COMPILES_JASPER
)
