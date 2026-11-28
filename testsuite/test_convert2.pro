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

pro TEST_CONVERT2_WRAP, cumul_errors, values, expected, illegal, source_types
type = SIZE(expected, /TYPE)
foreach threads, [1, 4] do begin
   CPU, TPOOL_NTHREADS=threads, TPOOL_MIN_ELTS=1, TPOOL_MAX_ELTS=0
   foreach source_type, source_types do begin
      if (source_type eq 6) || (source_type eq 9) then begin
         ; Only the real component contributes to the result and its flags.
         source = COMPLEX(values, REPLICATE(!VALUES.F_NAN, N_ELEMENTS(values)), DOUBLE=(source_type eq 9))
      endif else source = FIX(values, TYPE=source_type)
      input = REPLICATE(FIX(0, TYPE=source_type), 4097)
      ; Exercise a whole worker group, one exceptional lane, and the final tail.
      foreach pos, [3968L, 4087L, 4096L] do begin
         last = pos eq 3968L ? pos + 63L : pos
         for i = 0, N_ELEMENTS(source) - 1 do begin
            input[pos:last] = source[i]
            flags = CHECK_MATH()
            scalar = FIX(source[i], TYPE=type)
            scalar_flags = CHECK_MATH()
            array = FIX(input, TYPE=type)
            flags = CHECK_MATH()
            if (scalar ne expected[i]) || ~ARRAY_EQUAL(array[pos:last], REPLICATE(expected[i], last - pos + 1)) || $
               ((scalar_flags and 128) ne illegal[i]) || ((flags and 128) ne illegal[i]) then $
               ERRORS_ADD, cumul_errors, TYPENAME(source)+' to '+TYPENAME(expected)+ $
                  ' at '+STRING(values[i], FORMAT='(G24.17)')+ $
                  ' with '+STRING(threads, FORMAT='(I0)')+' thread(s)'
         endfor
         input[pos:last] = 0
      endforeach
   endforeach
endforeach
end

pro TEST_CONVERT2_INDICES, cumul_errors
saved_except = !EXCEPT
!EXCEPT = 0
values = [11, 22, 33]
indices = [-!VALUES.D_INFINITY, -0.75D, 1.75D, 2D^64, !VALUES.D_INFINITY, !VALUES.D_NAN]
foreach type, [4, 5, 6, 9] do begin
   idx = FIX(indices, TYPE=type)
   if ~ARRAY_EQUAL(values[idx], [11, 11, 22, 33, 33, 33]) then $
      ERRORS_ADD, cumul_errors, 'Array subscript clipping for '+TYPENAME(idx)
   ; REPLICATE_INPLACE checks the subscripts on its selected dimension strictly.
   rows = INTARR(2, 3)
   REPLICATE_INPLACE, rows, 1, 1, [0, 0], 2, idx[1:2]
   if ~ARRAY_EQUAL(rows[0, *], [1, 1, 0]) then $
      ERRORS_ADD, cumul_errors, 'Strict fractional subscripts for '+TYPENAME(idx)
   ; NaN takes the positive overflow path, which also sets CHECK_MATH.
   flags = CHECK_MATH()
   nan_pick = values[idx[[5]]]
   flags = CHECK_MATH()
   if (nan_pick[0] ne 33) || ((flags and 128) eq 0) then $
      ERRORS_ADD, cumul_errors, 'NaN subscript or CHECK_MATH flag for '+TYPENAME(idx)
   updated = values
   updated[idx[[5]]] = 99
   if ~ARRAY_EQUAL(updated, [11, 22, 99]) then $
      ERRORS_ADD, cumul_errors, 'NaN subscript assignment for '+TYPENAME(idx)
   ; Strict indexing rejects negative, overflowing and NaN subscripts.
   foreach i, [0, 3, 4, 5] do begin
      bad = idx[[1, i]]
      if EXECUTE('REPLICATE_INPLACE, rows, 1, 1, [0, 0], 2, bad', 1) then $
         ERRORS_ADD, cumul_errors, 'Strict indexing accepted an out-of-range '+TYPENAME(idx)
   endforeach
endforeach
flags = CHECK_MATH()
!EXCEPT = saved_except
end

pro TEST_CONVERT2_INVALID_STRING, cumul_errors
values = REPLICATE('7', 1024)
values[1023] = 'invalid'
result = -1L
ON_IOERROR, invalid_element
result = LONG(values)
ERRORS_ADD, cumul_errors, 'LONG did not raise ON_IOERROR for a late invalid string'
return
invalid_element:
if ~ARRAY_EQUAL(result, -1L) then $
   ERRORS_ADD, cumul_errors, 'Failed STRING conversion overwrote the assignment target'
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
saved_cpu = !CPU
foreach type, [2, 12, 3, 13, 14, 15] do begin
   CPU, TPOOL_NTHREADS=1, TPOOL_MIN_ELTS=64, TPOOL_MAX_ELTS=0
   values = INDGEN(8192, TYPE=type)
   expected = STRING(values)
   CPU, TPOOL_NTHREADS=4
   if ~ARRAY_EQUAL(STRING(values), expected) then $
      ERRORS_ADD, cumul_errors, 'Parallel STRING conversion differs for '+TYPENAME(values)
endforeach
;
; 40000 wraps to -25536 in a signed 16-bit INT on every conversion path.
if FIX(40000.) ne -25536 then $
   ERRORS_ADD, cumul_errors, 'Scalar FIX(40000.) must return -25536'
values = REPLICATE(40000., 8192)
expected = REPLICATE(-25536, 8192)
foreach threads, [1, 4] do begin
   CPU, TPOOL_NTHREADS=threads, TPOOL_MIN_ELTS=1, TPOOL_MAX_ELTS=0
   if ~ARRAY_EQUAL(FIX(values), expected) then $
      ERRORS_ADD, cumul_errors, 'FIX of a 40000. array must return -25536 with '+ $
         STRING(threads, FORMAT='(I0)')+' thread(s)'
endforeach
CPU, RESTORE=saved_cpu
;
; Cover the complete numeric dispatch matrix on the threaded array path.
CPU, TPOOL_NTHREADS=4, TPOOL_MIN_ELTS=1, TPOOL_MAX_ELTS=0
numeric_types = [1, 2, 12, 3, 13, 14, 15, 4, 5, 6, 9]
base_values = LINDGEN(4097) MOD 43
foreach source_type, numeric_types do begin
   source = FIX(base_values, TYPE=source_type)
   foreach destination_type, numeric_types do begin
      result = FIX(source, TYPE=destination_type)
      if (SIZE(result, /TYPE) ne destination_type) || ~ARRAY_EQUAL(result, base_values) then $
         ERRORS_ADD, cumul_errors, 'Numeric conversion '+TYPENAME(source)+ $
            ' to type '+STRING(destination_type, FORMAT='(I0)')
   endforeach
endforeach
;
; Integer narrowing must retain the low destination-width bits.
if FIX(65535L, TYPE=2) ne -1 then $
   ERRORS_ADD, cumul_errors, 'LONG to INT low-bit conversion'
if FIX(-1L, TYPE=12) ne 65535U then $
   ERRORS_ADD, cumul_errors, 'LONG to UINT low-bit conversion'
if FIX(4294967295ULL, TYPE=3) ne -1L then $
   ERRORS_ADD, cumul_errors, 'ULONG64 to LONG low-bit conversion'
if FIX(-1LL, TYPE=13) ne 4294967295UL then $
   ERRORS_ADD, cumul_errors, 'LONG64 to ULONG low-bit conversion'
if FIX(NOT 0ULL, TYPE=14) ne -1LL then $
   ERRORS_ADD, cumul_errors, 'ULONG64 to LONG64 low-bit conversion'
;
; Inside the wrap window, truncation retains the low destination bits.
; These values are exactly representable in all four source types.
saved_except = !EXCEPT
!EXCEPT = 0
min32 = -2147483647L - 1L
max32 = 2147483647L
min64 = -9223372036854775807LL - 1LL
max64 = 9223372036854775807LL
umax32 = 4294967295UL
umax64 = 18446744073709551615ULL
bound = 2D^63
values = [0D, -3D9, 3D9, -1.5D, -0.75D, 2D^31, 2D^32, -bound, bound, 2D*bound]
types = [4, 5, 6, 9]
high32 = 2147483648UL
high64 = 9223372036854775808ULL
TEST_CONVERT2_WRAP, cumul_errors, values, $
   [0L, 1294967296L, -1294967296L, -1L, 0L, min32, min32, min32, min32, min32], $
   [0, 128, 128, 0, 0, 128, 128, 128, 128, 128], types
TEST_CONVERT2_WRAP, cumul_errors, values, $
   [0UL, 1294967296UL, 3000000000UL, umax32, 0UL, high32, high32, high32, high32, high32], $
   [0, 0, 0, 0, 0, 0, 128, 128, 128, 128], types
TEST_CONVERT2_WRAP, cumul_errors, values, $
   [0LL, -3000000000LL, 3000000000LL, -1LL, 0LL, 2147483648LL, 4294967296LL, min64, min64, min64], $
   [0, 0, 0, 0, 0, 0, 0, 0, 128, 128], types
TEST_CONVERT2_WRAP, cumul_errors, values, $
   [0ULL, umax64-2999999999ULL, 3000000000ULL, umax64, 0ULL, 2147483648ULL, 4294967296ULL, high64, high64, high64], $
   [0, 0, 0, 0, 0, 0, 0, 0, 0, 128], types
;
; NaN and both infinities have the same sentinel as an oversized finite value.
values = [!VALUES.D_NAN, !VALUES.D_INFINITY, -!VALUES.D_INFINITY]
illegal = [128, 128, 128]
TEST_CONVERT2_WRAP, cumul_errors, values, [min32, min32, min32], illegal, types
TEST_CONVERT2_WRAP, cumul_errors, values, [high32, high32, high32], illegal, types
TEST_CONVERT2_WRAP, cumul_errors, values, [min64, min64, min64], illegal, types
TEST_CONVERT2_WRAP, cumul_errors, values, $
   [high64, high64, high64], illegal, types
;
; Adjacent DOUBLEs at the signed cast boundary differ in their low fraction bits.
edge = 2D^31
below = edge - 2D^(-22)
above = edge + 2D^(-21)
TEST_CONVERT2_WRAP, cumul_errors, [below, edge, above, -below, -edge, -above], $
   [max32, min32, min32, -max32, min32, min32], [0, 128, 128, 0, 0, 0], [5, 9]
; An exceptional imaginary component must not affect the real conversion or its flags.
real_values = LINDGEN(4097) MOD 43 - 21L
imag_values = REPLICATE(!VALUES.D_NAN, N_ELEMENTS(real_values))
foreach wide, [0, 1] do begin
   source = COMPLEX(real_values, imag_values, DOUBLE=wide)
   flags = CHECK_MATH()
   result = LONG(source)
   flags = CHECK_MATH()
   if ~ARRAY_EQUAL(result, real_values) || ((flags and 128) ne 0) then $
      ERRORS_ADD, cumul_errors, 'Imaginary NaN in '+TYPENAME(source)+' to LONG'
endforeach
; Check adjacent DOUBLE values and fractions whose truncation still fits.
TEST_CONVERT2_WRAP, cumul_errors, [-2147483648.75D, 2147483647.75D], $
   [min32, max32], [0, 0], [5, 9]
; Nearest DOUBLE on each side of the positive and negative wrap limits.
cutoff = 2D^32
below = cutoff - 2D^(-21)
above = cutoff + 2D^(-20)
values = [-above, -cutoff, -below, below, cutoff, above]
flags_at_cutoff = [128, 128, 0, 0, 128, 128]
TEST_CONVERT2_WRAP, cumul_errors, values, $
   [min32, min32, 1L, -1L, min32, min32], REPLICATE(128, 6), [5, 9]
TEST_CONVERT2_WRAP, cumul_errors, values, $
   [high32, high32, 1UL, umax32, high32, high32], flags_at_cutoff, [5, 9]
foreach type, [1, 2, 12] do begin
   expected = FIX([0L, 0L, 1L, -1L, 0L, 0L], TYPE=type)
   TEST_CONVERT2_WRAP, cumul_errors, values, expected, REPLICATE(128, 6), [5, 9]
endforeach
values = [-bound-2048D, -bound, -bound+1024D, bound-1024D, bound, bound+2048D]
TEST_CONVERT2_WRAP, cumul_errors, values, $
   [max64-2047LL, min64, min64+1024LL, max64-1023LL, min64, min64+2048LL], $
   [128, 0, 0, 0, 128, 128], [5, 9]
TEST_CONVERT2_WRAP, cumul_errors, values, $
   [high64-2048ULL, high64, high64+1024ULL, high64-1024ULL, high64, high64+2048ULL], $
   [128, 0, 0, 0, 0, 0], [5, 9]
cutoff = 2D * bound
values = [-cutoff-4096D, -cutoff, -cutoff+2048D, cutoff-2048D, cutoff, cutoff+4096D]
TEST_CONVERT2_WRAP, cumul_errors, values, $
   [min64, min64, 2048LL, -2048LL, min64, min64], REPLICATE(128, 6), [5, 9]
TEST_CONVERT2_WRAP, cumul_errors, values, $
   [high64, high64, 2048ULL, umax64-2047ULL, high64, high64], [128, 128, 128, 0, 128, 128], [5, 9]
TEST_CONVERT2_WRAP, cumul_errors, [-1D19, 1D19], $
   [8446744073709551616LL, -8446744073709551616LL], [128, 128], [5, 9]
TEST_CONVERT2_WRAP, cumul_errors, [-1D19, 1D19], $
   [8446744073709551616ULL, 10000000000000000000ULL], [128, 0], [5, 9]
TEST_CONVERT2_WRAP, cumul_errors, [-1D300, 1D300], [min64, min64], [128, 128], [5, 9]
; Adjacent single-precision values below 2^32 and above 2^63.
TEST_CONVERT2_WRAP, cumul_errors, [4294967040.], [-256L], [128], [4, 6]
TEST_CONVERT2_WRAP, cumul_errors, [4294967040.], [4294967040UL], [0], [4, 6]
TEST_CONVERT2_WRAP, cumul_errors, [2.^63 + 2.^40], [min64 + 2LL^40], [128], [4, 6]
edge = 2.^63 + 2.^40
TEST_CONVERT2_WRAP, cumul_errors, [-edge, edge], $
   [high64 - 2ULL^40, high64 + 2ULL^40], [128, 0], [4, 6]
flags = CHECK_MATH()
!EXCEPT = saved_except
;
; A NUL terminates its own row. A full row retains every byte.
bytes = REFORM([0b,90b,90b, 65b,0b,90b, 128b,255b,66b], 3, 3)
strings = STRING(bytes)
if (strings[0] ne '') || (strings[1] ne 'A') || $
   ~ARRAY_EQUAL(BYTE(strings[2]), [128b,255b,66b]) then $
   ERRORS_ADD, cumul_errors, 'BYTE rows with embedded NUL or high-bit bytes'
;
; Preserve whitespace, prefix parsing, and Fortran D-exponent compatibility.
integer_strings = REPLICATE('  -42 trailing', 1024)
foreach type, [2, 12, 3, 13, 14, 15] do begin
   expected = REPLICATE(FIX(-42LL, TYPE=type), 1024)
   if ~ARRAY_EQUAL(FIX(integer_strings, TYPE=type), expected) then $
      ERRORS_ADD, cumul_errors, 'STRING conversion to integer type '+ $
         STRING(type, FORMAT='(I0)')
endforeach
real_strings = REPLICATE('  1D2 trailing', 1024)
foreach type, [4, 5, 6, 9] do begin
   expected = REPLICATE(FIX(100D, TYPE=type), 1024)
   if ~ARRAY_EQUAL(FIX(real_strings, TYPE=type), expected) then $
      ERRORS_ADD, cumul_errors, 'STRING conversion to real type '+ $
         STRING(type, FORMAT='(I0)')
endforeach
TEST_CONVERT2_INVALID_STRING, cumul_errors
TEST_CONVERT2_INDICES, cumul_errors
CPU, RESTORE=saved_cpu
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

