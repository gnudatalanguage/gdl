;
; Collecting various tests related to NetCDF format
;
; former files: 
; -- nc_test.pro (not called by "make check"
; -- test_bug_3055720.pro
; -- test_bug_3057511.pro
; -- test_bug_3057520.pro
; -- test_bug_3061072.pro
; -- test_bug_3100945.pro
;
; Alain Coulais, 30 August 2013
;
; --------------------------------------------
;
pro TEST_NETCDF
;
if ~NCDF_EXISTS() then EXIT, status=77
;
NC_TEST
TEST_BUG_3055720
TEST_BUG_3057511
TEST_BUG_3057520
TEST_BUG_3061072
TEST_BUG_3100945
;
end
