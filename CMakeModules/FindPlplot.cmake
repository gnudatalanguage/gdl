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

# the following code is justify to manage plplot
# on OSX 10.11 where the current versions of PLplot
# packaged in Brew and Fink may be not OK with the CVS version of GDL
# Then, when PLPLOTDIR is provide, it must be strickly enforce,
# not looking at the lib. provided by Brew or Fink
# 
if(PLPLOTDIR)
  find_library(NEW_PLPLOT_LIBRARY NAMES plplot NO_DEFAULT_PATH)
  find_library(OLD_PLPLOT_LIBRARY NAMES plplotd NO_DEFAULT_PATH)
else()
  find_library(NEW_PLPLOT_LIBRARY NAMES plplot)
  find_library(OLD_PLPLOT_LIBRARY NAMES plplotd)
endif()

if (NEW_PLPLOT_LIBRARY AND OLD_PLPLOT_LIBRARY) 
    message(WARNING "Two incompatible plplot libraries are installed, probable trouble at compile time ahead.")
endif (NEW_PLPLOT_LIBRARY AND OLD_PLPLOT_LIBRARY)

# the logic here is to use first "plplot" version, if not then "plplotd" one
find_library(PLPLOT_LIBRARY NAMES plplot)
find_library(PLPLOTCXX_LIBRARY NAMES plplotcxx)
if (NOT PLPLOT_LIBRARY)
    find_library(PLPLOT_LIBRARY NAMES plplotd)
    find_library(PLPLOTCXX_LIBRARY NAMES plplotcxxd)
endif(NOT PLPLOT_LIBRARY)

# normal situation : take care that PLPLOT_LIBRARIES and PLPLOT_INCLUDE_DIR prefix
# must be the same !!

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
