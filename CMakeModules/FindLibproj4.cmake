#
# copyright : (c) 2010 Maxime Lenoir, Alain Coulais,
#                      Sylwester Arabas and Orion Poplawski
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#


find_library(LIBPROJ4_LIBRARIES NAMES proj)
#find_path(LIBPROJ4_INCLUDE_DIR NAMES lib_proj.h)
find_path(LIBPROJ4_INCLUDE_DIR NAMES proj_api.h)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBPROJ4 DEFAULT_MSG LIBPROJ4_LIBRARIES LIBPROJ4_INCLUDE_DIR)

mark_as_advanced(
LIBPROJ4_LIBRARIES
LIBPROJ4_INCLUDE_DIR
)
