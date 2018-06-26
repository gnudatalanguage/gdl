find_path(PSLIBPKG libps.pc PATH_SUFFIXES lib lib/pkgconfig lib64/pkgconfig)
include(FindPackageHandleStandardArgs)
if(PSLIBPKG)
	set(ENV{PKG_CONFIG_PATH} ${PSLIBPKG}) # pkg search path
	include(FindPkgConfig)
	pkg_search_module(PSLIB libps)
	if(PSLIB_FOUND)
		set(PSLIB_INCLUDE_DIR ${PSLIB_INCLUDEDIR})
		find_package_handle_standard_args(PSLIB DEFAULT_MSG PSLIB_LIBRARIES PSLIB_INCLUDE_DIR)
	endif(PSLIB_FOUND)
else() # no libps.pc file
	find_library(PSLIB_LIBRARIES NAMES libps pslib ps)
	find_path(PSLIB_INCLUDE_DIR NAMES libps/pslib.h)	
	find_package_handle_standard_args(PSLIB DEFAULT_MSG PSLIB_LIBRARIES PSLIB_INCLUDE_DIR)
endif()

mark_as_advanced(
PSLIBPKG
PSLIB
PSLIB_LIBRARIES
PSLIB_INCLUDE_DIR
)
