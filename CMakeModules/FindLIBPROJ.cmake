find_path(LIBPROJ_INCLUDE_DIR
  NAMES proj_api.h proj.h
  DOC "libproj include directories")
mark_as_advanced(LIBPROJ_INCLUDE_DIR)

find_library(LIBPROJ_LIBRARY_RELEASE
  NAMES proj
  DOC "libproj release library")
mark_as_advanced(LIBPROJ_LIBRARY_RELEASE)

find_library(LIBPROJ_LIBRARY_DEBUG
  NAMES projd
  DOC "libproj debug library")
mark_as_advanced(LIBPROJ_LIBRARY_DEBUG)

include(SelectLibraryConfigurations)
select_library_configurations(LIBPROJ)

if (LIBPROJ_INCLUDE_DIR)
  if (EXISTS "${LIBPROJ_INCLUDE_DIR}/proj.h")
    file(STRINGS "${LIBPROJ_INCLUDE_DIR}/proj.h" _libproj_version_lines REGEX "#define[ \t]+PROJ_VERSION_(MAJOR|MINOR|PATCH)")
    string(REGEX REPLACE ".*PROJ_VERSION_MAJOR *\([0-9]*\).*" "\\1" _libproj_version_major "${_libproj_version_lines}")
    string(REGEX REPLACE ".*PROJ_VERSION_MINOR *\([0-9]*\).*" "\\1" _libproj_version_minor "${_libproj_version_lines}")
    string(REGEX REPLACE ".*PROJ_VERSION_PATCH *\([0-9]*\).*" "\\1" _libproj_version_patch "${_libproj_version_lines}")
  else ()
    file(STRINGS "${LIBPROJ_INCLUDE_DIR}/proj_api.h" _libproj_version_lines REGEX "#define[ \t]+PJ_VERSION")
    string(REGEX REPLACE ".*PJ_VERSION *\([0-9]*\).*" "\\1" _libproj_version "${_libproj_version_lines}")
    math(EXPR _libproj_version_major "${_libproj_version} / 100")
    math(EXPR _libproj_version_minor "(${_libproj_version} % 100) / 10")
    math(EXPR _libproj_version_patch "${_libproj_version} % 10")
  endif ()
  set(LIBPROJ_VERSION "${_libproj_version_major}.${_libproj_version_minor}.${_libproj_version_patch}")
  set(LIBPROJ_MAJOR_VERSION "${_libproj_version_major}")
  unset(_libproj_version_major)
  unset(_libproj_version_minor)
  unset(_libproj_version_patch)
  unset(_libproj_version)
  unset(_libproj_version_lines)
endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBPROJ
  REQUIRED_VARS LIBPROJ_LIBRARY LIBPROJ_INCLUDE_DIR
  VERSION_VAR LIBPROJ_VERSION)

if (LIBPROJ_FOUND)
  set(LIBPROJ_INCLUDE_DIRS "${LIBPROJ_INCLUDE_DIR}")
  set(LIBPROJ_LIBRARIES "${LIBPROJ_LIBRARY}")

  if (NOT TARGET LIBPROJ::LIBPROJ)
    add_library(LIBPROJ::LIBPROJ UNKNOWN IMPORTED)
    set_target_properties(LIBPROJ::LIBPROJ PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${LIBPROJ_INCLUDE_DIR}")
    if (LIBPROJ_LIBRARY_RELEASE)
      set_property(TARGET LIBPROJ::LIBPROJ APPEND PROPERTY
        IMPORTED_CONFIGURATIONS RELEASE)
      set_target_properties(LIBPROJ::LIBPROJ PROPERTIES
        IMPORTED_LOCATION_RELEASE "${LIBPROJ_LIBRARY_RELEASE}")
    endif ()
    if (LIBPROJ_LIBRARY_DEBUG)
      set_property(TARGET LIBPROJ::LIBPROJ APPEND PROPERTY
        IMPORTED_CONFIGURATIONS DEBUG)
      set_target_properties(LIBPROJ::LIBPROJ PROPERTIES
        IMPORTED_LOCATION_DEBUG "${LIBPROJ_LIBRARY_DEBUG}")
    endif ()
  endif ()
endif ()
