;
; AC 2024 Mars 13
; quick tests for operators.
; need GIVE_LIST_NUMERIC procedure in the testsuite
;
; We don't play with negative numbers because for Int/Long & unsigned
; we will have other difficulties related to modulo
;
; --------------------------------------
;
pro PROCESS_RESULTS, operateur, type, val, expected, errors, verbose=verbose
;
if KEYWORD_SET(verbose) then print, operateur, ' ', type,  val, expected
if (TOTAL(ABS(val-expected)) GT 1e-6) then begin
   ERRORS_ADD, errors, operateur+' '+type
endif
end
; --------------------------------------
;
pro TEST_OP_DIV, cumul_errors, value, expected, verbose=verbose, test=test
;
errors=0
;
GIVE_LIST_NUMERIC,a,b,c
;
for ii=0, N_ELEMENTS(a)-1 do begin
   val=FIX(value, type=a[ii])
   val /=2
   ;;
   PROCESS_RESULTS, 'DIV', b[ii], val, expected, errors, verbose=verbose
endfor
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_OP_DIV", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------
;
pro TEST_OP_MUL, cumul_errors, value, expected, verbose=verbose, test=test
;
errors=0
;
GIVE_LIST_NUMERIC,a,b,c

for ii=0, n_elements(a)-1 do begin
   val=FIX(value, type=a[ii])
   val *=2
   PROCESS_RESULTS, 'MUL', b[ii], val, expected, errors, verbose=verbose
endfor
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_OP_MUL", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------
;
pro TEST_OP_ADD, cumul_errors, value, expected, verbose=verbose, test=test
;
errors=0
;
GIVE_LIST_NUMERIC,a,b,c
;
for ii=0, n_elements(a)-1 do begin
   val=FIX(value, type=a[ii])
   val +=2
   PROCESS_RESULTS, 'ADD', b[ii], val, expected, errors, verbose=verbose
endfor
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_OP_ADD", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------
;
pro TEST_OP_SUB, cumul_errors, value, expected, verbose=verbose, test=test
;
errors=0
;
GIVE_LIST_NUMERIC,a,b,c

for ii=0, n_elements(a)-1 do begin
   val=FIX(value, type=a[ii])
   val -=2
   PROCESS_RESULTS, 'SUB', b[ii], val, expected, errors, verbose=verbose
endfor
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_OP_SUB", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------
;
pro TEST_OPERATORS, help=help, test=test, verbose=verbose, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_TOTAL, help=help, test=test, verbose=verbose, no_exit=no_exit'
   return
endif
;
TEST_OP_ADD, cumul_errors, 10, 12., verbose=verbose
TEST_OP_ADD, cumul_errors, [10], 12., verbose=verbose
TEST_OP_ADD, cumul_errors, [10,20], [12.,22], verbose=verbose
;
TEST_OP_SUB, cumul_errors, 10, 8, verbose=verbose
TEST_OP_SUB, cumul_errors, [10], 8, verbose=verbose
TEST_OP_SUB, cumul_errors, [10,20], [8,18], verbose=verbose
;
TEST_OP_MUL, cumul_errors, 10, 20, verbose=verbose
TEST_OP_MUL, cumul_errors, [10], 20, verbose=verbose
TEST_OP_MUL, cumul_errors, [10,20], [20,40], verbose=verbose
;
TEST_OP_DIV, cumul_errors, 10, 5, verbose=verbose
TEST_OP_DIV, cumul_errors, [10], 5, verbose=verbose
TEST_OP_DIV, cumul_errors, [10,20], [5,10], verbose=verbose
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_OPERATORS', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
