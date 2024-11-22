# It is a fatal error if no working C compiler is available to build
# the PLplot core C library and core C examples.  All other compilers
# required by our bindings are optional in that if no working compiler
# of the kind needed is available, the associated bindings and
# examples are disabled.
enable_language(C)
message(STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_COMMAND = ${CMAKE_COMMAND}")
message(STATUS "CMAKE_VERSION = ${CMAKE_VERSION}")
message(STATUS "CMAKE_GENERATOR = ${CMAKE_GENERATOR}")
message(STATUS "CMAKE_INCLUDE_PATH = ${CMAKE_INCLUDE_PATH}")
message(STATUS "Environment variable CMAKE_INCLUDE_PATH = $ENV{CMAKE_INCLUDE_PATH}")
message(STATUS "CMAKE_LIBRARY_PATH = ${CMAKE_LIBRARY_PATH}")
message(STATUS "Environment variable CMAKE_LIBRARY_PATH = $ENV{CMAKE_LIBRARY_PATH}")
# Location of configured language support files.
message(STATUS "CMAKE_PLATFORM_INFO_DIR = ${CMAKE_PLATFORM_INFO_DIR}")

# Locations where the gdl build system first looks for cmake modules.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/src/plplot/modules)

#Version data that need modification for each release.
include(plplot_version)
# parameters to control overall cmake behaviour.
# Configure PLplot component variables....
include(plplot)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/plplot_config.h.in
  ${CMAKE_CURRENT_SOURCE_DIR}/plplot_config.h
  )
# Allow access to the generated plplot_config.h for this build.
add_definitions("-DPLPLOT_HAVE_CONFIG_H")
add_subdirectory(fonts) # does not do anything until option BUILD_HERSHEY_FONTS is set to ON
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(data)
summary()
