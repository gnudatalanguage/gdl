find_path(LIBPSPKG libps.pc PATH_SUFFIXES lib lib/pkgconfig)
include(FindPackageHandleStandardArgs)
if(LIBPSPKG)
	set(ENV{PKG_CONFIG_PATH} ${LIBPSPKG}) # pkg search path
	include(FindPkgConfig)
	pkg_check_modules(LIBPS libps)
	if(LIBPS_FOUND)
		find_package_handle_standard_args(LIBPS DEFAULT_MSG LIBPS_LIBRARIES)
	endif(LIBPS_FOUND)
else(LIBPSPKG) # no netcdf.pc file
	find_library(LIBPS_LIBRARIES NAMES libps)
	find_path(LIBPS_INCLUDE_DIRS NAMES pslib.h)	
	find_package_handle_standard_args(LIBPS DEFAULT_MSG LIBPS_LIBRARIES LIBPS_INCLUDE_DIRS)
endif(LIBPSPKG)

mark_as_advanced(
LIBPSPKG
LIBPS
LIBPS_INCLUDE_DIRS
LIBPS_LIBRARIES
)
