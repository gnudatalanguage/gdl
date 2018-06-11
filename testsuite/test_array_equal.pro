;
; Some tests for ARRAY_EQUAL()
;
; Alain Coulais, 30 August 2013
;
; In fact, we may have tricky results when using 
; ARRAY_EQUAL() without taking into account dimentions:
;
; print, ARRAY_EQUAL([1],[1,1]) ; should return 0
; print, ARRAY_EQUAL(1,[1,1]) ; should return 1
;
; --------------------------------------------
;
; Modifications history :
;
;* AC 2017-10-01 : 
; - removed internal BANNER_FOR_TESTSUITE & MY_MESS (now ADD_ERROR)
; 
; --------------------------------------------
;
pro TEST_ARRAY_EQUAL_DIFF_TYPE, cumul_errors, verbose=verbose, test=test, help=help
;
case_name='TEST_ARRAY_EQUAL_DIFF_TYPE'
;
if KEYWORD_SET(help) then begin
   print, 'pro '+case_name+', cumul_errors, verbose=verbose, test=test, help=help'
   return
endif
;
line="======================================="
MESSAGE, /Continue, line
MESSAGE, /Continue, "this case suite TEST_ARRAY_EQUAL_DIFF_TYPE is not ready !"
MESSAGE, /Continue, "please contribute"
MESSAGE, /Continue, line
;
nb_pbs=0
;
;BANNER_FOR_TESTSUITE, case_name, nb_pbs
;ERRORS_CUMUL, cumul_errors, nb_pbs
;
end
;
; --------------------------------------------
;
pro TEST_ARRAY_EQUAL_CHECK_DIM, cumul_errors, verbose=verbose, test=test, help=help
;
case_name='TEST_ARRAY_EQUAL_CHECK_DIM'
;
if KEYWORD_SET(help) then begin
   print, 'pro '+case_name+', cumul_errors, verbose=verbose, test=test, help=help'
   return
endif
;
nb_pbs=0
indice=0
;
a_singleton=1
a_array=[1]
a_full_array=REPLICATE(1, 5)
;
if ARRAY_EQUAL(a_singleton, a_singleton) NE 1 then begin
   ADD_ERRORS, nb_pbs, 'ARRAY_EQUAL(a_singleton, a_singleton)'
endif
if ARRAY_EQUAL(a_array, a_array) NE 1 then begin
   ADD_ERRORS, nb_pbs, 'ARRAY_EQUAL(a_array, a_array)'
endif
if ARRAY_EQUAL(a_full_array, a_full_array) NE 1 then begin
   ADD_ERRORS, nb_pbs, 'ARRAY_EQUAL(a_full_array, a_full_array)'
endif
;
if ARRAY_EQUAL(a_singleton, a_array) NE 1 then begin
   ADD_ERRORS, nb_pbs, 'ARRAY_EQUAL(a_singleton, a_array)'
endif
if ARRAY_EQUAL(a_array, a_singleton) NE 1 then begin
   ADD_ERRORS, nb_pbs, 'ARRAY_EQUAL(a_array, a_singleton)'
endif
;
if ARRAY_EQUAL(a_singleton, a_full_array) NE 1 then begin
   ADD_ERRORS, nb_pbs, 'ARRAY_EQUAL(a_singleton, a_full_array)'
endif
if ARRAY_EQUAL(a_full_array, a_singleton) NE 1 then begin
   ADD_ERRORS, nb_pbs, 'ARRAY_EQUAL(a_full_array, a_singleton)'
endif
;
; When both inputs are arrays, if n_elements differents, should return ZERO
;
if ARRAY_EQUAL(a_array, a_full_array) NE 0 then begin
   ADD_ERRORS, nb_pbs, 'ARRAY_EQUAL(a_array, a_full_array)'
endif
if ARRAY_EQUAL(a_full_array, a_array) NE 0 then begin
   ADD_ERRORS, nb_pbs, 'ARRAY_EQUAL(a_full_array, a_array)'
endif
;
BANNER_FOR_TESTSUITE, case_name, nb_pbs, /short
;
ERRORS_CUMUL, cumul_errors, nb_pbs
;
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------
;
pro TEST_ARRAY_EQUAL_SAME_TYPE, cumul_errors, verbose=verbose, test=test, help=help
;
case_name='TEST_ARRAY_EQUAL_SAME_TYPE'
;
if KEYWORD_SET(help) then begin
   print, 'pro '+case_name+', cumul_errors, verbose=verbose, test=test, help=help'
   return
endif
;
nb_pbs=0
indice=0
;
a_singleton=1
a_array=[1]
a_full_array=REPLICATE(1, 5)
;
b_singleton=10
b_array=[10]
b_full_array=REPLICATE(10, 5)
;
if ARRAY_EQUAL(a_singleton, b_array) NE 0 then begin
    ADD_ERRORS, nb_pbs, 'ARRAY_EQUAL(a_singleton, b_array)'
endif
if ARRAY_EQUAL(a_array, b_singleton) NE 0 then begin
    ADD_ERRORS, nb_pbs, 'ARRAY_EQUAL(a_array, b_singleton)'
endif
;
;
if ARRAY_EQUAL(a_singleton, b_full_array) NE 0 then begin
    ADD_ERRORS, nb_pbs, 'ARRAY_EQUAL(a_singleton, b_full_array)'
endif
if ARRAY_EQUAL(a_full_array, b_singleton) NE 0 then begin
    ADD_ERRORS, nb_pbs, 'ARRAY_EQUAL(a_full_array, b_singleton)'
endif
;
; When both inputs are arrays, if n_elements differents, should return ZERO
;
if ARRAY_EQUAL(a_array, b_full_array) NE 0 then begin
    ADD_ERRORS, nb_pbs, 'ARRAY_EQUAL(a_array, b_full_array)'
endif
if ARRAY_EQUAL(a_full_array, b_array) NE 0 then begin
    ADD_ERRORS, nb_pbs, 'ARRAY_EQUAL(a_full_array, b_array)'
endif
;
BANNER_FOR_TESTSUITE, case_name, nb_pbs, /short
;
ERRORS_CUMUL, cumul_errors, nb_pbs
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------------------------------------
;
pro TEST_ARRAY_EQUAL, no_exit=no_exit, verbose=verbose, test=test, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_ARRAY_EQUAL, no_exit=no_exit, verbose=verbose, test=test, help=help'
   return
endif
;
nb_errors=0
;
TEST_ARRAY_EQUAL_CHECK_DIM, nb_errors, verbose=verbose, test=test, help=help
TEST_ARRAY_EQUAL_SAME_TYPE, nb_errors, verbose=verbose, test=test, help=help
TEST_ARRAY_EQUAL_DIFF_TYPE, nb_errors, verbose=verbose, test=test, help=help
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, "TEST_ARRAY_EQUAL", nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
if ~ARRAY_EQUAL(findgen(20), findgen(20)+20.,/not_equal) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

