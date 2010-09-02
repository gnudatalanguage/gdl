find_path(NETCDFPKG netcdf.pc PATH_SUFFIXES lib lib/pkgconfig)
include(FindPackageHandleStandardArgs)
if(NETCDFPKG)
	set(ENV{PKG_CONFIG_PATH} ${NETCDFPKG}) # pkg search path
	include(FindPkgConfig)
	pkg_check_modules(NETCDF netcdf)
	if(NETCDF_FOUND)
		set(NETCDF_INCLUDE_DIR ${NETCDF_INCLUDE_DIRS})
		find_package_handle_standard_args(NETCDF DEFAULT_MSG NETCDF_LIBRARIES NETCDF_INCLUDE_DIR)
	endif(NETCDF_FOUND)
else(NETCDFPKG) # no netcdf.pc file
	find_library(NETCDF_LIBRARIES NAMES netcdf)
	find_path(NETCDF_INCLUDE_DIR NAMES netcdfcpp.h)	
	find_package_handle_standard_args(NETCDF DEFAULT_MSG NETCDF_LIBRARIES NETCDF_INCLUDE_DIR)
endif(NETCDFPKG)

mark_as_advanced(
NETCDFPKG
NETCDF
NETCDF_INCLUDE_DIR
NETCDF_LIBRARIES
)
