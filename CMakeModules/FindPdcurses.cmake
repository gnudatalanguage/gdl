#
# 2014/Jun/25 Jeongbin Park added this file; to support Windows platform.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#

find_path(PDCURSES_INCLUDE_DIR curses.h)
find_library(PDCURSES_LIBRARY pdcurses)

set(PDCURSES_LIBRARIES ${PDCURSES_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PDCurses DEFAULT_MSG PDCURSES_LIBRARY PDCURSES_LIBRARIES PDCURSES_INCLUDE_DIR)

mark_as_advanced(PDCURSES_LIBRARY PDCURSES_LIBRARIES PDCURSES_INCLUDE_DIR)
