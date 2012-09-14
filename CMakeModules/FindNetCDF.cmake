#
# copyright : (c) 2010 Maxime Lenoir, Alain Coulais,
#                      Sylwester Arabas and Orion Poplawski
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#


find_path(NETCDFPKG netcdf.pc PATH_SUFFIXES lib lib/pkgconfig)
include(FindPackageHandleStandardArgs)
if(NETCDFPKG)
	set(ENV{PKG_CONFIG_PATH} ${NETCDFPKG}) # pkg search path
	include(FindPkgConfig)
	pkg_check_modules(NETCDF netcdf)
	if(NETCDF_FOUND)
		find_package_handle_standard_args(NETCDF DEFAULT_MSG NETCDF_LIBRARIES NETCDF_INCLUDE_DIRS)
	endif(NETCDF_FOUND)
else(NETCDFPKG) # no netcdf.pc file
	find_library(NETCDF_LIBRARIES NAMES netcdf)
	find_path(NETCDF_INCLUDE_DIRS NAMES netcdfcpp.h PATH_SUFFIXES netcdf-3)	
	find_package_handle_standard_args(NETCDF DEFAULT_MSG NETCDF_LIBRARIES NETCDF_INCLUDE_DIRS)
endif(NETCDFPKG)

mark_as_advanced(
NETCDFPKG
NETCDF
NETCDF_INCLUDE_DIRS
NETCDF_LIBRARIES
)
