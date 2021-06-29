include(CTest)
set(CTEST_USE_LAUNCHERS 1)

set(BASE_SOURCE ${CMAKE_SOURCE_DIR})
set(BASE_BINARY ${CMAKE_BINARY_DIR})

if (GDLDEV)
  file(GLOB TESTLIST "${BASE_SOURCE}/testsuite/test_*.pro")
else()
  file(READ "${BASE_SOURCE}/testsuite/LIST" TESTLIST)
endif()

set(GDL_BINARY ${BASE_BINARY}/src/gdl)
if(WIN32 AND NOT CYGWIN)
  set(GDL_BINARY ${GDL_BINARY}.exe)
endif()

string(REGEX MATCHALL "test[_a-zA-Z0-9]+\\.pro" TESTS ${TESTLIST})
foreach(TEST ${TESTS})
  string(REGEX REPLACE "\\.pro$" "" TESTNAME ${TEST})
  add_test(NAME ${TEST} COMMAND ${GDL_BINARY} -quiet -e ${TESTNAME})
endforeach(TEST TESTS)
set_tests_properties(${TESTS} PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 3600) # autoconf's setting
set_tests_properties(${TESTS} PROPERTIES ENVIRONMENT "LC_COLLATE=C;GDL_PATH=${BASE_SOURCE}/testsuite/;${BASE_SOURCE}/src/pro/;GDL_STARTUP=;IDL_STARTUP=")