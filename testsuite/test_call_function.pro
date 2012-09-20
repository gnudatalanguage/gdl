;
; Alain C., 22 June 2012
;
; More systematic tests on EXECUTE, CALL_FUNCTION and CALL_PROCEDURE
;
; ----------------------------------------------
; we add a keyword (add_one) to test also keyword,
; because of bug report 3490415 
; we add a keyword (via_keyword) to test also value transmited by
; keyword, because of bug report 3569697
;
function FUNC_MY_FUNC, x, add_one=add_one, via_keyword=via_key_keyword
;
resultat=x+5
if KEYWORD_SET(add_one) then resultat=resultat+1
;
; transmitting outside value via keyword
via_key_keyword=-1
;
return, resultat
;
end
;
; --------------------
;
pro BASIC_CALL_FUNCTION, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'pro BASIC_CALL_FUNCTION, help=help, test=test, no_exit=no_exit, verbose=verbose'
    return
endif
;
nb_errors = 0
tolerance=1e-5
;
; internal intrinsic function, single value
expected=-1.
result=CALL_FUNCTION('COS', !pi)
;
if (ABS(result-expected) GT tolerance)  then begin
    MESSAGE, /continue, 'Error at ARRAY call level'
    nb_errors=nb_errors+1
endif
if KEYWORD_SET(verbose) then print, result, expected
;
; internal intrinsic function, array
;
expected=REPLICATE(-1.,10)
result=CALL_FUNCTION('COS', REPLICATE(!pi,10))
;
if (TOTAL(ABS(result-expected)) GT tolerance)  then begin
    MESSAGE, /continue, 'Error at Intrinsic Function call level'
    nb_errors=nb_errors+1
endif
if KEYWORD_SET(verbose) then print, result, expected
;
; external function, single element
;
expected=0.
result=CALL_FUNCTION('FUNC_MY_FUNC', -5.)
;
if (ABS(result-expected) GT tolerance)  then begin
    MESSAGE, /continue, 'Error at basic Self Defined Function call level'
    nb_errors=nb_errors+1
endif
if KEYWORD_SET(verbose) then print, result, expected
;
; external function, single element, one keyword
;
expected=1.
result=CALL_FUNCTION('FUNC_MY_FUNC', -5., /add_one)
;
if (ABS(result-expected) GT tolerance)  then begin
    MESSAGE, /continue, 'Error at /KEYWORD (Self Defined Function) call level'
    nb_errors=nb_errors+1
endif
if KEYWORD_SET(verbose) then print, result, expected
;
; external function, single element, one keyword by value
;
expected=0.
key_expected=-1.
result=CALL_FUNCTION('FUNC_MY_FUNC', -5., via_keyword=via_keyword)
;
if (ABS(result-expected) GT tolerance) then begin
    MESSAGE, /continue, 'Error 1 at KEYWORD BY VALUE (Self Defined Function) call level'
    nb_errors=nb_errors+1
endif
if KEYWORD_SET(verbose) then print, result, expected
;
if (ABS(via_keyword-key_expected) GT tolerance) then begin
    MESSAGE, /continue, 'Error 2 at KEYWORD BY VALUE (Self Defined Function) call level'
    nb_errors=nb_errors+1
endif
if KEYWORD_SET(verbose) then print, via_keyword, key_expected
;
; external function, single element, two keywords
;
expected=1.
key_expected=-1.
result=CALL_FUNCTION('FUNC_MY_FUNC', -5., via_keyword=via_keyword,/add_one)
;
if (ABS(result-expected) GT tolerance) then begin
    MESSAGE, /continue, 'Error 1 at KEYWORD BY VALUE (Self Defined Function) call level'
    nb_errors=nb_errors+1
endif
if KEYWORD_SET(verbose) then print, result, expected
;
if (ABS(via_keyword-key_expected) GT tolerance) then begin
    MESSAGE, /continue, 'Error 2 at KEYWORD BY VALUE (Self Defined Function) call level'
    nb_errors=nb_errors+1
endif
if KEYWORD_SET(verbose) then print, via_keyword, key_expected
;;
; external function, value 2D array
;
expected=0.
result=CALL_FUNCTION('FUNC_MY_FUNC', REPLICATE(-5,12,3))
;
if (TOTAL(ABS(result-expected)) GT tolerance)  then begin
    MESSAGE, /continue, 'Error at ARRAY (Self Defined Function) call level'
    nb_errors=nb_errors+1
endif
if KEYWORD_SET(verbose) then print, result, expected
;
;
;
if (nb_errors GT 0) then begin
    MESSAGE, STRING(nb_errors)+' Errors founded when testing CALL_FUNCTION', /continue
endif else begin
    MESSAGE, 'testing CALL_FUNCTION: No Errors found', /continue
endelse
;
if KEYWORD_SET(test) then STOP
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
end
;
pro TEST_CALL_FUNCTION, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_CALL_FUNCTION, help=help, test=test, no_exit=no_exit, verbose=verbose'
    return
endif
;
BASIC_CALL_FUNCTION, help=help, test=test, no_exit=no_exit, verbose=verbose
;
end
