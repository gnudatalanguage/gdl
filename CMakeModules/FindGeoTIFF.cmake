# FindGeoTIFF
# -----------
# Find the GeoTIFF library (libgeotiff)

find_path(GEOTIFF_INCLUDE_DIR geotiff.h PATH_SUFFIXES geotiff)
find_library(GEOTIFF_LIBRARY NAMES geotiff libgeotiff)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GeoTIFF DEFAULT_MSG GEOTIFF_INCLUDE_DIR GEOTIFF_LIBRARY)

if(GEOTIFF_FOUND)
    set(GEOTIFF_LIBRARIES ${GEOTIFF_LIBRARY})
    set(GEOTIFF_INCLUDE_DIRS ${GEOTIFF_INCLUDE_DIR})
endif(GEOTIFF_FOUND)

mark_as_advanced(
GEOTIFF_INCLUDE_DIR
GEOTIFF_LIBRARY
GEOTIFF_LIBRARIES
GEOTIFF_INCLUDE_DIRS
)

