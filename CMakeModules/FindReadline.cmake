#
# copyright : (c) 2010 Maxime Lenoir, Alain Coulais,
#                      Sylwester Arabas and Orion Poplawski
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#


find_library(READLINE_LIBRARY NAMES readline)
find_library(HISTORY_LIBRARY NAMES history)
set(READLINE_LIBRARIES ${READLINE_LIBRARY} ${HISTORY_LIBRARY})
find_path(READLINE_INCLUDE_DIR NAMES readline/readline.h)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Readline DEFAULT_MSG READLINE_LIBRARIES READLINE_INCLUDE_DIR)

mark_as_advanced(
READLINE_LIBRARY
HISTORY_LIBRARY
READLINE_LIBRARIES
READLINE_INCLUDE_DIR
)
