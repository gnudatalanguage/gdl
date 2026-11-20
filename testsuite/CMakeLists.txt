include(CTest)
set(CTEST_USE_LAUNCHERS 1)

set(BASE_SOURCE ${CMAKE_SOURCE_DIR})
set(BASE_BINARY ${CMAKE_BINARY_DIR})

if (GDLDEV)
  file(GLOB TESTLIST "${BASE_SOURCE}/testsuite/test_*.pro")
else()
  file(READ "${BASE_SOURCE}/testsuite/LIST" TESTLIST)
endif()

add_library(test_ce SHARED EXCLUDE_FROM_ALL libtest_ce.cpp)
add_test(NAME test_build_libtest_ce COMMAND "${CMAKE_COMMAND}"
  --build "${CMAKE_BINARY_DIR}"
  --config "$<CONFIG>"
  --target test_ce
)
set_tests_properties(test_build_libtest_ce PROPERTIES FIXTURES_SETUP call_external_library)
file(COPY ${CMAKE_SOURCE_DIR}/testsuite/example.grib DESTINATION ${CMAKE_BINARY_DIR}/testsuite/)

set(GDL_BINARY "${GDL_BUILD_LAUNCHER}")
set(PATH_SEP ":")
if(WIN32 AND NOT CYGWIN)
  # The build-tree launcher is a POSIX shell script (MSYS/MinGW builds).
  find_program(GDL_TEST_SHELL NAMES sh bash)
  if(NOT GDL_TEST_SHELL)
    message(FATAL_ERROR "Build-tree tests require a POSIX shell (e.g. MSYS sh).")
  endif()
  set(GDL_BINARY "${GDL_TEST_SHELL}" "${GDL_BUILD_LAUNCHER}")
  set(PATH_SEP "\\;")
endif()

string(REGEX MATCHALL "test[_a-zA-Z0-9]+\\.pro" TESTS ${TESTLIST})
foreach(TEST ${TESTS})
  string(REGEX REPLACE "\\.pro$" "" TESTNAME ${TEST})
  add_test(NAME ${TEST} COMMAND ${GDL_BINARY} -quiet -e "if execute('${TESTNAME}') ne 1 then exit, status=1")
  if(TESTNAME STREQUAL "test_call_external")
    set_tests_properties(${TEST} PROPERTIES
      FIXTURES_REQUIRED call_external_library
      WORKING_DIRECTORY "$<TARGET_FILE_DIR:test_ce>")
  endif()
endforeach(TEST TESTS)

set_tests_properties(${TESTS} PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 120) # autoconf's setting
set_tests_properties(${TESTS} PROPERTIES ENVIRONMENT "LC_COLLATE=C;GDL_PATH=${BASE_SOURCE}/testsuite/${PATH_SEP}${BASE_SOURCE}/src/pro/${PATH_SEP}${BASE_SOURCE}/src/pro/CMprocedures/${PATH_SEP}${BASE_SOURCE}/src/pro/dicom/${PATH_SEP}${BASE_SOURCE}/src/pro/utilities/;GDL_STARTUP=;IDL_STARTUP=")

# These isolated build-system tests need no optional GDL libraries. Their C
# executable fixture uses POSIX APIs, so register them on POSIX hosts only.
if(UNIX)
  find_package(Python3 QUIET COMPONENTS Interpreter)
  find_program(GDL_TEST_NINJA NAMES ninja ninja-build)
  if(Python3_Interpreter_FOUND AND GDL_TEST_NINJA)
    add_test(NAME test_build_helpers
      COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/test_build_helpers.py")
    set_tests_properties(test_build_helpers PROPERTIES TIMEOUT 180 LABELS build
      ENVIRONMENT "CMAKE_COMMAND=${CMAKE_COMMAND};CTEST_COMMAND=${CMAKE_CTEST_COMMAND}")
  endif()
endif()

## set(ARM_XFAIL_TESTS
## #    test_byte_conversion.pro
## #    test_formats.pro
## )
## foreach(test ${ARM_XFAIL_TESTS})
##   set_tests_properties(${test} PROPERTIES WILL_FAIL $<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},aarch64>)
## endforeach()
## 
