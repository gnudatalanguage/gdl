cmake_minimum_required(VERSION 3.5)
## if (NOT MINGW )
## 	project(MyLittleProject  VERSION 1.0.0 DESCRIPTION "loadable shared library providing command MYCOMMAND")
## 	set(CMAKE_SHARED_LIBRARY_PREFIX "") # prevent to prepend 'lib' to all .so names
## 	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
## 	  set(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -undefined dynamic_lookup")
## 	endif()
## 
## 	add_library(MyLittleProject SHARED 
## 		MyLittleProject.cpp
## 	)
## 	set_target_properties(MyLittleProject PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
## 	SET_TARGET_PROPERTIES(MyLittleProject PROPERTIES SUFFIX .so)
## 	set(dlm_files
## 	  ${PROJECT_BINARY_DIR}/MyLittleProject.so
## 	  MyLittleProject.dlm
## 	  )
## 	add_dependencies(MyLittleProject gdl) # be sure that gdl is built before MyLittleProject
## 
## 	install(FILES ${dlm_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/gnudatalanguage/dlm)
## endif()
