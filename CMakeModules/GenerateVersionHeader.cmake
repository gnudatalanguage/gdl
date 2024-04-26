# based on https://github.com/nocnokneo/cmake-git-versioning-example/blob/master/CMakeLists.txt

execute_process(
  COMMAND ${GIT_EXECUTABLE} describe --tags --dirty --always --match "v*" 
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE VERSION
  RESULT_VARIABLE GIT_DESCRIBE_ERROR_CODE
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT "${GIT_DESCRIBE_ERROR_CODE}" EQUAL "0") 
  message(FATAL_ERROR
    "Failed to obtain version data from git. "
    "Please use GDL tarballs which contain version.h header (release or weekly from https://github.com/gnudatalanguage/gdl/releases). "
    "Alternatively, clone the repo using git to be able to generate version.h from git metadata")
endif()
configure_file(${SRC} ${DST})
