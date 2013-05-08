set(BASE_SOURCE ${CMAKE_SOURCE_DIR})
set(BASE_BINARY ${CMAKE_BINARY_DIR})
# write the test launcher
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
  } else {
    exit(WEXITSTATUS(child_status));
  }
}
}
")
file(WRITE ${CMAKE_SOURCE_DIR}/testsuite/launchtest.c "${LAUNCH}")
add_executable(launchtest EXCLUDE_FROM_ALL ${CMAKE_SOURCE_DIR}/testsuite/launchtest.c)
add_library(test_ce SHARED EXCLUDE_FROM_ALL libtest_ce.cpp)
add_custom_target(check COMMAND ${CMAKE_BUILD_TOOL} test DEPENDS launchtest test_ce)
get_target_property(LAUNCHTESTLOCATION launchtest LOCATION)
execute_process(
  COMMAND "fgrep" ".pro" "${BASE_SOURCE}/testsuite/Makefile.am" 
  COMMAND "awk" "{printf(\"%s;\",$1)}" 
  OUTPUT_VARIABLE TESTS)
foreach(TEST ${TESTS})
	add_test(${TEST} ${LAUNCHTESTLOCATION} ${TEST})
endforeach(TEST TESTS)
