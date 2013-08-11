;
; Alain Coulais 20/08/2011, under GNP v2 or later
;
; quick test for IDL_VALIDNAME function
;
; Since August 2013, we have to test also input as a list
;
;---------------------------------------------
;
pro SMART_MESSAGE, input, expected, resu
print, '%TEST_IDL_VALIDNAME: error at input: >>'+input+'<<'
txt='input: >>'+input+'<<, expected: >>'+expected+'<<, '
print, txt+'resu: >>'+resu+'<<'
end
;
pro INTERNAL_TEST, input, expected, kwverb, nb_pbs, _extra=_extra
;
resu=IDL_VALIDNAME(input, _extra=_extra)
;
if (STRCMP(expected, resu) NE 1) then begin
   if (kwverb EQ 1) then SMART_MESSAGE, input, expected, resu
   nb_pbs=nb_pbs+1
endif
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
if (nb_pbs GT 0) then begin
   MESSAGE, /continue, 'Number of problems detected: '+STRING(nb_pbs)
   ;; now the EXIT code is managed at the end
   ;; if ~KEYWORD_SET(test) then EXIT, status=1
endif else begin
   MESSAGE, /Continue, "No problem found"
endelse
;
if KEYWORD_SET(test) then STOP
;
errors_in_single=nb_pbs
;
end
;
;---------------------------------------------
;
pro TEST_IDL_VALIDNAME_MULTI,errors_in_multi, test=test, verbose=verbose
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
if (nb_pbs GT 0) then begin
   MESSAGE, /continue, 'Number of problems detected: '+STRING(nb_pbs)
   ;; now the EXIT code is managed at the end
   ;; if ~KEYWORD_SET(test) then EXIT, status=1
endif else begin
   MESSAGE, /Continue, "No problem found"
endelse
;
if KEYWORD_SET(test) then STOP
;
errors_in_multi=nb_pbs
;
end
;---------------------------------------------
;
;
pro TEST_IDL_VALIDNAME, test=test, verbose=verbose, help=help, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_IDL_VALIDNAME, no_exit=no_exit, test=test, verbose=verbose, help=help'
   return
endif
;
print, 'This testsuite for IDL_VALIDNAME is not finished ...'
print, 'Please contribute'
;
TEST_IDL_VALIDNAME_SINGLE, errors_in_single, test=test, verbose=verbose
;
TEST_IDL_VALIDNAME_MULTI, errors_in_multi, test=test, verbose=verbose
;
if ((errors_in_single GT 0) OR (errors_in_multi GT 0)) then begin
   if ~KEYWORD_SET(no_exit) then EXIT, status=1
endif
;
end

