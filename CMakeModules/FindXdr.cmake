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

find_library(XDR_LIBRARY NAMES xdr)
set(XDR_LIBRARIES ${XDR_LIBRARY})
find_path(XDR_INCLUDE_DIR NAMES rpc/xdr.h)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(XDR DEFAULT_MSG XDR_LIBRARIES XDR_INCLUDE_DIR)

mark_as_advanced(
XDR_LIBRARY
XDR_LIBRARIES
XDR_INCLUDE_DIR 
)
