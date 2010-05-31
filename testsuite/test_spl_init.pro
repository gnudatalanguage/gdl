;
; testsuite for the SPL_INIT function
; Lea Noreskal and Alain Coulais
; Mai 2010
; under GNU GPL 2 or later
;
pro TEST_SPL_INIT, test=test, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_SPL_INIT, test=test, help=help'
   return
endif
;
; generating well known input
x=(FINDGEN(21)/20.)*2.0*!PI  
y=SIN(X)  
a=FINDGEN(5)
b=MAKE_ARRAY(7,/DOUBLE, VALUE = 1)
c=[1,2,!values.f_nan,4,5]
;
status=0
;
; -------------------------------
; Result equal to NaN
;
;  YP0=NaN
result=SPL_INIT(X, Y, YP0 = !values.f_nan, YPN_1 = 0.0)

if (TOTAL(FINITE(result, /NaN)) NE N_ELEMENTS(result) ) then begin
   MESSAGE, 'result ne NaN', /continue
   EXIT, status=1
endif
;
; YPN_1=NaN
;
result=SPL_INIT(X, Y, YP0 = 0.0, YPN_1 = !values.f_nan)
;
if (TOTAL(FINITE(result, /NaN)) NE N_ELEMENTS(result) ) then begin
   MESSAGE, 'result ne NaN', /continue
   EXIT, status=1
endif


; YP0=YPN_1=NaN
result=SPL_INIT(X, Y, YP0 = !values.f_nan, YPN_1 = !values.f_nan)

if (TOTAL(FINITE(result, /NaN)) NE N_ELEMENTS(result) ) then begin
   MESSAGE, 'result ne NaN', /continue
   EXIT, status=1
endif
;
; X contains a Nan value
;
result=SPL_INIT(c, a) 
if (TOTAL(FINITE(result, /NaN)) NE N_ELEMENTS(result) ) then begin
   MESSAGE, 'result ne NaN', /continue
   EXIT, status=1
endif

; Y contains a Nan value
result=SPL_INIT(A, C) 
if (TOTAL(FINITE(result, /NaN)) NE N_ELEMENTS(result) ) then begin
   MESSAGE, 'result ne NaN', /continue
   EXIT, status=1
endif

; At least two consecutive X values are identical
result=SPL_INIT(B, B) 
if (TOTAL(FINITE(result, /NaN)) NE N_ELEMENTS(result) ) then begin
   MESSAGE, 'result ne NaN', /continue
   EXIT, status=1
endif
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
if (ARRAY_EQUAL(XY,0) EQ 0 ) then begin
   MESSAGE, 'SPL_INIT error : SPL_INIT(X,Y) wrong result', /continue
   EXIT, status=1
endif

;with key word YP0
XYP=SPL_INIT(X, yy, YP0 = kwyp0)
if (ARRAY_EQUAL(XYP,0) EQ 0 ) then begin
   MESSAGE, 'SPL_INIT error : SPL_INIT(X,Y,YP0) wrong result', /continue
   EXIT, status=1
endif

;with YPN_1
XYPN=SPL_INIT(X, yy, YPN_1 = kwyn1)
if (ARRAY_EQUAL(XYPN,0) EQ 0 ) then begin
   MESSAGE, 'SPL_INIT error : SPL_INIT(X,Y,YPN_1) wrong result', /continue
   EXIT, status=1
endif

;with key words YP0 and YPN_1
result=SPL_INIT(X, yy, YP0 = kwyp0, YPN_1 = kwyn1)
if (ARRAY_EQUAL(result,0) EQ 0 ) then begin
   MESSAGE, 'SPL_INIT error : SPL_INIT(X,Y,YP0,YPN_1) wrong result', /continue
   EXIT, status=1
endif
;
; -------------------------------
; When input X and Y do not have the same number of elements ... 
;
; n_element(X) lt n_element(Y)
result=SPL_INIT(A,B) 
if (ARRAY_EQUAL(result,0) EQ 0 ) then begin
   MESSAGE, 'SPL_INIT error : n_element(X) lt n_element(Y)', /continue
   EXIT, status=1
endif
;
; n_element(X) gt n_element(Y)
; result undefined
result=SPL_INIT(Y,B)
; if (ARRAY_EQUAL(result,0) EQ 0 ) then begin
;MESSAGE, 'SPL_INIT error :  n_element(X) gt n_element(Y)', /continue
;EXIT, status=1
;endif
;
; -------------------------------
; Cases with YP0 or YPN_1 equal infinity... 
;
;with key words YP0=infinity and YPN_1 != infinity
result=SPL_INIT(X, yy, YP0 = !values.F_INFINITY, YPN_1 = kwyn1)
;result=SPL_INIT(X,yy,YPN_1=0.0)=XYPN?
;
if (ARRAY_EQUAL(XYPN,result) EQ 0 ) then begin
   MESSAGE, 'SPL_INIT error : YP0==inf and YPN_1 != inf', /continue
   EXIT, status=1
endif
;
;with key words YP0=infinity and YPN_1 != infinity
result=SPL_INIT(X, yy, YP0 = kwyp0 , YPN_1 = !values.F_INFINITY)
;result=SPL_INIT(X,yy,YP0=-1.1) =XYP?
;
if (ARRAY_EQUAL(XYP,result) EQ 0 ) then begin
   MESSAGE, 'SPL_INIT error : YP0!=inf and YPN_1 == inf', /continue
   EXIT, status=1
endif
;
; with keywords YP0=YPN_1=INFINITY
result=SPL_INIT(X, yy, YP0 = !values.f_infinity, YPN_1 = !values.f_infinity)
;result=SPL_INIT(X,yy) =XY?
;
if (ARRAY_EQUAL(XY,result) EQ 0 ) then begin
   MESSAGE, 'SPL_INIT error : YP0==YPN_1 == inf', /continue
   EXIT, status=1
endif
;
MESSAGE, 'No (real) problem found in TEST_SPL_INIT', /continue
;
if KEYWORD_SET(test) then STOP
;
end
