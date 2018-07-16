;
; Alain C., 21 March 2013
;
; draft: very preliminary version for testing BYTSCL(),
; the last case is buggy (when we have an "f_infinity", 
; max at f_infinity is convert in 255B)
;
; Code of BYTSCL() revised in March 2017 to take
; into account /Nan and also to use check on Types
; at a good level
;
; WARNING ! 9-March-2017 : this test is NOT working with IDL
; when we add NAN/INF  --> TEST_BYTSCL_IDL_PROBLEM
;
; WARNING ! in idlwave 6.1, indentation problems !!
; ForEach/EndForEach loops indentation are not managed
;
; ------------------------
;
; AC 2017-03-13
; testing the TOP keyword, no clear idea now ...
; how to solve the differences between IDL & GDL
;
pro TEST_BYTSCL_TOP, cumul_errors, test=test, verbose=verbose
;
nb_errors=0
;
input=DIST(512)
;
; just testing min/max now, more to do when basics will be fixed !!
;
top_list=[600, 256, 255, 128, 0, -128]
expect_max=[255, 255, 255, 128, 0, 255]
expect_min=0 ; always
;
for ii=0, N_ELEMENTS(top_list)-1 do begin
   calculus=BYTSCL(input, top=top_list[ii])
   result=[MIN(calculus), MAX(calculus)]
   expected=[expect_min,expect_max[ii]]
   ;;
   if ARRAY_EQUAL(expected, result) NE 1 then begin
      mess=', min/max : '+STRING(FIX(result[0]))+', '+STRING(FIX(result[1]))
      ERRORS_ADD, nb_errors, 'Pb with top= '+STRING(top_list[ii])+mess
   endif
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_BYTSCL_RAMPS', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
;
pro TEST_BYTSCL_RAMPS, cumul_errors, test=test, verbose=verbose
;
nb_errors=0
;
expected=BYTARR(10)
expected[*]=[0,28,56,85,113,142,170,199,227,255]
;
for itype=1, 15 do begin
   if (itype NE 8) then begin
      if ISA(MAKE_ARRAY(1, type=itype),/number) then begin
         ramp=INDGEN(10, type=itype)
         resu=BYTSCL(ramp)
         if ARRAY_EQUAL(expected, resu) NE 1 then begin
            ERRORS_ADD, nb_errors, 'TYPE : '+STRING(itype)
         endif
      endif
   endif
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_BYTSCL_RAMPS', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
;
pro TEST_BYTSCL_RAMPS_NAN, cumul_errors, test=test, verbose=verbose
;
nb_errors=0
;
expected=BYTARR(10)
expected[*]=[0,28,56,85,113,142,170,199,227,255]
;
expected_nan=BYTARR(10)
expected_nan[6]=255
;
expected_nan_flag=expected
expected_nan_flag[5:6]=0
;
; loop over Float/Double & Complex/Dcomplex
;
no_int=[4,5,6,9]
FOREACH itype, no_int do begin
   ;; init
   ramp_nan_inf=INDGEN(10, type=itype)
   ;; add Nan & Inf
   ramp_nan_inf[5]=!values.f_nan
   ramp_nan_inf[6]=!values.f_infinity
   ;;
   ;; without /nan flag
   resu_nan=BYTSCL(ramp_nan_inf)
   if ARRAY_EQUAL(expected_nan, resu_nan) NE 1 then begin
      ERRORS_ADD, nb_errors, 'pb in TYPE : '+STRING(itype)
   endif
   ;; 
   ;; with /nan flag
   resu_nan_flag=BYTSCL(ramp_nan_inf,/nan)
   if ARRAY_EQUAL(expected_nan_flag, resu_nan_flag) NE 1 then begin
      ERRORS_ADD, nb_errors, 'pb in TYPE + /NAN flag: '+STRING(itype)
   endif
ENDFOREACH
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_BYTSCL_RAMPS_NAN', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
; convenience procedure used below
pro TEST_BYTSCL_PRINT, input, expected, outpout
print, 'Real Part Input    : ', REAL_PART(input)
print, 'Imag Part Input    : ', IMAGINARY(input)
print, 'expected : ', expected
print, 'result   : ', outpout
end
;
; We have specific problems with IDL, I do not understand why
; they cannot manage the Complex/DComplex cases, and nothing
; in the IDL documentation in BYTSCL. Furthermore we cannot
; simply that much the code to avoid triggering Math exceptions ...
;
pro TEST_BYTSCL_IDL_PROBLEM, cumul_errors, test=test, verbose=verbose, debug=debug
;
if KEYWORD_SET(debug) then debug=1 else debug=0
;
nb_errors=0
;
input_nan=[0., !values.f_nan, 0, 10, 20]
input_inf=[0., !values.f_infinity, 0, 10, 20]
input_mix=[0., !values.f_nan, 0, !values.f_infinity, 0, 10, 20]
;
exp_nan= BYTE([0, 0, 0, 127, 255])
exp_nan_flag=exp_nan
;
exp_inf= BYTE([0, 255, 0, 0,0 ])
exp_inf_flag= BYTE([0, 0, 0, 127, 255])
;
exp_mix= BYTE([0, 0, 0, 255, 0, 0, 0])
exp_mix_flag= BYTE([0, 0, 0, 0, 0, 127, 255])
;
no_int=[4,5,610,910,611,911]    ;,601,901]
FOREACH itype, no_int do begin
   ;;
   if itype EQ 4 then begin
      used_nan=input_nan
      used_inf=input_inf
      used_mix=input_mix
   endif
   if itype EQ 5 then begin
      used_nan=DOUBLE(input_nan)
      used_inf=DOUBLE(input_inf)
      used_mix=DOUBLE(input_mix)
   endif
   if itype EQ 610 OR itype EQ 910 then begin
      if itype EQ 610 then double=0 else double=1
      used_nan=COMPLEX(input_nan, REPLICATE(0., N_ELEMENTS(input_nan)), double=double)
      used_inf=COMPLEX(input_inf, REPLICATE(0., N_ELEMENTS(input_inf)), double=double)
      used_mix=COMPLEX(input_mix, REPLICATE(0., N_ELEMENTS(input_mix)), double=double)
   endif
   if itype EQ 611 OR itype EQ 911 then begin
      if itype EQ 611 then double=0 else double=1
      used_nan=COMPLEX(input_nan, input_nan, double=double)
      used_inf=COMPLEX(input_inf, input_inf, double=double)
      used_mix=COMPLEX(input_mix, input_mix, double=double)
   endif
   ;;
   if ARRAY_EQUAL(BYTSCL(used_nan), exp_nan) NE 1 then begin
      ERRORS_ADD, nb_errors, 'pb in TYPE + NAN + no flag: '+STRING(itype)
      if debug then TEST_BYTSCL_PRINT, used_nan, exp_nan, BYTSCL(used_nan)
   endif
   if ARRAY_EQUAL(BYTSCL(used_inf), exp_inf) NE 1 then begin
      ERRORS_ADD, nb_errors, 'pb in TYPE + Inf + no flag: '+STRING(itype)
      if debug then TEST_BYTSCL_PRINT, used_inf, exp_inf, BYTSCL(used_inf)
   endif
   if ARRAY_EQUAL(BYTSCL(used_mix), exp_mix) NE 1 then begin
      ERRORS_ADD, nb_errors, 'pb in TYPE + MIX + no flag: '+STRING(itype)
      if debug then TEST_BYTSCL_PRINT, used_mix, exp_mix, BYTSCL(used_mix)
   endif
   ;;
   ;; with the /NAN flag on
   ;;
   if ARRAY_EQUAL(BYTSCL(used_nan, /NAN), exp_nan_flag) NE 1 then begin
      ERRORS_ADD, nb_errors, 'pb in TYPE + NAN + /NAN flag ON: '+STRING(itype)
      if debug then TEST_BYTSCL_PRINT, used_nan, exp_nan_flag, BYTSCL(used_nan,/nan)
   endif
   if ARRAY_EQUAL(BYTSCL(used_inf, /NAN), exp_inf_flag) NE 1 then begin
      ERRORS_ADD, nb_errors, 'pb in TYPE + Inf + /NAN flag ON: '+STRING(itype)
      if debug then TEST_BYTSCL_PRINT, used_inf, exp_inf_flag, BYTSCL(used_inf,/nan)
   endif
   if ARRAY_EQUAL(BYTSCL(used_mix, /NAN), exp_mix_flag) NE 1 then begin
      ERRORS_ADD, nb_errors, 'pb in TYPE + MIX + /NAN flag ON: '+STRING(itype)
      if debug then TEST_BYTSCL_PRINT, used_mix, exp_mix_flag, BYTSCL(used_mix,/nan)
   endif
ENDFOREACH
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_BYTSCL_IDL_PROBLEM', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
;
pro TEST_BYTSCL, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_BYTSCL, help=help, verbose=verbose, $'
   print, '                 no_exit=no_exit, test=test'
   return
endif
;
nl=STRING(10B)
print, nl+"PLEASE contribute to add tests on MIN, MAX, TOP keywords"+nl
;
TEST_BYTSCL_TOP, nb_errors, test=test
;
TEST_BYTSCL_RAMPS, nb_errors, test=test
;
TEST_BYTSCL_RAMPS_NAN, nb_errors, test=test
;
; This test is a clone of previous test, testing what is "expected"
; for types Float, Double, Complex and DComplex with NaN and Inf ...
; In IDL, the outputs for Complex and DComplex are not understanded.
;
TEST_BYTSCL_IDL_PROBLEM, nb_errors, test=test, verbose=verbose
;
ERRORS_ADD, nb_errors, 'IDL does not pass this test, please re-write the test before removing this error'
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_BYTSCL', nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
