;
; Alain C., Mars 2022.
; Under GNU GPL v2 or later
;
; TO BE EXPAND !! help/advice/tests cases needed !
;
; ---------------------------------------
; Modifications history :
;
; - 2022-MAR-27 : AC. Creation. Only one occurence of LA_TRIRED
; in the testsuite (test_bug_2876372)
;
; ---------------------------------------
;
; This code is an extension of historical "test_bug_2876372.pro"
; Please note that LA_TRIRED has the specifies that the 3 inputs
; will be modified in the code : they should be writable !
; 
pro TEST_LA_TRIRED_CHECK, cumul_errors, test=test, verbose=verbose
;
errors=0
;
; all should fail with various messages but not crash GDL
in=DIST(2)
;
if EXECUTE("LA_TRIRED, 1, 1, 1") then ERRORS_ADD, errors, 'case 1,1,1'
if EXECUTE("LA_TRIRED, [1], 1, 1") then ERRORS_ADD, errors, 'case 2D'
if EXECUTE("LA_TRIRED, DIST(2), 1, 1") then ERRORS_ADD, errors, 'case Writable 1'
if EXECUTE("LA_TRIRED, in, a, 1") then ERRORS_ADD, errors, 'case Writable 2'
if EXECUTE("LA_TRIRED, in, 1, b") then ERRORS_ADD, errors, 'case Writable 3'
;
if EXECUTE("LA_TRIRED, dist(2), a, b") then ERRORS_ADD, errors, 'case Writable'
;
; -------------------
;
BANNER_FOR_TESTSUITE, 'pro TEST_LA_TRIRED_CHECK', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; -----------------------------------------------------------------
;
pro TEST_LA_TRIRED, help=help, test=test, verbose=verbose, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_LA_TRIRED, help=help, test=test, verbose=verbose, no_exit=no_exit'
   return
endif 
;
cumul_errors=0
;
TEST_LA_TRIRED_CHECK, cumul_errors, test=test, verbose=verbose
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_LA_TRIRED', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
