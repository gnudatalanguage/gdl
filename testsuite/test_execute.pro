;
; Alain C., 22 June 2012
;
; More systematic tests on EXECUTE, CALL_FUNCTION and CALL_PROCEDURE
;
pro PRO_MY_PRO, x, y
;
y=x+5
;
end
;
function FUNC_MY_FUNC, x
;
return, x+5
;
end
;
; ---------------------
; old temporay bug
;
pro TEST_BUG_3441031
;; this caused a segfault
res=EXECUTE("a = STRJOIN(STRSPLIT((['a'])[1],'a'),'a')")
;
end
;
; another old temporay bug
;
; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
pro OLD_TEST_EXECUTE

if EXECUTE('print, EXECUTE([''''])') then begin
   MESSAGE, 'EXECUTE should not accept array arguments', /conti
   EXIT, status=1
endif
;
end
;
; --------------------
;
pro BASIC_EXECUTE, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'pro BASIC_EXECUTE, help=help, test=test, no_exit=no_exit, verbose=verbose'
    return
endif
;
nb_errors = 0
tolerance=1e-5
;
; internal intrinsic function, single value
com='a=COS(!pi)'
expected=-1.
status=EXECUTE(com)
;
if (status NE 1) then nb_errors=nb_errors+1
if (ABS(a-expected) GT tolerance)  then nb_errors=nb_errors+1
if KEYWORD_SET(verbose) then print, com, status, a, expected
;
; internal intrinsic function, array
;
com='a=COS(REPLICATE(!pi,10))'
expected=REPLICATE(-1.,10)
status=EXECUTE(com)
;
if (status NE 1) then nb_errors=nb_errors+1
if (TOTAL(ABS(a-expected)) GT tolerance)  then nb_errors=nb_errors+1
if KEYWORD_SET(verbose) then print, com, status, a, expected
;
; internal intrinsic procedure (better idea welcome !)
;
com='plot, SIN(!pi*findgen(100)/10.)'
status=EXECUTE(com)
;
if (status NE 1) then nb_errors=nb_errors+1
;
; external function, single element
;
com='a=FUNC_MY_FUNC(12.)'
expected=17.
status=EXECUTE(com)
;
if (status NE 1) then nb_errors=nb_errors+1
if (ABS(a-expected) GT tolerance)  then nb_errors=nb_errors+1
if KEYWORD_SET(verbose) then print, com, status, a, expected
;
; external function, value 2D array
;
com='a=FUNC_MY_FUNC(REPLICATE(-5,12,3))'
expected=REPLICATE(0.,12,3)
status=EXECUTE(com)
;
if (status NE 1) then nb_errors=nb_errors+1
if (TOTAL(ABS(a-expected)) GT tolerance)  then nb_errors=nb_errors+1
if KEYWORD_SET(verbose) then print, com, status, a, expected
;
; external function, named' 2D array
;
input=REPLICATE(-5,12,3)
com='a=FUNC_MY_FUNC(input)'
expected=input+5.
status=EXECUTE(com)
;
if (status NE 1) then nb_errors=nb_errors+1
if (TOTAL(ABS(a-expected)) GT tolerance)  then nb_errors=nb_errors+1
if KEYWORD_SET(verbose) then print, com, status, a, expected
;
;
;
if (nb_errors GT 0) then begin
    MESSAGE, STRING(nb_errors)+' Errors founded when testing EXECUTE', /continue
endif else begin
    MESSAGE, 'testing EXECUTE: No Errors founded', /continue
endelse
;
if KEYWORD_SET(test) then STOP
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
WDELETE
;
end
;
; ----------------------------------------------------
;
pro TEST_EXECUTE, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_EXECUTE, help=help, test=test, no_exit=no_exit, verbose=verbose'
    return
endif;
;
TEST_BUG_3441031
OLD_TEST_EXECUTE
;
BASIC_EXECUTE, help=help, test=test, no_exit=no_exit, verbose=verbose
;
end
