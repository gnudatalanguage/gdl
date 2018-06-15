;
; Alain C., 22 June 2012
;
; More systematic tests on EXECUTE
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
; old  (corrected) bug
;
pro TEST_BUG_3441031, cumul_errors, test=test
;;
errors=0
;
; this caused a segfault, we don't care of the result
;
res=EXECUTE("a = STRJOIN(STRSPLIT((['a'])[1],'a'),'a')")
;
if (res EQ 1) then ERRORS_ADDS, errors, 'Case STRJOIN'
;
BANNER_FOR_TESTSUITE, 'TEST_BUG_3441031', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; ---------------------
; another old (corrected) bug
;
; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
pro TEST_EXECUTE_OLD, cumul_errors, test=test
;
errors=0
;
res=EXECUTE('print, EXECUTE([''''])')
;
txt='EXECUTE should not accept array arguments'
if (res EQ 1) then ERRORS_ADDS, errors, txt
;
BANNER_FOR_TESTSUITE, 'TEST_EXECUTE_OLD', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; --------------------
;
pro TEST_BASIC_EXECUTE, cumul_errors, help=help, test=test, $
                        verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_BASIC_EXECUTE, cumul_errors, help=help, test=test, $'
    print, '                        verbose=verbose'
    return
endif
;
errors = 0
tolerance=1e-5
;
; internal intrinsic function, single value
com='a=COS(!pi)'
expected=-1.
status=EXECUTE(com)
;
if (status NE 1) then ERRORS_ADDS, errors, 'Cos Status'
if (ABS(a-expected) GT tolerance)  then ERRORS_ADDS, errors, 'Cos value'
if KEYWORD_SET(verbose) then print, com, status, a, expected
;
; internal intrinsic function, array
;
com='a=COS(REPLICATE(!pi,10))'
expected=REPLICATE(-1.,10)
status=EXECUTE(com)
;
if (status NE 1) then ERRORS_ADDS, errors, 'Cos Status (arr)'
if (TOTAL(ABS(a-expected)) GT tolerance) then $
   ERRORS_ADDS, errors, 'Cos Value (arr)'
if KEYWORD_SET(verbose) then print, com, status, a, expected
;
; internal intrinsic procedure (better idea welcome !)
;
com='plot, SIN(!pi*findgen(100)/10.)'
status=EXECUTE(com)
;
if (status NE 1) then ERRORS_ADDS, errors, 'Sin Status'
WDELETE
;
; external function, single element
;
com='a=FUNC_MY_FUNC(12.)'
expected=17.
status=EXECUTE(com)
;
if (status NE 1) then ERRORS_ADDS, errors, 'FUNC_MY_FUNC Status'
if (ABS(a-expected) GT tolerance) then $
   ERRORS_ADDS, errors, 'FUNC_MY_FUNC valeur 12'
if KEYWORD_SET(verbose) then print, com, status, a, expected
;
; external function, value 2D array
;
com='a=FUNC_MY_FUNC(REPLICATE(-5,12,3))'
expected=REPLICATE(0.,12,3)
status=EXECUTE(com)
;
if (status NE 1) then ERRORS_ADDS, errors, 'FUNC_MY_FUNC Status (arr)'
if (TOTAL(ABS(a-expected)) GT tolerance) then $
   ERRORS_ADDS, errors, 'FUNC_MY_FUNC valeur (arr)'
if KEYWORD_SET(verbose) then print, com, status, a, expected
;
; external function, named' 2D array
;
input=REPLICATE(-5,12,3)
com='a=FUNC_MY_FUNC(input)'
expected=input+5.
status=EXECUTE(com)
;
if (status NE 1) then ERRORS_ADDS, errors, 'FUNC_MY_FUNC Status (input)'
if (TOTAL(ABS(a-expected)) GT tolerance) then $
   ERRORS_ADDS, errors, 'FUNC_MY_FUNC valeur (input)'
if KEYWORD_SET(verbose) then print, com, status, a, expected
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_BASIC_EXECUTE', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
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
TEST_BUG_3441031, cumul_errors
TEST_EXECUTE_OLD, cumul_errors
;
TEST_BASIC_EXECUTE, cumul_errors, test=test, verbose=verbose
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_EXECUTE', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP

;
end
