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
find_path(GSL_INCLUDE_DIR NAMES gsl/gsl_math.h)

# Keep the direct-CMake default; build_gdl.sh opts into AUTO. An explicit
# GSLCBLAS_LIBRARY (including an existing cache entry) always takes priority.
set(GSL_CBLAS_PROVIDER "GSL" CACHE STRING "GSL CBLAS provider: AUTO, FLEXIBLAS, ACCELERATE, OPENBLAS, or GSL")
set_property(CACHE GSL_CBLAS_PROVIDER PROPERTY STRINGS AUTO FLEXIBLAS ACCELERATE OPENBLAS GSL)
set(GSL_CBLAS_ROOTS "" CACHE STRING "Additional search prefixes for GSL CBLAS providers")
string(TOUPPER "${GSL_CBLAS_PROVIDER}" _gsl_cblas_provider)
if(NOT _gsl_cblas_provider MATCHES "^(AUTO|FLEXIBLAS|ACCELERATE|OPENBLAS|GSL)$")
    message(FATAL_ERROR "Invalid GSL_CBLAS_PROVIDER: ${GSL_CBLAS_PROVIDER}")
endif()

include(CheckCSourceCompiles)
include(CheckCSourceRuns)
include(CMakePushCheckState)
find_package(PkgConfig QUIET)

function(gdl_find_cblas provider result)
    string(TOLOWER "${provider}" name)
    set(${result} "" PARENT_SCOPE)
    if(provider STREQUAL "ACCELERATE")
        if(NOT APPLE)
            return()
        endif()
        find_library(GDL_ACCELERATE_LIBRARY NAMES Accelerate)
        set(GDL_ACCELERATE_INCLUDE_DIR "${GSL_INCLUDE_DIR}")
        # Prebuilt GSL calls the legacy, LP64 CBLAS symbols. CBLAS avoids the
        # legacy Fortran float/complex return ABI differences. Defining
        # ACCELERATE_NEW_LAPACK here would not redirect calls inside libgsl.
        set(_gsl_cblas_headers "#include <gsl/gsl_cblas.h>")
    else()
        if(PKG_CONFIG_FOUND)
            pkg_check_modules(PC_GDL_${provider} QUIET ${name})
        endif()
        # Explicit roots come first: Homebrew's pkg-config files name versioned
        # Cellar directories, which disappear when the formula is upgraded.
        find_library(GDL_${provider}_LIBRARY NAMES ${name}
            HINTS ${GSL_CBLAS_ROOTS} ${PC_GDL_${provider}_LIBRARY_DIRS}
            PATH_SUFFIXES lib lib64)
        find_path(GDL_${provider}_INCLUDE_DIR NAMES ${name}_config.h
            HINTS ${GSL_CBLAS_ROOTS} ${PC_GDL_${provider}_INCLUDE_DIRS}
            PATH_SUFFIXES include include/${name} ${name} include/openblas-pthread include/openblas-openmp)
        if(provider STREQUAL "FLEXIBLAS")
            set(_gsl_cblas_headers "
                #include <flexiblas_config.h>
                #ifdef FLEXIBLAS_INTEGER8
                #error GSL requires the LP64 CBLAS interface
                #endif
                #include <cblas.h>")
        else()
            set(_gsl_cblas_headers "
                #include <cblas.h>
                typedef char gdl_blas_int_must_match_int[sizeof(blasint) == sizeof(int) ? 1 : -1];")
        endif()
    endif()
    if(GDL_${provider}_LIBRARY AND GDL_${provider}_INCLUDE_DIR)
        # GSL's CBLAS ABI uses int, so reject
        # ILP64 providers, as well as builds without the real/complex CBLAS API.
        # Use provider headers only in this probe; GDL continues to use GSL's.
        cmake_push_check_state(RESET)
        set(CMAKE_REQUIRED_INCLUDES "${GDL_${provider}_INCLUDE_DIR}")
        set(CMAKE_REQUIRED_LIBRARIES "${GDL_${provider}_LIBRARY}")
        # Recheck if a cached library/header was replaced or the toolchain changed.
        unset(GDL_${provider}_CBLAS_WORKS CACHE)
        check_c_source_compiles("
            ${_gsl_cblas_headers}
            int main(void) {
                double a = 2, b = 3, c = 0, z[2] = {1, 0}, zd[2];
                float x[2] = {1, 0}, cd[2];
                cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                            1, 1, 1, 1, &a, 1, &b, 1, 0, &c, 1);
                cblas_cdotc_sub(1, x, 1, x, 1, cd);
                cblas_zdotc_sub(1, z, 1, z, 1, zd);
                return c != 6 || cblas_sdot(1, x, 1, x, 1) != 1 || cd[0] != 1 || zd[0] != 1;
            }" GDL_${provider}_CBLAS_WORKS)
        cmake_pop_check_state()
        if(GDL_${provider}_CBLAS_WORKS)
            set(${result} "${GDL_${provider}_LIBRARY}" PARENT_SCOPE)
        else()
            message(STATUS "GSL: ${provider} does not provide a compatible LP64 CBLAS interface")
        endif()
    endif()
    mark_as_advanced(GDL_${provider}_LIBRARY GDL_${provider}_INCLUDE_DIR)
endfunction()

# A custom path must still export the CBLAS interface used by GSL. For OpenBLAS,
# interrogate the actual library rather than trusting an unrelated LP64 header.
# Other custom implementations remain the caller's responsibility for ABI size.
function(gdl_check_custom_cblas)
    if(NOT GSL_INCLUDE_DIR)
        return()
    endif()
    cmake_push_check_state(RESET)
    set(CMAKE_REQUIRED_INCLUDES "${GSL_INCLUDE_DIR}")
    set(CMAKE_REQUIRED_LIBRARIES "${GSLCBLAS_LIBRARY}")
    unset(GDL_CUSTOM_CBLAS_WORKS CACHE)
    check_c_source_compiles("
        #include <gsl/gsl_cblas.h>
        int main(void) {
            double a = 2, b = 3, c = 0, z[2] = {1, 0}, zd[2];
            float x[2] = {1, 0}, cd[2];
            cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                        1, 1, 1, 1, &a, 1, &b, 1, 0, &c, 1);
            cblas_cdotc_sub(1, x, 1, x, 1, cd);
            cblas_zdotc_sub(1, z, 1, z, 1, zd);
            return c != 6 || cblas_sdot(1, x, 1, x, 1) != 1 || cd[0] != 1 || zd[0] != 1;
        }" GDL_CUSTOM_CBLAS_WORKS)
    if(NOT GDL_CUSTOM_CBLAS_WORKS)
        message(FATAL_ERROR "GSLCBLAS_LIBRARY does not export the CBLAS interface required by GSL")
    endif()
    unset(GDL_CUSTOM_OPENBLAS CACHE)
    check_c_source_compiles("
        extern char *openblas_get_config(void);
        int main(void) { return openblas_get_config() == 0; }
        " GDL_CUSTOM_OPENBLAS)
    if(GDL_CUSTOM_OPENBLAS)
        if(CMAKE_CROSSCOMPILING AND NOT CMAKE_CROSSCOMPILING_EMULATOR)
            message(WARNING "Cannot check the custom OpenBLAS integer ABI while cross-compiling without an emulator. GSLCBLAS_LIBRARY must use LP64.")
        else()
            unset(GDL_CUSTOM_OPENBLAS_LP64 CACHE)
            check_c_source_runs("
                #include <string.h>
                extern char *openblas_get_config(void);
                int main(void) {
                    const char *config = openblas_get_config();
                    return !config || strstr(config, \"USE64BITINT\") != 0;
                }" GDL_CUSTOM_OPENBLAS_LP64)
            if(NOT GDL_CUSTOM_OPENBLAS_LP64)
                message(FATAL_ERROR "GSLCBLAS_LIBRARY is not a runnable LP64 OpenBLAS library; GSL cannot use ILP64 CBLAS")
            endif()
        endif()
    endif()
    cmake_pop_check_state()
endfunction()

set(GSL_CBLAS_SELECTED_PROVIDER "CUSTOM (GSLCBLAS_LIBRARY override)")
if(GSLCBLAS_LIBRARY)
    gdl_check_custom_cblas()
else()
    if(_gsl_cblas_provider STREQUAL "AUTO")
        if(WIN32 OR CYGWIN)
            # GSL DLLs bind to their own libgslcblas; another library linked
            # into GDL cannot redirect those calls.
            set(_gsl_cblas_candidates GSL)
        else()
            set(_gsl_cblas_candidates FLEXIBLAS)
            if(APPLE)
                list(APPEND _gsl_cblas_candidates ACCELERATE)
            endif()
            list(APPEND _gsl_cblas_candidates OPENBLAS GSL)
        endif()
    else()
        set(_gsl_cblas_candidates ${_gsl_cblas_provider})
    endif()
    foreach(_gsl_candidate IN LISTS _gsl_cblas_candidates)
        if(_gsl_candidate STREQUAL "GSL")
            find_library(GDL_GSLCBLAS_LIBRARY NAMES gslcblas)
            set(GSLCBLAS_LIBRARY "${GDL_GSLCBLAS_LIBRARY}")
            mark_as_advanced(GDL_GSLCBLAS_LIBRARY)
        else()
            gdl_find_cblas(${_gsl_candidate} GSLCBLAS_LIBRARY)
        endif()
        if(GSLCBLAS_LIBRARY)
            set(GSL_CBLAS_SELECTED_PROVIDER "${_gsl_candidate}")
            break()
        endif()
    endforeach()
    if(NOT GSLCBLAS_LIBRARY AND NOT _gsl_cblas_provider STREQUAL "AUTO")
        message(FATAL_ERROR "Requested GSL CBLAS provider ${GSL_CBLAS_PROVIDER} was not found or is incompatible")
    endif()
endif()
if(GSLCBLAS_LIBRARY)
    # Older caches and explicit paths are labelled CUSTOM even when they point
    # at GSL's own CBLAS. Those do not attempt to replace the DLL's bindings.
    get_filename_component(_gsl_cblas_filename "${GSLCBLAS_LIBRARY}" NAME)
    string(TOLOWER "${_gsl_cblas_filename}" _gsl_cblas_filename)
    if((WIN32 OR CYGWIN) AND NOT GSL_CBLAS_SELECTED_PROVIDER STREQUAL "GSL"
       AND NOT _gsl_cblas_filename MATCHES "^(lib)?gslcblas(\\.dll\\.a|\\.a|\\.lib|\\.so(\\.[0-9]+)*|\\.dylib|(-[0-9]+)?\\.dll)?$")
        message(WARNING "Selecting ${GSL_CBLAS_SELECTED_PROVIDER} cannot change the CBLAS bindings of a GSL DLL already linked to libgslcblas.")
    endif()
    message(STATUS "GSL CBLAS library: ${GSLCBLAS_LIBRARY}")
endif()
# Do not cache an automatically selected GSLCBLAS_LIBRARY: changing the
# provider on a subsequent configure must run selection again.
set(GSL_LIBRARIES ${GSL_LIBRARY} ${GSLCBLAS_LIBRARY})
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GSL DEFAULT_MSG GSL_LIBRARY GSLCBLAS_LIBRARY GSL_INCLUDE_DIR)

mark_as_advanced(
GSL_LIBRARY
GSLCBLAS_LIBRARY
GSL_LIBRARIES
GSL_INCLUDE_DIR
)
