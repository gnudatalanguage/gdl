;
; Alain C., 21 March 2013
;
; draft: very preliminary version for testing BYTSCL(),
; the last case is buggy.
;
pro TEST_BYTSCL
;
ramp=FINDGEN(10)
;
expected=BYTARR(10)
expected[*]=[0,28,56,85,113,142,170,199,227,255]
;
resu=BYTSCL(ramp)
;
print, ARRAY_EQUAL(expected, resu)
;
expected_nan=BYTARR(10)
expected_nan[6]=255
;
expected_nan_flag=expected
expected_nan_flag[5:6]=0
;
;
ramp_nan=ramp
ramp_nan[5]=!values.f_nan
ramp_nan[6]=!values.f_infinity
;
resu_nan=BYTSCL(ramp_nan)
resu_nan_flag=BYTSCL(ramp_nan,/nan)
;
print, ARRAY_EQUAL(expected_nan, resu_nan)
print, ARRAY_EQUAL(expected_nan_flag, resu_nan_flag)
;
print, 'not finished'

stop
end
