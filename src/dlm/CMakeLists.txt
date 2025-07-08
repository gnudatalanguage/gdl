cmake_minimum_required(VERSION 3.5)

project(powell  VERSION 1.0.0 DESCRIPTION "loadable shared library providing command POWELL")
set(CMAKE_SHARED_LIBRARY_PREFIX "") # prevent to prepend 'lib' to all .so names
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -undefined dynamic_lookup")
endif()############################################################
# Create a library
############################################################
#Generate the shared library from the library sources
add_library(powell SHARED 
    brent.cpp
)
set_target_properties(powell PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(dlm_files
  ${PROJECT_BINARY_DIR}/powell.so
  powell.dlm
  )

install(FILES ${dlm_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/gnudatalanguage/dlm)
