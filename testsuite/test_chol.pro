;----------------------------NOT_positive_definit--------------------------------------------------------------
;------------ As the procedure woubld be failed. we need to do this test by hand.------------------------------
;
pro TEST_NOT_POSITIVE_DEFINIT
;-------------test not positive definite------------
;print,'should be failed. and P=0'
A=[[9,6],[6,0]]&CHOLDC, A, P
;-------------test demi positive definite------------
;print,'should be failed. and P=0'
A=[[9,6],[6,4]]&CHOLDC, A, P
;
;-------------test not positive definite, complex------------
;print,'should be failed. and P=0'
a11=complex(9,1)
a12=complex(6,1)
a21=complex(6,-1)
a22=complex(4,1)
a = [[a11,a12],[a21,a22]]
CHOLDC, A, P
;
end


;--------------------------------------------------------------------------------------------------------
;----------------------------COMPLEX--------------------------------------------------------------
pro TEST_CHOLDC_COMPLEX, cumul_errors, test=test, verbose=verbose
;
errors = 0
;
a11=complex(9,1)
a12=complex(6,1)
a21=complex(6,-1)
a22=complex(5,1)
A = [[a11,a12],[a21,a22]]
;---
CHOLDC, A, P
P_expected = [3.0000000,1.0000000]
if (total(abs(P-P_expected)) GT 1E-5) then begin & errors++ & print,' error CHOLDC complex!' & endif
;---
A = [[a11,a12],[a21,a22]]
lower = A
LA_CHOLDC, lower
lower_expected = [[complex(3.0000000,0.0000000),complex(6.0000000,1.0000000)],[complex(2.0000000, -0.33333334),complex( 0.94280905,0.0000000)]]
if (total(abs(lower-lower_expected)) GT 1E-5) then begin & errors++ & print,' error LA_CHOLDC complex lower!' & endif
;---
A = [[a11,a12],[a21,a22]]
upper = A
LA_CHOLDC, upper, /upper
upper_expected = [[complex(3.0000000,0.0000000),complex(2.0000000, 0.33333334)],[complex(6.00000,-1.00000),complex( 0.94280905,0.0000000)]]
if (total(abs(upper-upper_expected)) GT 1E-5) then begin & errors++ & print,' error LA_CHOLDC complex upper!' & endif
;------------------------------------------------
BANNER_FOR_TESTSUITE, "TEST_CHOLDC_COMPLEX", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
;--------------------------------------------------------------------------------------------------------
;----------------------------CHOLDC/CHOLSOL--------------------------------------------------------------
pro TEST_CHOLSOL, cumul_errors, test=test, verbose=verbose
;
errors = 0
;
A = [[ 6.0, 15.0, 55.0], [15.0, 55.0, 225.0],[55.0, 225.0, 979.0]]
B = [9.5, 50.0, 237.0]
res_expected = [-0.5,  -1.0,  0.50]
;
;----------------------NORMAL---------------------
CHOLDC, A, P
P_idl = [2.4494898, 4.1833005, 6.1100993]
;Compute the solution:
res=CHOLSOL(A, P, B)
if (total(abs(res-res_expected)) GT 1E-5) then begin & errors++ & print,' error cholsol result!' & endif
if (total(abs(P-P_idl)) GT 1E-5) then begin & errors++ & print,' error choldc P !' & endif
;
;---------------------/DOUBLE---------------------
CHOLDC, A, P, /DOUBLE
res_double=CHOLSOL(A, P, B, /DOUBLE)
if (total(abs(res_double-res_expected)) GT 1E-10) then begin & errors++ & print,' error cholsol result double !' & endif
;
;------------------------------------------------
BANNER_FOR_TESTSUITE, "TEST_CHOLSOL", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
;--------------------------------------------------------------------------------------------------------
;----------------------------LA_CHOLDC/LA_CHOLSOL--------------------------------------------------------
pro TEST_LA_CHOLSOL, cumul_errors, test=test, verbose=verbose
;
errors = 0
;
A = [[ 6.0, 15.0, 55.0], [15.0, 55.0, 225.0],[55.0, 225.0, 979.0]]
B = [9.5, 50.0, 237.0]
res_expected = [-0.5, -1.0, 0.50]
;
;---------------------NORMAL----------------------
;Compute the Cholesky decomposition of A:
ACHOL = A ; make a copy
LA_CHOLDC, ACHOL
ACHOL_idl = [[2.4494898,15.000000,55.000000],[6.1237240,4.1833005,225.00000],[22.453655,20.916502,6.1100998]]
res_la = LA_CHOLSOL(ACHOL, B)
if (total(abs(res_la-res_expected)) GT 1E-5) then begin & errors++ & print,' error la_cholsol result!' & endif
if (total(abs(ACHOL-ACHOL_idl)) GT 1E-6) then begin & errors++ & print,' error la_choldc achol !' & endif
;
;---------------------/DOUBLE---------------------
ACHOL = A ; make a copy
LA_CHOLDC, ACHOL, /DOUBLE
res_la_double = LA_CHOLSOL(ACHOL, B, /DOUBLE)
if (total(abs(res_la_double-res_expected)) GT 1E-10) then begin & errors++ & print,' error la_cholsol_double result!' & endif
;
;---------------------/UPPER----------------------
ACHOL = A ; make a copy
LA_CHOLDC, ACHOL, /UPPER
res_la_upper = LA_CHOLSOL(ACHOL, B, /UPPER)
if (total(abs(res_la_upper-res_expected)) GT 1E-5) then begin & errors++ & print,' error la_cholsol_upper result!' & endif
;
;-----------------/DOUBLE, /UPPER-----------------
ACHOL = A ; make a copy
LA_CHOLDC, ACHOL, /DOUBLE, /UPPER
res_la_double_upper = LA_CHOLSOL(ACHOL, B, /DOUBLE, /UPPER)
if (total(abs(res_la_double_upper-res_expected)) GT 1E-10) then begin & errors++ & print,' error la_cholsol_double_upper result!' & endif
;
;------------------------------------------------
BANNER_FOR_TESTSUITE, "TEST_LA_CHOLSOL", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
;--------------------------------------------------------------------------------------------------------
;--------------------------------------------------------------------------------------------------------
pro TEST_CHOL, help=help, test=test, verbose=verbose, no_exit=no_exit
;
cumul_errors=0
TEST_CHOLDC_COMPLEX, cumul_errors, test=test, verbose=verbose
TEST_CHOLSOL, cumul_errors, test=test, verbose=verbose
TEST_LA_CHOLSOL, cumul_errors, test=test, verbose=verbose
;
;----------- final message ----------
BANNER_FOR_TESTSUITE, 'TEST_CHOL', cumul_errors, short=short
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
if KEYWORD_SET(test) then STOP
;
end