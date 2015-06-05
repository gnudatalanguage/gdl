;
; AC, 1er June 2015
;
; very preliminary tests for ISHFT, to be extended 
;
pro ADD_ERRORS, nb_errors, message
;
print, 'Error on operation : '+message
nb_errors=nb_errors+1
;
end
;
pro TEST_ISHFT, test=test, help=help, no_exit=no_exit
;
if KEYWORD_SET(test) then begin
    print, 'pro TEST_ISHFT, test=test, help=help, no_exit=no_exit'
    return
endif
;
nb_errors=0
;
tableau=BINDGEN(8)
;
; very basic test
;
expected=tableau*2^3
result=ISHFT(tableau, 3)  
if ~ARRAY_EQUAL(expected, result) then ADD_ERRORS, nb_errors, 'error 1 2^3'
;
; bug reported by Bill D., Mai 30, 2015 
;
expected=2^tableau
result=ISHFT(1b, tableau)
if ~ARRAY_EQUAL(expected, result) then ADD_ERRORS, nb_errors, 'error 2 BD'
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_ISHFT', nb_errors;, short=short
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP

end

