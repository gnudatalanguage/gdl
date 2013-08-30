;
; Some tests for ARRAY_EQUAL()
;
; Alain Coulais, 30 August 2013
;
; In fact, we may have tricky results when using 
; ARRAY_EQUAL() without taking ainto account dimentions:
;
; print, ARRAY_EQUAL([1],[1,1]) ; should return 0
; print, ARRAY_EQUAL(1,[1,1]) ; should return 1
;
; --------------------------------------------
;
pro BANNER_FOR_TESTSUITE, case_name, nb_pbs
;
prefixe='% '+STRUPCASE(case_name)+': '
;
message=' errors encoutered during '+STRUPCASE(case_name)+' tests'
if (nb_pbs GT 0) then message=STRING(nb_pbs)+message else message='NO'+message
;
line="======================================="
MESSAGE, /Continue
print, prefixe, line
print, prefixe, " "
print, prefixe, message
print, prefixe, " "
print, prefixe, line
;
end
;
; --------------------------------------------
;
pro MY_MESS, message, verbose=verbose
prefixe='% TEST_ARRAY_EQUAL: error in '
print, prefixe+message
end
;
; --------------------------------------------
;
pro TEST_ARRAY_EQUAL_DIFF_TYPE, nb_pbs, no_exit=no_exit, verbose=verbose, test=test, help=help
;
case_name='TEST_ARRAY_EQUAL_DIFF_TYPE'
;
if KEYWORD_SET(help) then begin
   print, 'pro '+case_name+', no_exit=no_exit, verbose=verbose, test=test, help=help'
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
;
end
;
; --------------------------------------------
;
pro TEST_ARRAY_EQUAL_CHECK_DIM, nb_pbs, verbose=verbose, test=test, help=help
;
case_name='TEST_ARRAY_EQUAL_CHECK_DIM'
;
if KEYWORD_SET(help) then begin
   print, 'pro '+case_name+', verbose=verbose, test=test, help=help'
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
    if KEYWORD_SET(verbose) then MY_MESS, 'ARRAY_EQUAL(a_singleton, a_singleton)'
    nb_pbs=nb_pbs+1
endif
if ARRAY_EQUAL(a_array, a_array) NE 1 then begin
    if KEYWORD_SET(verbose) then MY_MESS, 'ARRAY_EQUAL(a_array, a_array)'
    nb_pbs=nb_pbs+1
endif
if ARRAY_EQUAL(a_full_array, a_full_array) NE 1 then begin
    if KEYWORD_SET(verbose) then MY_MESS, 'ARRAY_EQUAL(a_full_array, a_full_array)'
    nb_pbs=nb_pbs+1
endif
;
if ARRAY_EQUAL(a_singleton, a_array) NE 1 then begin
    if KEYWORD_SET(verbose) then MY_MESS, 'ARRAY_EQUAL(a_singleton, a_array)'
    nb_pbs=nb_pbs+1
endif
if ARRAY_EQUAL(a_array, a_singleton) NE 1 then begin
    if KEYWORD_SET(verbose) then MY_MESS, 'ARRAY_EQUAL(a_array, a_singleton)'
    nb_pbs=nb_pbs+1
endif
;
;
if ARRAY_EQUAL(a_singleton, a_full_array) NE 1 then begin
    if KEYWORD_SET(verbose) then MY_MESS, 'ARRAY_EQUAL(a_singleton, a_full_array)'
    nb_pbs=nb_pbs+1
endif
if ARRAY_EQUAL(a_full_array, a_singleton) NE 1 then begin
    if KEYWORD_SET(verbose) then MY_MESS, 'ARRAY_EQUAL(a_full_array, a_singleton)'
    nb_pbs=nb_pbs+1
endif
;
; When both inputs are arrays, if n_elements differents, should return ZERO
;
if ARRAY_EQUAL(a_array, a_full_array) NE 0 then begin
    if KEYWORD_SET(verbose) then MY_MESS, 'ARRAY_EQUAL(a_array, a_full_array)'
    nb_pbs=nb_pbs+1
endif
if ARRAY_EQUAL(a_full_array, a_array) NE 0 then begin
    if KEYWORD_SET(verbose) then MY_MESS, 'ARRAY_EQUAL(a_full_array, a_array)'
    nb_pbs=nb_pbs+1
endif
;
BANNER_FOR_TESTSUITE, case_name, nb_pbs
;
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------
;
pro TEST_ARRAY_EQUAL_SAME_TYPE, nb_pbs, verbose=verbose, test=test, help=help
;
case_name='TEST_ARRAY_EQUAL_SAME_TYPE'
;
if KEYWORD_SET(help) then begin
   print, 'pro '+case_name+', no_exit=no_exit, verbose=verbose, test=test, help=help'
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
    if KEYWORD_SET(verbose) then MY_MESS, 'ARRAY_EQUAL(a_singleton, b_array)'
    nb_pbs=nb_pbs+1
endif
if ARRAY_EQUAL(a_array, b_singleton) NE 0 then begin
    if KEYWORD_SET(verbose) then MY_MESS, 'ARRAY_EQUAL(a_array, b_singleton)'
    nb_pbs=nb_pbs+1
endif
;
;
if ARRAY_EQUAL(a_singleton, b_full_array) NE 0 then begin
    if KEYWORD_SET(verbose) then MY_MESS, 'ARRAY_EQUAL(a_singleton, b_full_array)'
    nb_pbs=nb_pbs+1
endif
if ARRAY_EQUAL(a_full_array, b_singleton) NE 0 then begin
    if KEYWORD_SET(verbose) then MY_MESS, 'ARRAY_EQUAL(a_full_array, b_singleton)'
    nb_pbs=nb_pbs+1
endif
;
; When both inputs are arrays, if n_elements differents, should return ZERO
;
if ARRAY_EQUAL(a_array, b_full_array) NE 0 then begin
    if KEYWORD_SET(verbose) then MY_MESS, 'ARRAY_EQUAL(a_array, b_full_array)'
    nb_pbs=nb_pbs+1
endif
if ARRAY_EQUAL(a_full_array, b_array) NE 0 then begin
    if KEYWORD_SET(verbose) then MY_MESS, 'ARRAY_EQUAL(a_full_array, b_array)'
    nb_pbs=nb_pbs+1
endif
;
BANNER_FOR_TESTSUITE, case_name, nb_pbs
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
TEST_ARRAY_EQUAL_CHECK_DIM, nb_pbs_check_dim, verbose=verbose, test=test, help=help
TEST_ARRAY_EQUAL_SAME_TYPE, nb_pbs_same_type, verbose=verbose, test=test, help=help
TEST_ARRAY_EQUAL_DIFF_TYPE, nb_pbs_diff_type, verbose=verbose, test=test, help=help
;
nb_pbs=nb_pbs_check_dim+nb_pbs_same_type+nb_pbs_diff_type
;
BANNER_FOR_TESTSUITE, "TEST_ARRAY_EQUAL", nb_pbs
;
; if /debug OR /test nodes, we don't want to exit
if (nb_pbs GT 0) then begin
    if ~KEYWORD_SET(verbose) then MESSAGE, /continue, 're-run with /verbose for details'
    if ~(KEYWORD_SET(test) or KEYWORD_SET(no_exit)) then EXIT, status=1
endif
;
if KEYWORD_SET(test) then STOP
;
end

