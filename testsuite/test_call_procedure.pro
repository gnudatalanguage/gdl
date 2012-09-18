;
; Alain C., 22 June 2012
;
; More systematic tests on EXECUTE, CALL_FUNCTION and CALL_PROCEDURE
;
; ----------------------------------------------
; we add a keyword to test also keyword, because of bug report 3490415 
;
pro PRO_MY_PRO, x, y, add_one=add_one
;
y=x+5
;
if KEYWORD_SET(add_one) then y=y+1.
;
end
;
; --------------------
;
pro BASIC_CALL_PROCEDURE, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'pro BASIC_CALL_PROCEDURE, help=help, test=test, no_exit=no_exit, verbose=verbose'
    return
endif
;
nb_errors = 0
tolerance=1e-5
;
; internal intrinsic function, single value
;
txt1='XYZ is cool'
txt2='GDL is cool'
CALL_PROCEDURE, 'STRPUT', txt1, 'GDL', 0
;
if (STRCMP(txt1, txt2) NE 1)  then nb_errors=nb_errors+1
if KEYWORD_SET(verbose) then print, txt1, txt2
;
; internal intrinsic function, array
;
; no idea now, help welcome
;
; external function, single element
;
expected=17.
CALL_PROCEDURE, 'PRO_MY_PRO', 12, result
;
if (ABS(result-expected) GT tolerance)  then begin
   MESSAGE, /continue, 'Error at basic call level'
   nb_errors=nb_errors+1
endif
if KEYWORD_SET(verbose) then print, result, expected
;
; external function, single element, keyword
;
expected=17.+1.
CALL_PROCEDURE, 'PRO_MY_PRO', 12, result, /add_one
;
if (ABS(result-expected) GT tolerance)  then begin
   MESSAGE, /continue, 'Error at /keyword call level'
   nb_errors=nb_errors+1
endif
if KEYWORD_SET(verbose) then print, result, expected
;
; external function, value 2D array
;
data_in=REPLICATE(-5,12,3)
PRO_MY_PRO, data_in, data_out1
CALL_PROCEDURE, 'PRO_MY_PRO', data_in, data_out2
;
if (TOTAL(ABS(data_out1-data_out2)) GT tolerance)  then begin
   MESSAGE, /continue, 'Error at ARRAY call level'
   nb_errors=nb_errors+1
endif
if KEYWORD_SET(verbose) then print, data_out1-data_out2
;
;
if (nb_errors GT 0) then begin
    MESSAGE, STRING(nb_errors)+' Errors founded when testing CALL_PROCEDURE', /continue
endif else begin
    MESSAGE, 'testing CALL_PROCEDURE: No Errors founded', /continue
endelse
;
if KEYWORD_SET(test) then STOP
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
end
;
; ----------------------------------------------------
;
pro TEST_CALL_PROCEDURE, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_CALL_PROCEDURE, help=help, test=test, no_exit=no_exit, verbose=verbose'
    return
endif
;
BASIC_CALL_PROCEDURE, help=help, test=test, no_exit=no_exit, verbose=verbose
;
end
