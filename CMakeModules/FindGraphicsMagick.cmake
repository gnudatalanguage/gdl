#
# copyright : (c) 2013 Sacha Hony
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
# http://sourceforge.net/tracker/?func=detail&aid=3611651&group_id=97659&atid=618685
#
# Find the GraphicsMagick includes and library
# 

find_library(GRAPHICSMAGICK_LIBRARY NAMES GraphicsMagick)
find_library(GRAPHICSMAGICKXX_LIBRARY NAMES GraphicsMagick++)
set(GRAPHICSMAGICK_LIBRARIES ${GRAPHICSMAGICK_LIBRARY} ${GRAPHICSMAGICKXX_LIBRARY})
find_path(GRAPHICSMAGICK_INCLUDE_DIR NAMES magick/api.h PATH_SUFFIXES GraphicsMagick)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GRAPHICSMAGICK DEFAULT_MSG GRAPHICSMAGICK_LIBRARIES GRAPHICSMAGICK_INCLUDE_DIR)

mark_as_advanced(
GRAPHICSMAGICK_LIBRARIES
GRAPHICSMAGICK_INCLUDE_DIR
)
