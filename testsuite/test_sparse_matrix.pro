;
; GD 2023-Dec
;
; no a test, really. Just a suggestion for test, plus the fact
; that it exercises the coverage.
;
; ----------------------------------------------------
; Modifications history :
;
; 2024-Jan-26 : AC. Return 77 if no Eigen3.
;
; ----------------------------------------------------
;
pro TEST_SPARSE_MATRIX_MULTIPLY, cumul_errors, test=test, verbose=verbose
;
nb_errors=0
tol=1e-6
;
a = [[ 2.0,  1.0,  1.0], $
     [ 4.0, -6.0,  0.0], $
     [-2.0,  7.0,  2.0]]
;
z=SPRSIN(a, thresh=0.5)
zz=SPRSTP(z)
q=FULSTR(z)
;
if (ABS(TOTAL(a-q)) gt tol) then ERRORS_ADD, nb_errors, 'Relicat 1 is too large'
;;
res=SPRSAB(z,zz)
result=FULSTR(res)
;
if (ABS(TOTAL(result)-29.) GT tol) then $
   ERRORS_ADD, nb_errors, 'Relicat 2 is too large'
if (ABS(TOTAL(sprsax(z,[1,1,1]))-9.) GT tol) then $
   ERRORS_ADD, nb_errors, 'Relicat 3 is too large'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_SPARSE_MATRIX_MULTIPLY', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ----------------------------------------------------
; 
pro TEST_SPARSE_MATRIX_SOLVE, cumul_errors, test=test, verbose=verbose
;
nb_errors=0
tol=1e-6
;
a = [[ 2.0,  1.0,  1.0], $
     [ 4.0, -6.0,  0.0], $
     [-2.0,  7.0,  2.0]]
  
aludc=a
LUDC, aludc, index
b = [3,-8.0,10]
x = LUSOL(aludc, index, b)
r= LINBCG(SPRSIN(a), B, X)
;
if (ABS(TOTAL(r-x)) GT tol) then $
   ERRORS_ADD, nb_errors, 'Relicat Mat. Solve is too large'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_SPARSE_MATRIX_SOLVE', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ----------------------------------------------------
;
pro TEST_SPARSE_MATRIX, help=help, test=test, no_exit=no_exit, verbose=verbose
;
FORWARD_FUNCTION EIGEN_EXISTS
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_SPARSE_MATRIX, help=help, test=test, $'
   print, '                        no_exit=no_exit, verbose=verbose'
   return
endif
;
DEFSYSV, '!gdl', exists=is_it_gdl
if (is_it_gdl) then begin 
   if ~EIGEN_EXISTS() then begin
      MESSAGE, /continue, 'This test cannot be run because'
      MESSAGE, /continue, 'GDL was compiled without EIGEN3 support'
      EXIT, status=77
   endif
endif
;
cumul_errors=0
;
TEST_SPARSE_MATRIX_MULTIPLY, cumul_errors, test=test, verbose=verbose
TEST_SPARSE_MATRIX_SOLVE, cumul_errors, test=test, verbose=verbose
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_SPARSE_MATRIX', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;

end
