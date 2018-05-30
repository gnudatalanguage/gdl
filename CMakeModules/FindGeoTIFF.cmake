# FindGeoTIFF
# -----------
# Find the GeoTIFF library (libgeotiff)

find_path(GEOTIFF_INCLUDE_DIR geotiff.h)
find_library(GEOTIFF_LIBRARY NAMES geotiff libgeotiff)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GEOTIFF DEFAULT_MSG GEOTIFF_LIBRARY GEOTIFF_INCLUDE_DIR)

if(GEOTIFF_FOUND)
    set(GEOTIFF_LIBRARIES ${GEOTIFF_LIBRARY})
    set(GEOTIFF_INCLUDE_DIRS ${GEOTIFF_INCLUDE_DIR})
endif(GEOTIFF_FOUND)

mark_as_advanced(
GEOTIFF_INCLUDE_DIR
GEOTIFF_LIBRARY
)
