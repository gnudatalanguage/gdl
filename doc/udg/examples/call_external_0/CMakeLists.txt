project(libexaple C)
cmake_minimum_required(VERSION 2.0)
add_library(example SHARED libexample.c) 
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES 
  "Makefile;CMakeCache.txt;cmake_install.cmake;CMakeFiles")

