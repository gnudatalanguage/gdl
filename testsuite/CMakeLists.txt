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
  test_bug_1779553.pro 
  test_bug_2555865.pro 
  test_bug_2610174.pro 
  test_bug_2846561.pro 
  test_bug_2876150.pro 
  test_bug_2876161.pro 
  test_bug_2974380.pro 
  test_bug_3033108.pro 
  test_bug_3054361.pro 
  test_bug_3055720.pro 
  test_bug_3057511.pro 
  test_bug_3057520.pro 
  test_bug_3061072.pro 
  test_bug_3081887.pro 
  test_bug_3085858.pro 
  test_bug_3086851.pro 
  test_bug_3091599.pro 
  test_bug_3091610.pro 
  test_bug_3100945.pro 
  test_bug_3104209.pro 
  test_bug_3104214.pro 
  test_bug_3104326.pro 
  test_bug_3104349.pro 
  test_bug_3147146.pro 
  test_bug_3147181.pro 
  test_bug_3151760.pro 
  test_ce.pro 
  test_clip.pro 
  test_common.pro 
  test_constants.pro 
  test_deriv.pro 
  test_device.pro 
  test_dicom.pro 
  test_erfinv.pro 
  test_execute.pro 
  test_fft.pro 
  test_fft_leak.pro 
  test_file_basename.pro 
  test_file_copy.pro 
  test_file_delete.pro 
  test_file_dirname.pro
  test_file_search.pro 
  test_fix.pro 
  test_fixprint.pro 
  test_gc.pro 
  test_get_screen_size.pro 
  test_grib.pro 
  test_hist_2d.pro 
  test_idl8.pro 
  test_interpol.pro 
  test_memory.pro 
  test_moment.pro 
  test_mpfit.pro 
  test_multiroots.pro 
  test_nans_in_sort_and_median.pro 
  test_nestedloop.pro 
  test_plotting_ranges.pro 
  test_pmulti.pro 
  test_product.pro 
  test_ps_decomposed.pro 
  test_python.pro 
  test_python_module_0.pro 
  test_python_module_1.pro 
  test_python_module_2.pro 
  test_readf.pro 
  test_rebin.pro 
  test_rk4.pro 
  test_spher_harm.pro 
  test_spl_init.pro 
  test_step.pro 
  test_str_functions.pro 
  test_strsplit.pro
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
