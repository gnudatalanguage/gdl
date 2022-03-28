;
; testsuite for the SPL_INIT and SPL_INTERP functions
; Eloi Rozier de Linage
; Mai 2010
; based on test_spl_init
;
; ----------------------------------------------------
;
; Testing the type of output of SPL_INIT() and SPL_INTERP()
; It should be FLOAT except for when keyword DOUBLE is set or one of the inputs is DOUBLE
;
pro TEST_SPL_TYPE, cumul_errors, test=test
;
errors=0
;
GIVE_LIST_NUMERIC, list_num_types, list_num_names
;
; We only test the 4 types of inputs: (BYTE INT LONG FLOAT DOUBLE)
;
for ii=0,4 do begin
   type_value=list_num_types[ii]
   type_name=list_num_names[ii]
   ;
   xin = INDGEN(5, type=type_value)
   yin = FIX(2*SIN(xin), type=type_value)
   ;
   interp1=SPL_INIT(xin, yin)
   interp2=SPL_INIT(xin, double(yin))
   interp_double=SPL_INIT(xin, yin, /double)
   ;
   xout = FIX(INDGEN(5) + 0.3, type=type_value)
   ;
   res1=spl_interp(xin, yin, interp1, xout)
   res2=spl_interp(xin, yin, interp1, double(xout))
   res_double=spl_interp(xin, yin, interp1, xout, /double)
   ;
   if type_name eq 'DOUBLE' then exp = 'DOUBLE' else exp='FLOAT'
   ;
   ; Checking type for spl_init
   if TYPENAME(interp1) ne exp then ERRORS_ADD, errors, 'SPL_INIT bad for type '+type_name
   if TYPENAME(interp2) ne 'DOUBLE' then ERRORS_ADD, errors, 'SPL_INIT bad for type '+type_name+' with double input'
   if TYPENAME(interp_double) ne 'DOUBLE' then ERRORS_ADD, errors, 'SPL_INIT bad for type '+type_name+' with /double keyword'
   ;
   ; Checking type for spl_interp
   if TYPENAME(res1) ne exp then ERRORS_ADD, errors, 'SPL_INTERP bad for type '+type_name
   if TYPENAME(res2) ne 'DOUBLE' then ERRORS_ADD, errors, 'SPL_INTERP bad for type '+type_name+' with double input'
   if TYPENAME(res_double) ne 'DOUBLE' then ERRORS_ADD, errors, 'SPL_INTERP bad for type '+type_name+' with /double keyword'
endfor
;
BANNER_FOR_TESTSUITE, 'TEST_SPL_TYPE', errors, /status
;
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------------------------------
;
; Testing SPL_INIT()
;
pro TEST_SPL_INIT, cumul_errors, test=test
;
errors=0
;
; Warning message
PRINT, '% TEST_SPL_INIT: following SPL_INIT errors are normal behaviour'
;
; Generating well known input
x=(FINDGEN(21)/20.)*2.0*!PI  
y=SIN(X)  
a=FINDGEN(5)
b=MAKE_ARRAY(7,/DOUBLE, VALUE = 1)
c=[1,2,!values.f_nan,4,5]
;
; -------------------------------
; Numerical test
;
result=SPL_INIT(X, Y, YP0 = -5.1, YPN_1 = 0.0)
exp=[67.2615,-18.3342,4.23653,-2.10966,-0.612186,-1.10115,-0.934012,-0.822361, $
    -0.590850,-0.312040,0.000107266,0.311611,0.592353,0.816782,0.954825,1.02348, $
     0.902068,1.02781,-0.198994,3.26597,-11.0260]
if (abs(total(exp-result)) ge 1E-5) then ERRORS_ADD, errors, 'SPL_INIT error Numerical Case 0'
;
; -------------------------------
; Result equal to NaN
;
;  YP0=NaN
result=SPL_INIT(X, Y, YP0 = !values.f_nan, YPN_1 = 0.0)
if (TOTAL(FINITE(result, /NaN)) NE N_ELEMENTS(result) ) then ERRORS_ADD, errors, 'SPL_INIT error Case 1'
;
; YPN_1=NaN
result=SPL_INIT(X, Y, YP0 = 0.0, YPN_1 = !values.f_nan)
if (TOTAL(FINITE(result, /NaN)) NE N_ELEMENTS(result) ) then ERRORS_ADD, errors, 'SPL_INIT error Case 2'
;
; YP0=YPN_1=NaN
result=SPL_INIT(X, Y, YP0 = !values.f_nan, YPN_1 = !values.f_nan)
if (TOTAL(FINITE(result, /NaN)) NE N_ELEMENTS(result) ) then ERRORS_ADD, errors, 'SPL_INIT error Case 3'
;
; X contains a Nan value
result=SPL_INIT(c, a) 
if (TOTAL(FINITE(result, /NaN)) NE N_ELEMENTS(result) ) then ERRORS_ADD, errors, 'SPL_INIT error Case 4'
;
; Y contains a Nan value
result=SPL_INIT(A, C) 
if (TOTAL(FINITE(result, /NaN)) NE N_ELEMENTS(result) ) then ERRORS_ADD, errors, 'SPL_INIT error Case 5'
;
; At least two consecutive X values are identical
result=SPL_INIT(B, B) 
if (TOTAL(FINITE(result, /NaN)) NE N_ELEMENTS(result) ) then ERRORS_ADD, errors, 'SPL_INIT error Case 6'
;
; -------------------------------
; When giving 0 value as keyword, sould return same results
; than without keywords
;
yy=MAKE_ARRAY(N_ELEMENTS(X),/DOUBLE, VALUE = 1)
;
kwyp0=0.0
kwyn1=0.0
;
;without key words (reference)
XY=SPL_INIT(X, yy)
if (ARRAY_EQUAL(XY,0) EQ 0 ) then ERRORS_ADD, errors, 'SPL_INIT error Case 7: SPL_INIT(X,Y) wrong result'
;
;with key word YP0
XYP=SPL_INIT(X, yy, YP0 = kwyp0)
if (ARRAY_EQUAL(XYP,0) EQ 0 ) then ERRORS_ADD, errors, 'SPL_INIT error Case 8: SPL_INIT(X,Y,YP0) wrong result'
;
;with YPN_1
XYPN=SPL_INIT(X, yy, YPN_1 = kwyn1)
if (ARRAY_EQUAL(XYPN,0) EQ 0 ) then ERRORS_ADD, errors, 'SPL_INIT error Case 9: SPL_INIT(X,Y,YPN_1) wrong result'

;with key words YP0 and YPN_1
result=SPL_INIT(X, yy, YP0 = kwyp0, YPN_1 = kwyn1)
if (ARRAY_EQUAL(result,0) EQ 0 ) then ERRORS_ADD, errors, 'SPL_INIT error Case 10: SPL_INIT(X,Y,YP0,YPN_1) wrong result'
;
; -------------------------------
; When input X and Y do not have the same number of elements ... 
;
; n_element(X) lt n_element(Y)
result=SPL_INIT(A,B) 
if (ARRAY_EQUAL(result,0) EQ 0 ) then ERRORS_ADD, errors, 'SPL_INIT error Case 11: n_element(X) lt n_element(Y)'
;
; -------------------------------
; Cases with YP0 or YPN_1 equal infinity... 
;
;with key words YP0=infinity and YPN_1 != infinity
result=SPL_INIT(X, yy, YP0 = !values.F_INFINITY, YPN_1 = kwyn1)
;
if (ARRAY_EQUAL(XYPN,result) EQ 0 ) then ERRORS_ADD, errors, 'SPL_INIT error Case 12: YP0==inf and YPN_1 != inf'
;
;with key words YP0=infinity and YPN_1 != infinity
result=SPL_INIT(X, yy, YP0 = kwyp0 , YPN_1 = !values.F_INFINITY)
;
if (ARRAY_EQUAL(XYP,result) EQ 0 ) then ERRORS_ADD, errors, 'SPL_INIT error Case 13: YP0!=inf and YPN_1 == inf'
;
; with keywords YP0=YPN_1=INFINITY
result=SPL_INIT(X, yy, YP0 = !values.f_infinity, YPN_1 = !values.f_infinity)
;
if (ARRAY_EQUAL(XY,result) EQ 0 ) then ERRORS_ADD, errors, 'SPL_INIT error Case 14: YP0==YPN_1 == inf'
;
BANNER_FOR_TESTSUITE, 'TEST_SPL_INIT', errors, /status
;
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------------------------------
;
; Testing SPL_INTERP()
;
pro TEST_SPL_INTERP, cumul_errors, test=test
;
errors=0
;
; Warning message
PRINT, 'TEST_SPL_INTERP: following SPL_INTERP errors are normal behaviour'
;
; Generating well known input
X=(FINDGEN(21)/20.)*2.0*!PI  
Y=SIN(X)  
Y2=SPL_INIT(X, Y)
X2=FINDGEN(11)/11.0*!PI
;
A=FINDGEN(5)
B2=SPL_INIT(A,A)
A2=A*0.5
D=[1,2,!values.f_nan,4,5]
E=FINDGEN(7)
;
; -------------------------------
; Numerical test
;
result=SPL_INTERP(X, Y, Y2, X2)
exp=[0.00000,0.281733,0.540638,0.755739,0.909613,0.989796,0.989796,0.909613, $
     0.755739,0.540638,0.281733]
if (abs(total(exp-result)) ge 1E-5) then ERRORS_ADD, errors, 'SPL_INTERP error Numerical Case 1'
;
; -------------------------------
; Result equal to NaN
;
; X2=NaN
result=SPL_INTERP(X, Y, Y2, !values.f_nan)
if (TOTAL(FINITE(result, /NaN)) NE N_ELEMENTS(result) ) then ERRORS_ADD, errors, 'SPL_INTERP error Case 1'
;
; X contains a Nan value
result=SPL_INTERP(D, A, B2, A2)
if (TOTAL(FINITE(result, /NaN)) NE N_ELEMENTS(result) ) then ERRORS_ADD, errors, 'SPL_INTERP error Case 2'
;
; Y contains a Nan value, first two values should be finite
result=SPL_INTERP(A, D, B2, A2)
if (TOTAL(FINITE(result, /NaN)) NE 3 ) then ERRORS_ADD, errors, 'SPL_INTERP error Case 3'
;
; -------------------------------
; Reversed input
;
result=SPL_INTERP(REVERSE(A), A, B2, A2)
exp=[0.00000,0.500000,1.00000,1.50000,2.00000]
if (ARRAY_EQUAL(result, exp) eq 1) then ERRORS_ADD, errors, 'SPL_INTERP error Case 4'
;
BANNER_FOR_TESTSUITE, 'TEST_SPL_INTERP', errors, /status
;
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------------------------------
;
pro TEST_SPL, test=test, help=help, verbose=verbose, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_SPL, test=test, help=help, verbose=verbose, no_exit=no_exit'
   return
endif
;
cumul_errors=0
;
TEST_SPL_TYPE, cumul_errors
TEST_SPL_INIT, cumul_errors
TEST_SPL_INTERP, cumul_errors
;
; ----------------- final message ----------
BANNER_FOR_TESTSUITE, 'TEST_SPL', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end