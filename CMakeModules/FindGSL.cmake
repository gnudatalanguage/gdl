#
# copyright : (c) 2010 Maxime Lenoir, Alain Coulais,
#                      Sylwester Arabas and Orion Poplawski
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#


find_library(GSL_LIBRARY NAMES gsl)
find_library(GSLCBLAS_LIBRARY NAMES gslcblas)
set(GSL_LIBRARIES ${GSL_LIBRARY} ${GSLCBLAS_LIBRARY})
find_path(GSL_INCLUDE_DIR NAMES gsl/gsl_math.h)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GSL DEFAULT_MSG GSL_LIBRARIES GSL_INCLUDE_DIR)

mark_as_advanced(
GSL_LIBRARY
GSLCBLAS_LIBRARY
GSL_LIBRARIES
GSL_INCLUDE_DIR
)
