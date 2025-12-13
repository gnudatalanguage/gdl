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
add_test(test_build_libtest_ce
  "${CMAKE_COMMAND}"
  --build "${CMAKE_BINARY_DIR}"
  --config "$<CONFIG>"
  --target test_ce
)
file(COPY ${CMAKE_SOURCE_DIR}/testsuite/example.grib DESTINATION ${CMAKE_BINARY_DIR}/testsuite/)

set(GDL_BINARY ${BASE_BINARY}/src/gdl)
set(PATH_SEP ":")
if(WIN32 AND NOT CYGWIN)
  set(GDL_BINARY ${GDL_BINARY}.exe)
  set(PATH_SEP "\\;")
endif()

string(REGEX MATCHALL "test[_a-zA-Z0-9]+\\.pro" TESTS ${TESTLIST})
foreach(TEST ${TESTS})
  string(REGEX REPLACE "\\.pro$" "" TESTNAME ${TEST})
  add_test(NAME ${TEST} COMMAND ${GDL_BINARY} -quiet -e "if execute('${TESTNAME}') ne 1 then exit, status=1")
  if (${TESTNAME} EQUAL "test_call_external")
    set_tests_properties(${TEST} PROPERTIES DEPEND test_build_libtest_ce)
  endif()
endforeach(TEST TESTS)

set_tests_properties(${TESTS} PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 3600) # autoconf's setting
set_tests_properties(${TESTS} PROPERTIES ENVIRONMENT "LC_COLLATE=C;GDL_PATH=${BASE_SOURCE}/testsuite/${PATH_SEP}${BASE_SOURCE}/src/pro/${PATH_SEP}${BASE_SOURCE}/src/pro/CMprocedures/${PATH_SEP}${BASE_SOURCE}/src/pro/dicom/${PATH_SEP}${BASE_SOURCE}/src/pro/utilities/;GDL_STARTUP=;IDL_STARTUP=")

set(ARM_XFAIL_TESTS
    test_byte_conversion.pro
    test_formats.pro
    test_rounding.pro
)
foreach(test ${ARM_XFAIL_TESTS})
  set_tests_properties(${test} PROPERTIES WILL_FAIL $<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},aarch64>)
endforeach()
