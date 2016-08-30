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

# the following code is justify to manage PLplot
# on OSX 10.11 where the current versions of PLplot (30 August 2016)
# packaged in Brew and Fink may be not OK with the CVS version of GDL
# Then, when PLPLOTDIR is provide, it must be *strickly enforce*,
# not looking at the lib. provided by Brew or Fink
# 
#message(" ${PLPLOTDIR}")
#  find_library(NEW_PLPLOT_LIBRARY NAMES plplot PATHS ${PLPLOTDIR} NO_DEFAULT_PATH)
#  find_library(OLD_PLPLOT_LIBRARY NAMES plplotd PATHS ${PLPLOTDIR} NO_DEFAULT_PATH)
#message("new ${NEW_PLPLOT_LIBRARY}")
#message("old ${OLD_PLPLOT_LIBRARY}")

if(PLPLOTDIR)
  message("Strickly enforce PLPLOTDIR=${PLPLOTDIR}")
  find_library(NEW_PLPLOT_LIBRARY NAMES plplot PATHS ${PLPLOTDIR} NO_DEFAULT_PATH)
  if(NEW_PLPLOT_LIBRARY)
      find_library(PLPLOT_LIBRARY NAMES plplot PATHS ${PLPLOTDIR} NO_DEFAULT_PATH)
      find_library(PLPLOTCXX_LIBRARY NAMES plplotcxx PATHS ${PLPLOTDIR} NO_DEFAULT_PATH)
  endif(NEW_PLPLOT_LIBRARY)
  #
  find_library(OLD_PLPLOT_LIBRARY NAMES plplotd PATHS ${PLPLOTDIR} NO_DEFAULT_PATH)
  if(OLD_PLPLOT_LIBRARY)
      find_library(PLPLOT_LIBRARY NAMES plplotd PATHS ${PLPLOTDIR} NO_DEFAULT_PATH)
      find_library(PLPLOTCXX_LIBRARY NAMES plplotcxxd PATHS ${PLPLOTDIR} NO_DEFAULT_PATH)
  endif(OLD_PLPLOT_LIBRARY)
else(PLPLOTDIR)
  message("Use fuzzy detection for PLplot lib. (e.g. in /usr/lib)")
  find_library(NEW_PLPLOT_LIBRARY NAMES plplot)
  if(NEW_PLPLOT_LIBRARY)
      find_library(PLPLOT_LIBRARY NAMES plplot)
      find_library(PLPLOTCXX_LIBRARY NAMES plplotcxx)
  endif(NEW_PLPLOT_LIBRARY)
  #
  find_library(OLD_PLPLOT_LIBRARY NAMES plplotd)
  if(OLD_PLPLOT_LIBRARY)
      find_library(PLPLOT_LIBRARY NAMES plplotd)
      find_library(PLPLOTCXX_LIBRARY NAMES plplotcxxd)
  endif(OLD_PLPLOT_LIBRARY)
endif(PLPLOTDIR)

#message("new ${NEW_PLPLOT_LIBRARY}")
#message("old ${OLD_PLPLOT_LIBRARY}")


if (NEW_PLPLOT_LIBRARY AND OLD_PLPLOT_LIBRARY) 
    message(FATAL "Two incompatible plplot libraries are installed, probable trouble at compile time ahead.")
endif (NEW_PLPLOT_LIBRARY AND OLD_PLPLOT_LIBRARY)

#      find_library(PLPLOT_LIBRARY NAMES plplot NO_DEFAULT_PATH)
#      find_library(PLPLOTCXX_LIBRARY NAMES plplotcxx NO_DEFAULT_PATH)
#message(" ${PLPLOT_LIBRARY}")

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
