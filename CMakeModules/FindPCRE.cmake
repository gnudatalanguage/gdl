#
# copyright : (c) 2012 Jeongbin Park
#
# 2012/Sep/18 Jeongbin Park added this file; to support Windows platform.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#

find_library(PCRE_LIBRARY NAMES pcre)
set(PCRE_LIBRARIES ${PCRE_LIBRARY})
find_path(PCRE_INCLUDE_DIR NAMES regex.h)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PCRE DEFAULT_MSG PCRE_LIBRARIES PCRE_INCLUDE_DIR)

mark_as_advanced(
PCRE_LIBRARY
PCRE_LIBRARIES
PCRE_INCLUDE_DIR 
)
