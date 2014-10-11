set(BASE_SOURCE ${CMAKE_SOURCE_DIR})
set(BASE_BINARY ${CMAKE_BINARY_DIR})
# write the test launcher
if(WIN32 AND NOT CYGWIN)
set(LAUNCH
"#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
int main(int c,char**v) {
char*p;
if(c!=2) return 1;
for(p=v[1];*p;++p);
if(p-4<v[1]) return 1;
if(p[-1]!='o'||p[-2]!='r'||p[-3]!='p'||p[-4]!='.') return 1;
*(p-4)=0;

_putenv(\"LC_COLLATE=C\");
_putenv(\"GDL_PATH=${BASE_SOURCE}/testsuite/;${BASE_SOURCE}/src/pro/\");
_putenv(\"GDL_STARTUP=\");
_putenv(\"IDL_STARTUP=\");

char *tmp = (char *)malloc( sizeof(char) * (strlen(\"${BASE_BINARY}/src/gdl -quiet -e \") + strlen(v[1]) + 1));
//PROCESS_INFORMATION processInformation = {0};
//STARTUPINFO startupInfo                = {0};
//startupInfo.cb                         = sizeof(startupInfo);
sprintf(tmp, \"${BASE_BINARY}/src/gdl.exe -quiet -e %s\", v[1]);
//int rtn = CreateProcess(NULL, tmp, NULL, NULL, TRUE, 0, NULL, NULL, &startupInfo, &processInformation);
int rtn = system(tmp);

if (rtn == 77) {
    printf(\"TEST SKIPPED\");
    exit(0);
} if (rtn == -1 || rtn == 127) {
    printf(\"LAUNCH PROCESS FAILED\");
    exit(1);
} else {
    exit(rtn);
}
}
")
else(WIN32 AND NOT CYGWIN)
set(LAUNCH
"#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include<sched.h>
int main(int c,char**v) {
char*p;
if(c!=2) return 1;
for(p=v[1];*p;++p);
if(p-4<v[1]) return 1;
if(p[-1]!='o'||p[-2]!='r'||p[-3]!='p'||p[-4]!='.') return 1;
*(p-4)=0;
setenv(\"LC_COLLATE\",\"C\",1);
setenv(\"GDL_PATH\",\"+${BASE_SOURCE}/testsuite/:+${BASE_SOURCE}/src/pro/\",1);
unsetenv(\"GDL_STARTUP\");
unsetenv(\"IDL_STARTUP\");
int devnull = open(\"/dev/null\",O_RDONLY);
dup2(devnull, 0);
int child_pid = fork();
if(child_pid == 0) {
  execl(\"${BASE_BINARY}/src/gdl\",\"-quiet\",\"-e\",v[1],(char*)0);
  exit(1);
} else {
  sched_yield();
  int child_status;
  waitpid(child_pid, &child_status, 0);
  if (WEXITSTATUS(child_status) == 77) {
    printf(\"TEST SKIPPED\");
    exit(0);
  } else if (WIFSIGNALED(child_status)) {
    printf(\"TEST EXITED FROM SIGNAL %d\", WTERMSIG(child_status));
    exit(1);
  } else {
    exit(WEXITSTATUS(child_status));
  }
}
}
")
endif(WIN32 AND NOT CYGWIN)
file(WRITE ${CMAKE_SOURCE_DIR}/testsuite/launchtest.c "${LAUNCH}")
add_executable(launchtest EXCLUDE_FROM_ALL ${CMAKE_SOURCE_DIR}/testsuite/launchtest.c)
add_library(test_ce SHARED EXCLUDE_FROM_ALL libtest_ce.cpp)
add_custom_target(check COMMAND ${CMAKE_BUILD_TOOL} test DEPENDS launchtest test_ce gdl)
get_target_property(LAUNCHTESTLOCATION launchtest LOCATION)
if(WIN32 AND NOT CYGWIN)
  execute_process(
    COMMAND "cmd" "/c" "findstr" "\".*pro ${BASE_SOURCE}/testsuite/Makefile.am\""
    RESULT_VARIABLE RESULTS
    OUTPUT_VARIABLE TESTS)
  message(FATAL_ERROR "${RESULTS}")
  message(FATAL_ERROR "${TESTS}")
  #string(REPLACE "\\" "" TESTS ${TESTS})
  #string(REPLACE "\n" ";" TESTS ${TESTS})
else(WIN32 AND NOT CYGWIN)
  execute_process(
    COMMAND "fgrep" ".pro" "${BASE_SOURCE}/testsuite/Makefile.am" 
    COMMAND "awk" "{printf(\"%s;\",$1)}" 
    OUTPUT_VARIABLE TESTS)
endif(WIN32 AND NOT CYGWIN)
foreach(TEST ${TESTS})
	add_test(${TEST} ${LAUNCHTESTLOCATION} ${TEST})
	set_tests_properties(${TEST} PROPERTIES SKIP_RETURN_CODE 77) # autoconf's setting
endforeach(TEST TESTS)
