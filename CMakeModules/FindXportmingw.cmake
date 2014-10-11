#
# copyright : (c) 2014 Jeongbin Park
#
# 2014/Sep/24 Jeongbin Park added this file; to support Windows platform.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#

find_library(XPORTMINGWX11_LIBRARY NAMES x11)
find_library(XPORTMINGWXAU_LIBRARY NAMES xau)
set(XPORTMINGW_LIBRARIES ${XPORTMINGWX11_LIBRARY} ${XPORTMINGWXAU_LIBRARY})
find_path(XPORTMINGW_INCLUDE_DIR NAMES X11/X.h)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(XPORTMINGW DEFAULT_MSG XPORTMINGW_LIBRARIES XPORTMINGW_INCLUDE_DIR)

mark_as_advanced(
XPORTMINGW_LIBRARY
XPORTMINGW_LIBRARIES
XPORTMINGW_INCLUDE_DIR 
)
