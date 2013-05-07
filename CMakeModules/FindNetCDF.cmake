#
# copyright : (c) 2010 Maxime Lenoir, Alain Coulais,
#                      Sylwester Arabas and Orion Poplawski
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#


find_package(PkgConfig QUIET)
pkg_check_modules(NETCDF netcdf)
if(NETCDF_FOUND) # no netcdf.pc file
	find_package_handle_standard_args(NETCDF DEFAULT_MSG NETCDF_LIBRARIES)
else(NETCDF_FOUND) # no netcdf.pc file
	find_library(NETCDF_LIBRARIES NAMES netcdf)
	find_path(NETCDF_INCLUDE_DIRS NAMES netcdf.h PATH_SUFFIXES netcdf-3)
	find_package_handle_standard_args(NETCDF DEFAULT_MSG NETCDF_LIBRARIES NETCDF_INCLUDE_DIRS)
endif(NETCDF_FOUND)

mark_as_advanced(
NETCDF_INCLUDE_DIRS
NETCDF_LIBRARIES
)
