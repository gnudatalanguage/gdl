;
; Thibault HUILLET, 04/08/2021
;
; NB : This test will have to be updated once the procedure got more fonctionnalities.
;

pro TEST_LA_SVD

errors = 0

A = [[ 3.0,  1.0, -4.0], $
    [ 1.0,  3.0, -4.0], $
    [-4.0, -4.0,  8.0]]

la_svd,A,W,U,V

if total(abs(W-[12,2,0])) GT 0.0001 then ERRORS_ADD, errors, "Numeric error"

if total(abs(U-[[-0.408248, 0.707107, 0.577350], [-0.408248, -0.707107, 0.577350], [0.816497, -1.19149e-16, 0.577350]])) GT 0.0001 then ERRORS_ADD, errors, "Numeric error"

if total(abs(V-[[-0.40824831, 0.70710677, -0.57735026], [-0.408248, -0.707107, -0.577350], [0.816497, 0, -0.577350]])) GT 0.0001 then ERRORS_ADD, errors, "Numeric error"

la_svd,A,W,U,V,/DOUBLE

if typename(V) NE "DOUBLE" then ERRORS_ADD, errors, "Type error"

if typename(W) NE "DOUBLE" then ERRORS_ADD, errors, "Type error"

if typename(U) NE "DOUBLE" then ERRORS_ADD, errors, "Type error" 

la_svd,A,W,U,V,STATUS=status

if status GT 0.0001 then ERRORS_ADD, errors, "Status error"

BANNER_FOR_TESTSUITE, 'TEST_TIMESTAMP', errors
if (errors GT 0) then EXIT, status=1
end   
