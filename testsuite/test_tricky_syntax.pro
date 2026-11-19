;
; AC 2026-11-10 under GNU GPL v3+
;
; The goal is to anticipate any potential regression ...
;
; https://github.com/gnudatalanguage/gdl/issues/2275
; sometime the syntax might be surprising ...
;
pro TEST_SYNTAX_NUMERIC, cumul_errors, unvisible=unvisible, $
                         test=test, verbose=verbose
;
errors=0
mess=', error on result'
;
if KEYWORD_SET(unvisible) then CompileFlags=1 else CompileFlags=0
;
a=1.
;
; must failed ! status=0
;
result=-1
txt='First case'
status=EXECUTE('result=COS(1d-a)', CompileFlags)
if (status NE 0) then ERRORS_ADD, errors, txt
;if ABS(result-a) GT 1e-6 then ERRORS_ADD, errors, txt+mess
if KEYWORD_SET(verbose) then print, txt, status, result
;
; must failed ! status=0
;
result=-1
txt='Second case'
status=EXECUTE('result=COS(1.d-a)', CompileFlags)
if (status NE 0) then ERRORS_ADD, errors, txt
;if ABS(result-a) GT 1e-6 then ERRORS_ADD, errors, txt+mess
if KEYWORD_SET(verbose) then print, txt, status, result
;
; must succeed ! status=1
;
; take care of the extra space : this one is OK
result=-1
txt='OK case (space)'
status=EXECUTE('result=COS(1.d -a)', CompileFlags)
if (status NE 1) then ERRORS_ADD, errors, txt
if ABS(result-a) GT 1e-6 then ERRORS_ADD, errors, txt+mess
if KEYWORD_SET(verbose) then print, txt, status, result
;
result=-1
txt='OK case (parenthesis)'
status=EXECUTE('result=COS((1.d)-a)', CompileFlags)
if (status NE 1) then ERRORS_ADD, errors, txt
if ABS(result-a) GT 1e-6 then ERRORS_ADD, errors, txt+mess
if KEYWORD_SET(verbose) then print, txt, status, result
;
; -------------- super tricky for me ------------
;
result=-1   
txt='PROBLEM case (numeric) +1. (dot)'
status=EXECUTE('result=COS(1.d-1.)', CompileFlags)
if (status NE 0) then ERRORS_ADD, errors, txt
;if ABS(result-a) GT 1e-6 then ERRORS_ADD, errors, txt+mess
if KEYWORD_SET(verbose) then print, txt, status, result
;
result=-1
txt='OK case (numeric) +1 (no dot)'
status=EXECUTE('result=COS(1.d-1)', CompileFlags)
if (status NE 1) then ERRORS_ADD, errors, txt
if ABS(result-0.99500417) GT 1e-6 then ERRORS_ADD, errors, txt+mess
if KEYWORD_SET(verbose) then print, txt, status, result
;
; --------------
;
if CompileFlags and (errors GT 0) then begin
   MESSAGE, /continue, '------ Since errors detected'
   MESSAGE, /continue, 'Please consider rerun the test withou flag /UNVISIBLE'
   MESSAGE, /continue, 'to have the exact messages returned'
endif
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_SYNTAX_NUMERIC", errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------
;
pro TEST_TRICKY_SYNTAX, no_exit=no_exit, help=help

cumul_errors=0
;
TEST_SYNTAX_NUMERIC, cumul_errors, /unvisible, $
                     test=test, verbose=verbose
;
; final message
;
BANNER_FOR_TESTSUITE, 'TEST_TRICKY_SYNTAX', cumul_errors, short=short
;
if KEYWORD_SET(test) then STOP
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
end
