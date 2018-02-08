;
; Alain Coulais 20/08/2011, under GNP v2 or later
;
; Quick test for IDL_VALIDNAME function
;
; Since August 2013, we have to test also input as a list
;
; ---------------------------------
; Modifications history :
;
; 2018-Feb-08: AC. cleaning ... (keyword /verbose not working ...)
;
;---------------------------------------------
;
pro SMART_MESSAGE, input, expected, resu
print, '%TEST_IDL_VALIDNAME: error at input: >>'+input+'<<'
txt='input: >>'+input+'<<, expected: >>'+expected+'<<, '
print, txt+'resu: >>'+resu+'<<'
end
;
; ---------------------------------
;
pro INTERNAL_TEST, input, expected, kwverb, nb_pbs, _extra=_extra
;
resu=IDL_VALIDNAME(input, _extra=_extra)
;
if (STRCMP(expected, resu) NE 1) then ERRORS_ADD, nb_pbs, 'case : '+input
if (kwverb EQ 1) then SMART_MESSAGE, input, expected, resu
;
end
;
;---------------------------------------------
;
pro TEST_IDL_VALIDNAME_SINGLE, errors_in_single, test=test, verbose=verbose
;
; we need a working STREGEX to be able to run IDL_VALIDNAME
;
if (STREGEX('1abc','[^0-9a-z]') NE -1) then begin
   MESSAGE, /continue, 'No working STREGEX, we cannot do this test.'
   if KEYWORD_SET(test) then STOP else EXIT, status=77
endif
;
; useful variables
;
nb_pbs=0
if KEYWORD_SET(verbose) then kwverb=1 else kwverb=0
;
INTERNAL_TEST, 'abc', 'abc', kwverb, nb_pbs
INTERNAL_TEST, 'a b c', '', kwverb, nb_pbs
INTERNAL_TEST, ' a b c', '', kwverb, nb_pbs
INTERNAL_TEST, ' a b c ', '', kwverb, nb_pbs
INTERNAL_TEST, ' a b c ', '', kwverb, nb_pbs
;
; spaces
;
INTERNAL_TEST, 'abc', 'abc', kwverb, nb_pbs, /convert_spaces
INTERNAL_TEST, 'a b c', 'a_b_c', kwverb, nb_pbs, /convert_spaces
INTERNAL_TEST, ' a b c', '_a_b_c', kwverb, nb_pbs, /convert_spaces
INTERNAL_TEST, 'a b c ', 'a_b_c_', kwverb, nb_pbs, /convert_spaces
INTERNAL_TEST, ' a b c ', '_a_b_c_', kwverb, nb_pbs, /convert_spaces
;
; numbers at begining
;
INTERNAL_TEST, '1abc', '_1abc', kwverb, nb_pbs, /convert_all
INTERNAL_TEST, '2a b c', '_2a_b_c', kwverb, nb_pbs, /convert_all
INTERNAL_TEST, '3 a b c', '_3_a_b_c', kwverb, nb_pbs, /convert_all
INTERNAL_TEST, '4a b c ', '_4a_b_c_', kwverb, nb_pbs, /convert_all
INTERNAL_TEST, '5 a b c ', '_5_a_b_c_', kwverb, nb_pbs, /convert_all
;
; "$" at begining
;
INTERNAL_TEST, '$abc', '', kwverb, nb_pbs
INTERNAL_TEST, '$abc', '', kwverb, nb_pbs, /convert_spaces
INTERNAL_TEST, '$1abc', '_$1abc', kwverb, nb_pbs, /convert_all
INTERNAL_TEST, '$2a b c', '_$2a_b_c', kwverb, nb_pbs, /convert_all
INTERNAL_TEST, '$ 3 a b c', '_$_3_a_b_c', kwverb, nb_pbs, /convert_all
INTERNAL_TEST, '$4a b c ', '_$4a_b_c_', kwverb, nb_pbs, /convert_all
INTERNAL_TEST, '$5 a b c ', '_$5_a_b_c_', kwverb, nb_pbs, /convert_all
;
; exemple gived by Tim, patch 66
;
a = '`~!@#$%^&*()_+-={} []|\";;/?.>,<123456789'
res='_____$__________________________123456789'
INTERNAL_TEST, a, '', kwverb, nb_pbs
INTERNAL_TEST, a, '', kwverb, nb_pbs, /convert_spaces
INTERNAL_TEST, a, res, kwverb, nb_pbs, /convert_all
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_IDL_VALIDNAME_SINGLE', nb_pbs, /short
ERRORS_CUMUL, errors_in_single, nb_pbs
if KEYWORD_SET(test) then STOP
;
end
;
;---------------------------------------------
;
pro TEST_IDL_VALIDNAME_MULTI, errors_in_multi, test=test, verbose=verbose
;
nb_pbs=0
;
input=['!a!!a', '']
filtre=REPLICATE(1,N_ELEMENTS(input))
;
expected=['','']
result=IDL_VALIDNAME(input)
if ~ARRAY_EQUAL(STRCMP(result,expected),filtre) then begin
   nb_pbs++
   if KEYWORD_SET(verbose) then begin
      print, 'pb 1 here : ', input
   endif
endif
;
expected=['!a__a','_']
result=IDL_VALIDNAME(input,/convert_all)
if ~ARRAY_EQUAL(STRCMP(result,expected),filtre) then begin
   nb_pbs++
   if KEYWORD_SET(verbose) then begin
      print, 'pb 1 here : ', input
   endif
endif
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_IDL_VALIDNAME_MULTI', nb_pbs, /short
ERRORS_CUMUL, errors_in_multi, nb_pbs
if KEYWORD_SET(test) then STOP
;
end
;
;---------------------------------------------
;
pro TEST_IDL_VALIDNAME, test=test, verbose=verbose, help=help, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_IDL_VALIDNAME, no_exit=no_exit, test=test, verbose=verbose, help=help'
   return
endif
;
cumul_errors=0
;
print, 'This testsuite for IDL_VALIDNAME is not finished ...'
print, 'Please contribute'
;
TEST_IDL_VALIDNAME_SINGLE, cumul_errors, test=test, verbose=verbose
;
TEST_IDL_VALIDNAME_MULTI, cumul_errors, test=test, verbose=verbose
;
; ---- Final message ----
;
BANNER_FOR_TESTSUITE, 'TEST_IDL_VALIDNAME', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

