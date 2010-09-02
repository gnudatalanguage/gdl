find_library(FFTW_LIBRARY NAMES fftw3)
find_library(FFTWF_LIBRARY NAMES fftw3f)
set(FFTW_LIBRARIES ${FFTW_LIBRARY} ${FFTWF_LIBRARY})
find_path(FFTW_INCLUDE_DIR NAMES fftw3.h)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FFTW DEFAULT_MSG FFTW_LIBRARIES FFTW_INCLUDE_DIR)
mark_as_advanced(
FFTW_LIBRARY
FFTWF_LIBRARY
FFTW_LIBRARIES
FFTW_INCLUDE_DIR
)
