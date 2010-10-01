set(BASE_SOURCE ${CMAKE_SOURCE_DIR})
set(BASE_BINARY ${CMAKE_BINARY_DIR})
# write the test launcher
set(LAUNCH
"#include<unistd.h>
#include<stdlib.h>
int main(int c,char**v) {
char*p;
if(c!=2) return 1;
for(p=v[1];*p;++p);
if(p-4<v[1]) return 1;
if(p[-1]!='o'||p[-2]!='r'||p[-3]!='p'||p[-4]!='.') return 1;
*(p-4)=0;
setenv(\"LC_COLLATE\",\"C\",1);
setenv(\"GDL_PATH\",\"+${BASE_SOURCE}/testsuite/:+${BASE_SOURCE}/src/pro/\",1);
execl(\"${BASE_BINARY}/src/gdl\",\"-quiet\",\"-e\",v[1],(char*)0);
}
")
file(WRITE ${CMAKE_SOURCE_DIR}/testsuite/launchtest.c "${LAUNCH}")
add_executable(launchtest ${CMAKE_SOURCE_DIR}/testsuite/launchtest.c)

set(TESTS
test_angles.pro
test_base64.pro
test_binfmt.pro
test_bug_3033108.pro
test_constants.pro
test_device.pro
test_erfinv.pro
test_fft.pro
test_fft_leak.pro
test_file_basename.pro
test_file_copy.pro
test_file_delete.pro
test_file_dirname.pro
test_file_search.pro
test_fix.pro
test_gc.pro
test_get_screen_size.pro
test_hist_2d.pro
test_idl8.pro
test_interpol.pro
test_memory.pro
test_mpfit.pro
test_multiroots.pro
test_nans_in_sort_and_median.pro
test_nestedloop.pro
test_pmulti.pro
test_product.pro
test_readf.pro
test_rebin.pro
test_spher_harm.pro
test_spl_init.pro
test_str_functions.pro
test_suite.pro
test_url.pro
test_wavelet.pro
test_zeropoly.pro
test_zip.pro
test_zzz.pro
)
get_target_property(LAUNCHTESTLOCATION launchtest LOCATION)
foreach(TEST ${TESTS})
	add_test(${TEST} ${LAUNCHTESTLOCATION} ${TEST})
endforeach(TEST TESTS)
