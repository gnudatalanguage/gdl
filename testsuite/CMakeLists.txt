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
set_tests_properties(test_build_libtest_ce PROPERTIES FIXTURES_SETUP test_ce_fixture) # Requires cMake >= 3.7

set(GDL_BINARY ${BASE_BINARY}/src/gdl)
set(PATH_SEP ":")
if(WIN32 AND NOT CYGWIN)
  set(GDL_BINARY ${GDL_BINARY}.exe)
  set(PATH_SEP "\\;")
endif()

string(REGEX MATCHALL "test[_a-zA-Z0-9]+\\.pro" TESTS ${TESTLIST})
foreach(TEST ${TESTS})
  string(REGEX REPLACE "\\.pro$" "" TESTNAME ${TEST})
  add_test(NAME ${TEST} COMMAND ${GDL_BINARY} -quiet -e ${TESTNAME})
  if (${TESTNAME} EQUAL "test_call_external")
    set_tests_properties(${TEST} PROPERTIES FIXTURES_REQUIRED test_ce_fixture)
  endif()
endforeach(TEST TESTS)

set_tests_properties(${TESTS} PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 3600) # autoconf's setting
set_tests_properties(${TESTS} PROPERTIES ENVIRONMENT "LC_COLLATE=C;GDL_PATH=${BASE_SOURCE}/testsuite/${PATH_SEP}${BASE_SOURCE}/src/pro/;GDL_STARTUP=;IDL_STARTUP=")