cmake_minimum_required(VERSION 3.5)

project(powell)
set(CMAKE_SHARED_LIBRARY_PREFIX "") # prevent to prepend 'lib' to all .so names
############################################################
# Create a library
############################################################

#Generate the shared library from the library sources
add_library(powell SHARED 
    brent.cpp
)

set(dlm_files
  powell.so
  powell.dlm
  )

install(FILES ${dlm_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/dlm)
