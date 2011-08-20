;
; Alain Coulais 20/08/2011, under GNP v2 or later
;
; quick test for IDL_VALIDNAME function
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
pro TEST_IDL_VALIDNAME, test=test, verbose=verbose
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
print, 'no finish ...'
;
print, nb_pbs
if (nb_pbs GT 0) then begin
   if ~KEYWORD_SET(test) then EXIT, status=1
endif else begin
   MESSAGE, /Continue, "No problem found"
endelse
;
if KEYWORD_SET(test) then STOP
;
end
