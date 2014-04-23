;
; Alain C., 23 April 2014: just moving this code
; from outside TEST_ARRAY_EQUAL to be common
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
