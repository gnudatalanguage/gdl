;
; Alain C.
; very preliminary tests (not really tested ...)
;
; AC 2017-03-16 during MIRI test at JPL ! obviously a basic test
; crashed :( (case A -3 ... zut ) Now also in TEST_BUG_719
;
; Since DIAG_MATRIX() don't have /Debug in IDL, we removed it.
;
pro TEST_DIAG_MATRIX_EXTRACT, cumul_errors, test=test, verbose=verbose
;
nb_errors=0
;
m3x4=REFORM(INDGEN(12),3,4)
;
res=DIAG_MATRIX(m3x4)
if ~ARRAY_EQUAL(res, [0,4, 8]) then ERRORS_ADD, nb_errors, 'case 3x4 0'
res=DIAG_MATRIX(m3x4,0)
if ~ARRAY_EQUAL(res, [0,4, 8]) then ERRORS_ADD, nb_errors, 'case 3x4 0b'
;
res=DIAG_MATRIX(m3x4, 1)
if ~ARRAY_EQUAL(res, [1,5]) then ERRORS_ADD, nb_errors, 'case 3x4 1'
res=DIAG_MATRIX(m3x4, 2)
if ~ARRAY_EQUAL(res, [2]) then ERRORS_ADD, nb_errors, 'case 3x4 2'
;
res=DIAG_MATRIX(m3x4, -1)
if ~ARRAY_EQUAL(res, [3,7,11]) then ERRORS_ADD, nb_errors, 'case 3x4 -1'
res=DIAG_MATRIX(m3x4, -2)
if ~ARRAY_EQUAL(res, [6,10]) then ERRORS_ADD, nb_errors, 'case 3x4 -2'
res=DIAG_MATRIX(m3x4, -3)
if ~ARRAY_EQUAL(res, [9]) then ERRORS_ADD, nb_errors, 'case 3x4 -3'
;
; transposed
m4x3=REFORM(INDGEN(12),4,3)
;
res=DIAG_MATRIX(m4x3)
if ~ARRAY_EQUAL(res, [0,5,10]) then ERRORS_ADD, nb_errors, 'case 4x3 0'
res=DIAG_MATRIX(m4x3,0)
if ~ARRAY_EQUAL(res, [0,5,10]) then ERRORS_ADD, nb_errors, 'case 4x3 0b'
;
res=DIAG_MATRIX(m4x3, 1)
if ~ARRAY_EQUAL(res, [1,6,11]) then ERRORS_ADD, nb_errors, 'case 4x3 1'
res=DIAG_MATRIX(m4x3, 2)
if ~ARRAY_EQUAL(res, [2,7]) then ERRORS_ADD, nb_errors, 'case 4x3 2'
res=DIAG_MATRIX(m4x3, 3)
if ~ARRAY_EQUAL(res, [3]) then ERRORS_ADD, nb_errors, 'case 4x3 3'
;
res=DIAG_MATRIX(m4x3, -1)
if ~ARRAY_EQUAL(res, [4,9]) then ERRORS_ADD, nb_errors, 'case 4x3 -1'
res=DIAG_MATRIX(m4x3, -2)
if ~ARRAY_EQUAL(res, [8]) then ERRORS_ADD, nb_errors, 'case 4x3 -2'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_DIAG_MATRIX_EXTRACT', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
;
pro TEST_DIAG_MATRIX_INSERT, cumul_errors, test=test, verbose=verbose
;
nb_errors=0
;
diag0=10+FINDGEN(5)
diag1=10+FINDGEN(4)
expected_size=[2,5,5,4,25]
;
; adding in the major diag.
a0=DIAG_MATRIX(diag0)
a1p=DIAG_MATRIX(REVERSE(diag1), 1)
a1m=DIAG_MATRIX(-REVERSE(diag1), -1)
;
if ~ARRAY_EQUAL(SIZE(a0),expected_size) then ERRORS_ADD, nb_errors, 'case A0'
if ~ARRAY_EQUAL(SIZE(a1p),expected_size) then ERRORS_ADD, nb_errors, 'case A1p'
if ~ARRAY_EQUAL(SIZE(a1m),expected_size) then ERRORS_ADD, nb_errors, 'case A1m'
;
exp=[0,13,12,11,10] & exp=-1.*(exp^2)
if ~ARRAY_EQUAL(DIAG_MATRIX(a1p#a1m),exp) then ERRORS_ADD, nb_errors, 'case p#m'
exp=SHIFT(exp,-1)
if ~ARRAY_EQUAL(DIAG_MATRIX(a1m#a1p),exp) then ERRORS_ADD, nb_errors, 'case m#p'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_DIAG_MATRIX_INSERT', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
;
pro TEST_DIAG_MATRIX, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_DIAG_MATRIX, help=help, verbose=verbose, $'
   print, '                      no_exit=no_exit, test=test'
   return
endif
;
TEST_DIAG_MATRIX_EXTRACT, nb_errors, test=test, verbose=verbose
;
TEST_DIAG_MATRIX_INSERT, nb_errors, test=test, verbose=verbose
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_DIAG_MATRIX', nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

