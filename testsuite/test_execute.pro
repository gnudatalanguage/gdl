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
if (res EQ 1) then ERRORS_ADD, errors, 'Case STRJOIN'
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
if (res EQ 1) then ERRORS_ADD, errors, txt
;
BANNER_FOR_TESTSUITE, 'TEST_EXECUTE_OLD', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; --------------------
;
; All the tests in this procedure should fail
; (EXECUTE() should return 0)
;
pro TEST_EXECUTE_MISSING, cumul_errors, help=help, test=test, $
                          verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_EXECUTE_MISSING, cumul_errors, help=help, test=test, $'
    print, '                          verbose=verbose'
    return
endif
;
errors = 0
;
status=EXECUTE('z=MY_UNKNOW_FUNCTION()')
if (status EQ 1) then ERRORS_ADD, errors, 'function without param'
;
status=EXECUTE('z=MY_UNKNOW_FUNCTION(1, 2)')
if (status EQ 1) then ERRORS_ADD, errors, 'function with param'
;
status=EXECUTE('MY_UNKNOW_PROCEDURE')
if (status EQ 1) then ERRORS_ADD, errors, 'procedure without param'
;
status=EXECUTE('MY_UNKNOW_PROCEDURE, findgen(10)')
if (status EQ 1) then ERRORS_ADD, errors, 'procedure with param'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_EXECUTE_MISSING', errors, /short
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
if (status NE 1) then ERRORS_ADD, errors, 'Cos Status'
if (ABS(a-expected) GT tolerance)  then ERRORS_ADD, errors, 'Cos value'
if KEYWORD_SET(verbose) then print, com, status, a, expected
;
; internal intrinsic function, array
;
com='a=COS(REPLICATE(!pi,10))'
expected=REPLICATE(-1.,10)
status=EXECUTE(com)
;
if (status NE 1) then ERRORS_ADD, errors, 'Cos Status (arr)'
if (TOTAL(ABS(a-expected)) GT tolerance) then $
   ERRORS_ADD, errors, 'Cos Value (arr)'
if KEYWORD_SET(verbose) then print, com, status, a, expected
;
; internal intrinsic procedure (better idea welcome !)
;
; REMOVED AS MAY STOP OR CRASH TESTS!
;com='plot, SIN(!pi*findgen(100)/10.)'
;status=EXECUTE(com)
;
;if (status NE 1) then ERRORS_ADD, errors, 'Sin Status'
;
; external function, single element
;
com='a=FUNC_MY_FUNC(12.)'
expected=17.
status=EXECUTE(com)
;
if (status NE 1) then ERRORS_ADD, errors, 'FUNC_MY_FUNC Status'
if (ABS(a-expected) GT tolerance) then $
   ERRORS_ADD, errors, 'FUNC_MY_FUNC valeur 12'
if KEYWORD_SET(verbose) then print, com, status, a, expected
;
; external function, value 2D array
;
com='a=FUNC_MY_FUNC(REPLICATE(-5,12,3))'
expected=REPLICATE(0.,12,3)
status=EXECUTE(com)
;
if (status NE 1) then ERRORS_ADD, errors, 'FUNC_MY_FUNC Status (arr)'
if (TOTAL(ABS(a-expected)) GT tolerance) then $
   ERRORS_ADD, errors, 'FUNC_MY_FUNC valeur (arr)'
if KEYWORD_SET(verbose) then print, com, status, a, expected
;
; external function, named' 2D array
;
input=REPLICATE(-5,12,3)
com='a=FUNC_MY_FUNC(input)'
expected=input+5.
status=EXECUTE(com)
;
if (status NE 1) then ERRORS_ADD, errors, 'FUNC_MY_FUNC Status (input)'
if (TOTAL(ABS(a-expected)) GT tolerance) then $
   ERRORS_ADD, errors, 'FUNC_MY_FUNC valeur (input)'
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
; Helpers for #2265: errors in called routines must reach EXECUTE.
pro EXECUTE_FAILING_PRO, kind, reached
case kind of
   0: EXECUTE_MISSING_PRO_2265
   1: value=EXECUTE_MISSING_FUN_2265()
   2: value=execute_undefined_variable_2265+1
   3: begin
      value=[1]
      value=value[2]
   end
   4: MESSAGE, 'Expected EXECUTE regression error'
endcase
reached=1
end
;
function EXECUTE_FAILING_FUN, kind, reached
EXECUTE_FAILING_PRO, kind, reached
reached=2
return, 42
end
;
pro EXECUTE_FAILING_WRAPPER, kind, reached
EXECUTE_FAILING_PRO, kind, reached
reached=3
end
;
pro EXECUTE_ON_ERROR, mode, reached
ON_ERROR, mode
EXECUTE_FAILING_PRO, 4, reached
reached=4
end
;
pro EXECUTE_CATCH_ERROR, recovered
CATCH, error
if error NE 0 then begin
   CATCH, /cancel
   recovered=1
   return
endif
EXECUTE_FAILING_PRO, 4, reached
recovered=-1
end
;
pro EXECUTE_IO_ERROR, recovered
ON_IOERROR, recovered_io
value=DOUBLE('not a number')
recovered=-1
return
recovered_io:
recovered=1
end
;
pro EXECUTE_WITH_CALLER_CATCH, status, resumed
CATCH, error
if error NE 0 then begin
   CATCH, /cancel
   resumed=-1
   return
endif
status=EXECUTE('EXECUTE_FAILING_PRO, 4, reached')
resumed=1
CATCH, /cancel
end
;
pro EXECUTE_WITH_CALLER_IOERROR, status, resumed
ON_IOERROR, caller_ioerror
status=EXECUTE("value=DOUBLE('not a number')")
resumed=1
return
caller_ioerror:
resumed=-1
end
;
pro TEST_EXECUTE_NESTED, cumul_errors
errors=0
commands=['EXECUTE_FAILING_PRO, kind, reached', $
          'value=EXECUTE_FAILING_FUN(kind, reached)', $
          'EXECUTE_FAILING_WRAPPER, kind, reached', $
          "CALL_PROCEDURE, 'EXECUTE_FAILING_PRO', kind, reached", $
          "value=CALL_FUNCTION('EXECUTE_FAILING_FUN', kind, reached)"]
for kind=0,4 do begin
   for i=0,N_ELEMENTS(commands)-1 do begin
      reached=0
      after=0
      value=-1
      status=EXECUTE(commands[i]+' & after=1')
      if status NE 0 then ERRORS_ADD, errors, 'nested failure status: '+commands[i]
      if reached NE 0 then ERRORS_ADD, errors, 'continued inside failed call'
      if after NE 0 then ERRORS_ADD, errors, 'continued after failed call'
      if value NE -1 then ERRORS_ADD, errors, 'assigned result of failed function'
   endfor
endfor
;
for mode=0,3 do begin
   reached=0
   status=EXECUTE('EXECUTE_ON_ERROR, mode, reached')
   if status NE 0 then ERRORS_ADD, errors, 'ON_ERROR failure status'
   if reached NE 0 then ERRORS_ADD, errors, 'continued after ON_ERROR failure'
endfor
;
recovered=0
status=EXECUTE('EXECUTE_CATCH_ERROR, recovered')
if (status NE 1) OR (recovered NE 1) then $
   ERRORS_ADD, errors, 'CATCH in called routine'
;
recovered=0
status=EXECUTE('EXECUTE_IO_ERROR, recovered')
if (status NE 1) OR (recovered NE 1) then $
   ERRORS_ADD, errors, 'ON_IOERROR in called routine'
;
; EXECUTE handles its error before an enclosing CATCH can intercept it.
resumed=0
EXECUTE_WITH_CALLER_CATCH, status, resumed
if (status NE 0) OR (resumed NE 1) then $
   ERRORS_ADD, errors, 'CATCH in EXECUTE caller'
;
resumed=0
EXECUTE_WITH_CALLER_IOERROR, status, resumed
if (status NE 0) OR (resumed NE 1) then $
   ERRORS_ADD, errors, 'ON_IOERROR in EXECUTE caller'
;
; A failed inner EXECUTE is a normal result for its caller.
inner=-1
after=0
status=EXECUTE("inner=EXECUTE('EXECUTE_FAILING_PRO, 4, reached') & after=1")
if (status NE 1) OR (inner NE 0) OR (after NE 1) then $
   ERRORS_ADD, errors, 'nested EXECUTE isolation'
;
; An inner EXECUTE must restore the outer error-handling boundary.
after=0
status=EXECUTE("inner=EXECUTE('value=42') & EXECUTE_FAILING_WRAPPER, 4, reached & after=1")
if (status NE 0) OR (after NE 0) then $
   ERRORS_ADD, errors, 'nested EXECUTE restoration'
;
; Errors in dynamically compiled loops must restore the caller's loop state.
iterations=0
for outer=0,2 do begin
   after=0
   status=EXECUTE('for inner_loop=0,1 do EXECUTE_FAILING_PRO, 4, reached & after=1')
   if (status NE 0) OR (after NE 0) then $
      ERRORS_ADD, errors, 'failure in EXECUTE loop'
   iterations++
endfor
if iterations NE 3 then ERRORS_ADD, errors, 'caller loop state after failure'
;
; ON_IOERROR in a calling routine does not handle a callee's I/O error.
RESOLVE_ROUTINE, 'test_ioerror'
path=''
status=EXECUTE('IOERROR_SCOPE_MIDDLE, path')
if (status NE 0) OR (path NE '') then $
   ERRORS_ADD, errors, 'ON_IOERROR in calling routine'
path=''
status=EXECUTE('IOERROR_SCOPE_CATCH, path')
if (status NE 1) OR (path NE ' caught') then $
   ERRORS_ADD, errors, 'CATCH behind ON_IOERROR in calling routine'
path=''
status=EXECUTE('IOERROR_SCOPE_LOCAL, path')
if (status NE 1) OR (path NE ' local_label') then $
   ERRORS_ADD, errors, 'ON_IOERROR in originating routine'
;
; A previous error (including a handled one) must not poison later calls.
status=EXECUTE('value=42')
if (status NE 1) OR (value NE 42) then ERRORS_ADD, errors, 'success after failure'
;
BANNER_FOR_TESTSUITE, 'TEST_EXECUTE_NESTED', errors, /short
ERRORS_CUMUL, cumul_errors, errors
end
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
TEST_EXECUTE_MISSING, cumul_errors
TEST_EXECUTE_NESTED, cumul_errors
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
