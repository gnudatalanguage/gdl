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
