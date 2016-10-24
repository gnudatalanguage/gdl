#
# copyright : (c) 2016 Alain Coulais
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
# This is a derived work from FindReadline.cmake
#
# On OSX, we do not have the GNU Readline Lib. by default
# but we do have access to a BSB clone, the Editline Lib.
# Unfortunatly, few functions are not visible in Editline Lib. !!

find_library(EDITLINE_LIBRARY NAMES editline)
set(EDITLINE_LIBRARIES ${EDITLINE_LIBRARY})
find_path(EDITLINE_INCLUDE_DIR NAMES editline/readline.h)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Editline DEFAULT_MSG EDITLINE_LIBRARIES EDITLINE_INCLUDE_DIR)

mark_as_advanced(
EDITLINE_LIBRARY
EDITLINE_LIBRARIES
EDITLINE_INCLUDE_DIR
)
