;
; J. Wang & A. Coulais, Feb. 2022, under GPL V2 or later
;
;
; Thanks to feedback by Dustem team, we realize that
; we do have a serious bottleneck in several STRING related code.
; The culprit is the internal Convert2() method in GDL
; for the Float, Double, Complex & DComplex types.
;
; We changed that into a classical "sprintf(%#13.6)"  in the code
; The new version is about 3 times faster.
; Then we have to carefully check several kind of formating rules
;
; Please report any missing case !!
;
;
; ----------------------FLOAT------------------------------
;
pro TEST_FLOAT2STR, cumul_errors, verbose=verbose, test=test
;
nb_pbs_float=0
;
mf1 = 3.14159265359
mf2 = 3.141
mf3 = -3141.5926
mf4 = 314159.26
mf5 = -31415926.5359
mf6 = 0.0031415926
mf7 = -0.00031412345
mf8 = 0.000031412345
mf9 = 714159.26
mf10 = !values.f_infinity
mf11 = !values.f_nan
mf12 = -!values.f_infinity
mf13 = -!values.f_nan
mf14 = -.0

EXP_STF1='      3.14159'
EXP_STF2='      3.14100'
EXP_STF3='     -3141.59'
EXP_STF4='      314159.'
EXP_STF5=' -3.14159e+07'
EXP_STF6='   0.00314159'
EXP_STF7=' -0.000314123'
EXP_STF8='  3.14123e-05'
EXP_STF9='      714159.'
EXP_STF10='          Inf'
EXP_STF11='          NaN'
EXP_STF12='         -Inf'
EXP_STF13='         -NaN'
EXP_STF14='     -0.00000'

stf1 = string(mf1)
stf2 = string(mf2)
stf3 = string(mf3)
stf4 = string(mf4)
stf5 = string(mf5)
stf6 = string(mf6)
stf7 = string(mf7)
stf8 = string(mf8)
stf9 = string(mf9)
stf10 = string(mf10)
stf11 = string(mf11)
stf12 = string(mf12)
stf13 = string(mf13)
stf14 = string(mf14)

if exp_stf1 NE stf1 then ERRORS_ADD, nb_pbs_float,  'Erreur float 1'
if exp_stf2 NE stf2 then ERRORS_ADD, nb_pbs_float,  'Erreur float 2'
if exp_stf3 NE stf3 then ERRORS_ADD, nb_pbs_float,  'Erreur float 3'
if exp_stf4 NE stf4 then ERRORS_ADD, nb_pbs_float,  'Erreur float 4'
if exp_stf5 NE stf5 then ERRORS_ADD, nb_pbs_float,  'Erreur float 5'
if exp_stf6 NE stf6 then ERRORS_ADD, nb_pbs_float,  'Erreur float 6'
if exp_stf7 NE stf7 then ERRORS_ADD, nb_pbs_float,  'Erreur float 7'
if exp_stf8 NE stf8 then ERRORS_ADD, nb_pbs_float,  'Erreur float 8'
if exp_stf9 NE stf9 then ERRORS_ADD, nb_pbs_float,  'Erreur float 9'
if exp_stf10 NE stf10 then ERRORS_ADD, nb_pbs_float,  'Erreur float 10'
if exp_stf11 NE stf11 then ERRORS_ADD, nb_pbs_float,  'Erreur float 11'
if exp_stf12 NE stf12 then ERRORS_ADD, nb_pbs_float,  'Erreur float 12'
if exp_stf13 NE stf13 then ERRORS_ADD, nb_pbs_float,  'Erreur float 13'
if exp_stf14 NE stf14 then ERRORS_ADD, nb_pbs_float,  'Erreur float 14'


if KEYWORD_SET(verbose) then begin
   ;help, stf1, stf2, stf3, stf4, stf5, stf6, stf7, stf8, stf9, stf10, stf11, stf12, stf13, stf14
endif
;
BANNER_FOR_TESTSUITE, 'TEST_FLOAT2STR', nb_pbs_float, /short, verb=verbose
;
ERRORS_CUMUL, cumul_errors, nb_pbs_float
;
if KEYWORD_set(test) then STOP
;
end
;
; ---------------------DOUBLE------------------------------
;
pro TEST_DOUBLE2STR, cumul_errors, verbose=verbose, test=test
;
nb_pbs_double = 0
;
md1 = 3.14159265359d
md2 = 3.141d
md3 = -3141.5926d
md4 = 314159.26d
md5 = -31415926.5359d
md6 = 0.0031415926d
md7 = -0.00031412345d
md8 = 0.000031412345d
md9 = 714159.26d
md10 = double(!values.f_infinity)
md11 = double(!values.f_nan)
md12 = -double(!values.f_infinity)
md13 = -double(!values.f_nan)
md14 = -.0d

EXP_ST1='       3.1415927'
EXP_ST2='       3.1410000'
EXP_ST3='      -3141.5926'
EXP_ST4='       314159.26'
EXP_ST5='      -31415927.'
EXP_ST6='    0.0031415926'
EXP_ST7='  -0.00031412345'
EXP_ST8='   3.1412345e-05'
EXP_ST9='       714159.26'
EXP_ST10='        Infinity'
EXP_ST11='             NaN'
EXP_ST12='       -Infinity'
EXP_ST13='            -NaN'
EXP_ST14='      -0.0000000'
st1 = string(md1)
st2 = string(md2)
st3 = string(md3)
st4 = string(md4)
st5 = string(md5)
st6 = string(md6)
st7 = string(md7)
st8 = string(md8)
st9 = string(md9)
st10 = string(md10)
st11 = string(md11)
st12 = string(md12)
st13 = string(md13)
st14 = string(md14)
;help, st1, st2, st3, st4, st5, st6, st7, st8, st9, st10, st11, st12, st13, st14
;help, md1,md2,md3,md4,md5,md6,md7,md8,md9,md10,md11
if exp_st1 NE st1 then ERRORS_ADD, nb_pbs_double, 'Erreur double 1'
if exp_st2 NE st2 then ERRORS_ADD, nb_pbs_double, 'Erreur double 2'
if exp_st3 NE st3 then ERRORS_ADD, nb_pbs_double, 'Erreur double 3'
if exp_st4 NE st4 then ERRORS_ADD, nb_pbs_double, 'Erreur double 4'
if exp_st5 NE st5 then ERRORS_ADD, nb_pbs_double, 'Erreur double 5'
if exp_st6 NE st6 then ERRORS_ADD, nb_pbs_double, 'Erreur double 6'
if exp_st7 NE st7 then ERRORS_ADD, nb_pbs_double, 'Erreur double 7'
if exp_st8 NE st8 then ERRORS_ADD, nb_pbs_double, 'Erreur double 8'
if exp_st9 NE st9 then ERRORS_ADD, nb_pbs_double, 'Erreur double 9'
if exp_st10 NE st10 then ERRORS_ADD, nb_pbs_double, 'Erreur double 10'
if exp_st11 NE st11 then ERRORS_ADD, nb_pbs_double, 'Erreur double 11'
if exp_st12 NE st12 then ERRORS_ADD, nb_pbs_double, 'Erreur double 12'
if exp_st13 NE st13 then ERRORS_ADD, nb_pbs_double, 'Erreur double 13'
if exp_st14 NE st14 then ERRORS_ADD, nb_pbs_double, 'Erreur double 14'
;
;
BANNER_FOR_TESTSUITE, 'TEST_DOUBLE2STR', nb_pbs_double, /short, verb=verbose
;
ERRORS_CUMUL, cumul_errors, nb_pbs_double
;
if KEYWORD_set(test) then STOP

end
;
; ---------------------COMPLEX-----------------------------
;
pro TEST_COMPLEX2STR, cumul_errors, test=test, verbose=verbose
nb_pbs_complex=0

mf1 = 3.14159265359
mf2 = 3.141
mf3 = -3141.5926
mf4 = 314159.26
mf5 = -31415926.5359
mf6 = 0.0031415926
mf7 = -0.00031412345
mf8 = 0.000031412345
mf9 = 714159.26
mf10 = !values.f_infinity
mf11 = !values.f_nan
mf12 = -!values.f_infinity
mf13 = -!values.f_nan
mf14 = -.0

mcp1 = COMPLEX(mf1,mf2)
mcp2 = COMPLEX(mf3,mf4)
mcp3 = COMPLEX(mf5,mf6)
mcp4 = COMPLEX(mf7,mf8)
mcp5 = COMPLEX(mf9,mf10)
mcp6 = COMPLEX(mf11,mf12)
mcp7 = COMPLEX(mf13,mf14)

EXP_ST1='(      3.14159,      3.14100)'
EXP_ST2='(     -3141.59,      314159.)'
EXP_ST3='( -3.14159e+07,   0.00314159)'
EXP_ST4='( -0.000314123,  3.14123e-05)'
EXP_ST5='(      714159.,          Inf)'
EXP_ST6='(          NaN,         -Inf)'
EXP_ST7='(         -NaN,     -0.00000)'


st1 = string(mcp1)
st2 = string(mcp2)
st3 = string(mcp3)
st4 = string(mcp4)
st5 = string(mcp5)
st6 = string(mcp6)
st7 = string(mcp7)

;help, st1, st2, st3, st4, st5, st6, st7

if exp_st1 NE st1 then ERRORS_ADD, nb_pbs_complex, 'Erreur complex 1'
if exp_st2 NE st2 then ERRORS_ADD, nb_pbs_complex, 'Erreur complex 2'
if exp_st3 NE st3 then ERRORS_ADD, nb_pbs_complex, 'Erreur complex 3'
if exp_st4 NE st4 then ERRORS_ADD, nb_pbs_complex, 'Erreur complex 4'
if exp_st5 NE st5 then ERRORS_ADD, nb_pbs_complex, 'Erreur complex 5'
if exp_st6 NE st6 then ERRORS_ADD, nb_pbs_complex, 'Erreur complex 6'
if exp_st7 NE st7 then ERRORS_ADD, nb_pbs_complex, 'Erreur complex 7'
;
;
BANNER_FOR_TESTSUITE, 'TEST_COMPLEX2STR', nb_pbs_complex, /short, verb=verbose
;
ERRORS_CUMUL, cumul_errors, nb_pbs_complex
;
if KEYWORD_SET(test) then STOP

end
;
; -------------------DCOMPLEX-----------------------------
;
pro TEST_DCOMPLEX2STR, cumul_errors, verbose=verbose, test=test
;
nb_pbs_dcomplex = 0
;
md1 = 3.14159265359d
md2 = 3.141d
md3 = -3141.5926d
md4 = 314159.26d
md5 = -31415926.5359d
md6 = 0.0031415926d
md7 = -0.00031412345d
md8 = 0.000031412345d
md9 = 714159.26d
md10 = double(!values.f_infinity)
md11 = double(!values.f_nan)
md12 = -double(!values.f_infinity)
md13 = -double(!values.f_nan)
md14 = -.0d

mcp1 = DCOMPLEX(md1,md2)
mcp2 = DCOMPLEX(md3,md4)
mcp3 = DCOMPLEX(md5,md6)
mcp4 = DCOMPLEX(md7,md8)
mcp5 = DCOMPLEX(md9,md10)
mcp6 = DCOMPLEX(md11,md12)
mcp7 = DCOMPLEX(md13,md14)

EXP_ST1='(       3.1415927,       3.1410000)'
EXP_ST2='(      -3141.5926,       314159.26)'
EXP_ST3='(      -31415927.,    0.0031415926)'
EXP_ST4='(  -0.00031412345,   3.1412345e-05)'
EXP_ST5='(       714159.26,        Infinity)'
EXP_ST6='(             NaN,       -Infinity)'
EXP_ST7='(            -NaN,      -0.0000000)'

st1 = string(mcp1)
st2 = string(mcp2)
st3 = string(mcp3)
st4 = string(mcp4)
st5 = string(mcp5)
st6 = string(mcp6)
st7 = string(mcp7)

;help, st1, st2, st3, st4, st5, st6, st7

if exp_st1 NE st1 then ERRORS_ADD, nb_pbs_dcomplex, 'Erreur 1'
if exp_st2 NE st2 then ERRORS_ADD, nb_pbs_dcomplex, 'Erreur 2'
if exp_st3 NE st3 then ERRORS_ADD, nb_pbs_dcomplex, 'Erreur 3'
if exp_st4 NE st4 then ERRORS_ADD, nb_pbs_dcomplex, 'Erreur 4'
if exp_st5 NE st5 then ERRORS_ADD, nb_pbs_dcomplex, 'Erreur 5'
if exp_st6 NE st6 then ERRORS_ADD, nb_pbs_dcomplex, 'Erreur 6'
if exp_st7 NE st7 then ERRORS_ADD, nb_pbs_dcomplex, 'Erreur 7'
;
;
BANNER_FOR_TESTSUITE, 'TEST_DCOMPLEX2STR', nb_pbs_dcomplex, verb=verbose, /short
;
ERRORS_CUMUL, cumul_errors, nb_pbs_dcomplex
;
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------STRLEN----------------------------
; To be removed ... We have to check if all the cases are included in
; the privious codes above
;
;
; Alain Coulais, June 2010
; under GNU GPL 2 or later
;
; few cases where STRING() conversion output
; differ between IDL and GDL
;
pro TEST_FLOAT2STRING, cumul_errors, verbose=verbose, test=test
;
nb_pbs_float_old = 0
;print, 'various Float conversion into strings:'

;print, STRING(123123.)
;print, STRING(123.)
;print, STRING(123.e)
;print, STRING(123.e0)
;print, STRING(123.e1)
;
;print, STRING(123.e, format='(f13.2)')
;print, STRING(123.e, format='(e13.3)')
;print, STRING(123.e1)
;print, STRING(123.e1, format='(e13.3)')
;print, STRING(123.e1, format='(e13.4)')
;print, STRING(123.e1, format='()')

;print, STRING(123.123e4)
;print, STRING(123.123e4, format='(e13.4)')
;
; bug id 2555865
;
;print, 'conversion into strings in C style:'
;
resu=string(format='(%"test32T_%dinp.dat")',42)
;print, 'Expected: >>test32T_42dinp.dat<<'
;print, 'Result:   >>'+resu+'<<'
if (STRLEN(resu) NE 17) then print, 'difference between Exp. and Res.'

BANNER_FOR_TESTSUITE, 'TEST_FLOAT2STRING', nb_pbs_float_old, verb=verbose, /short
;
ERRORS_CUMUL, cumul_errors, nb_pbs_float_old
;
if KEYWORD_SET(test) then STOP

end

; ---------------------SUMMARY-----------------------------
;

pro TEST_CONVERT2, help=help, test=test, verbose=verbose, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_CONVERT2, help=help, test=test, verbose=verbose, no_exit=no_exit'
   print, ''
   print, 'TEST_FLOAT2STR, TEST_DOUBLE2STR, TEST_COMPLEX2STR, TEST_DCOMPLEX2STR'
   return
endif 
;
cumul_errors=0
;
TEST_FLOAT2STR, cumul_errors, test=test, verbose=verbose
TEST_DOUBLE2STR, cumul_errors, test=test, verbose=verbose
TEST_COMPLEX2STR, cumul_errors, test=test, verbose=verbose
TEST_DCOMPLEX2STR, cumul_errors, test=test, verbose=verbose
;; to be removedCD .
TEST_FLOAT2STRING, cumul_errors, test=test, verbose=verbose
;; to be removed
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_CONVERT2', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

