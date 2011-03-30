find_path(NETCDFPKG netcdf.pc PATH_SUFFIXES lib lib/pkgconfig)
include(FindPackageHandleStandardArgs)
if(NETCDFPKG)
	set(ENV{PKG_CONFIG_PATH} ${NETCDFPKG}) # pkg search path
	include(FindPkgConfig)
	pkg_check_modules(NETCDF netcdf)
	if(NETCDF_FOUND)
		find_package_handle_standard_args(NETCDF DEFAULT_MSG NETCDF_LIBRARIES)
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
