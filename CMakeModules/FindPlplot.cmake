#
# copyright : (c) 2010 Maxime Lenoir, Alain Coulais,
#                      Sylwester Arabas and Orion Poplawski
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#test of rare case where old and new libraries are still present: terrible trouble!
find_library(NEW_PLPLOT_LIBRARY NAMES plplot)
find_library(OLD_PLPLOT_LIBRARY NAMES plplotd)
if (NEW_PLPLOT_LIBRARY AND OLD_PLPLOT_LIBRARY) 
  message(WARNING "Two incompatible plplot libraries are installed, probable trouble at compile time ahead.")
endif (NEW_PLPLOT_LIBRARY AND OLD_PLPLOT_LIBRARY)
find_library(PLPLOT_LIBRARY NAMES plplot)
find_library(PLPLOTCXX_LIBRARY NAMES plplotcxx)
if (NOT PLPLOT_LIBRARY)
    find_library(PLPLOT_LIBRARY NAMES plplotd)
    find_library(PLPLOTCXX_LIBRARY NAMES plplotcxxd)
endif(NOT PLPLOT_LIBRARY)
set(PLPLOT_LIBRARIES ${PLPLOT_LIBRARY} ${PLPLOTCXX_LIBRARY})
find_path(PLPLOT_INCLUDE_DIR NAMES plplot/plplot.h)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PLPLOT DEFAULT_MSG PLPLOT_LIBRARIES PLPLOT_INCLUDE_DIR)

mark_as_advanced(
PLPLOT_LIBRARY
PLPLOTCXX_LIBRARY
PLPLOT_LIBRARIES
PLPLOT_INCLUDE_DIR 
)
