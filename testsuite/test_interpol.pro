;
; testin INTERPOL on few basic cases ...
;
; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
;
; Extended by Alain Coulais on March 5, 2012
; Extended and updated by Eloi Rozier de Linage on May 28, 2021
;
; ----------------------------------------------------
;
; Testing the type of output of INTERPOL()
; It should be FLOAT except for when one of the inputs is DOUBLE
;
pro TEST_INTERPOL_TYPE, cumul_errors, test=test
;
errors=0
;
GIVE_LIST_NUMERIC, list_num_types, list_num_names
;
; We only test the 5 types of inputs: (BYTE INT LONG FLOAT DOUBLE)
;
for ii=0,4 do begin
   type_value=list_num_types[ii]
   type_name=list_num_names[ii]
   ;
   x = INDGEN(10, type=type_value)
   y = FIX(2*SIN(x), type=type_value)
   xinterp = FIX(INDGEN(5) + 0.3, type=type_value)
   ;
   ; With 3 args -- interpol(Y,X,XOUT)
   res1=interpol(y, x, xinterp)
   res2=interpol(y, double(x), xinterp)
   res3=interpol(double(y), x, xinterp)
   ; With 2 args -- interpol(V,N)
   res4=interpol(y,1)
   ;
   if type_name eq 'DOUBLE' then exp = 'DOUBLE' else exp='FLOAT'
   ;
   if TYPENAME(res1) ne exp then ERRORS_ADD, errors, 'INTERPOL bad for type '+type_name
   if TYPENAME(res2) ne 'DOUBLE' then ERRORS_ADD, errors, 'INTERPOL bad for type '+type_name+' with X as double'
   if TYPENAME(res3) ne 'DOUBLE' then ERRORS_ADD, errors, 'INTERPOL bad for type '+type_name+' with Y as double'
   if TYPENAME(res4) ne exp then ERRORS_ADD, errors, 'INTERPOL bad for type '+type_name+' with two arguments'
endfor
;
BANNER_FOR_TESTSUITE, 'TEST_INTERPOL_TYPE', errors, /status
;
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------------------------------
;
; Testing INTERPOL()
;
;
pro TEST_INTERPOL, test=test, quiet=quiet, help=help, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_INTERPOL, test=test, quiet=quiet, help=help, no_exit=no_exit'
    return
endif
;
cumul_errors=0
;
TEST_INTERPOL_TYPE, cumul_errors
;
error_level=1e-6
errors=0
;
; --- Multiple tests
;
; test data - a parabola probed at four points
x = [0.,1.,2.,3.]
y = x*x
;
; 2-parameter case, linear interpolation, sanity check
;
if ~ARRAY_EQUAL(y, INTERPOL(y, 4)) then ERRORS_ADD, errors, 'ERROR: 2p, linear' $
else if ~KEYWORD_SET(quiet) then MESSAGE, 'SUCCESS: 2p, linear:', /continue
;
; 2-parameter case, linear vs. spline interpolation
;
wh = WHERE(INTERPOL(y, 7, /spline) gt INTERPOL(y, 7), cnt)
if (cnt NE 0) then ERRORS_ADD, errors, 'ERROR: 2p, spline' $
else if ~KEYWORD_SET(quiet) then MESSAGE, 'SUCCESS: 2p, spline', /continue
;
; 2-parameter case, 1 output value, check if NaN (corrected bug)
;
res=INTERPOL(y, 1)
if (finite(res) EQ 0) then ERRORS_ADD, errors, 'ERROR: 2p, 1 output, NaN' $
else if ~KEYWORD_SET(quiet) then MESSAGE, 'SUCCESS: 2p, 1 output, NaN', /continue
;
; 3-parameter case, linear vs. spline interpolation
;
mid = [0.5,1.5,2.5]
wh = WHERE(INTERPOL(y, x, mid, /spline) gt INTERPOL(y, x, mid), cnt)
if (cnt NE 0) then ERRORS_ADD, errors, 'ERROR: 3p, spline' $
else if ~KEYWORD_SET(quiet) then MESSAGE, 'SUCCESS: 3p, spline', /continue
;
; ensuring INTERPOL(/spline) returns the same as spl_interp() 
; (intentionally swapping x,y with y,x)
;
res_interpol=INTERPOL(x, y, mid, /spline)
res_spline=SPL_INTERP(y, x, SPL_INIT(y, x), mid)
if ~ARRAY_EQUAL(res_interpol,res_spline) then ERRORS_ADD, errors, 'ERROR: INTERPOL(/spline) != spl_interp' $
else if ~KEYWORD_SET(quiet) then MESSAGE, 'SUCCESS: INTERPOL(/spline) != spl_interp', /continue
;
; testing computation outside input range
;
; flat case (very basic case)
;
yy=REPLICATE(2.,N_ELEMENTS(x))
zz=[-10,-5,x, 5, 10]
;
res_interpol=(INTERPOL(yy, x, zz)-2.)
res_spline=(INTERPOL(yy, x, zz, /spline)-2)
;
if (MAX(ABS(res_interpol)) GT error_level) then ERRORS_ADD, errors, 'ERROR: extrapol. flat linear', /continue $
else if ~KEYWORD_SET(quiet) then MESSAGE, 'SUCCESS: extrapol. flat linear', /continue
;
if (MAX(ABS(res_spline)) GT error_level) then ERRORS_ADD, errors, 'ERROR: extrapol. flat spline', /continue $
else if ~KEYWORD_SET(quiet) then MESSAGE, 'SUCCESS: extrapol. flat spline', /continue
;
; linear case (basic case)
;
nbp=11
x=FINDGEN(nbp)-nbp/2.
y=x
;
nbp=21
x_new=1.1*(FINDGEN(nbp)-nbp/2.)
y_new=x_new
;
res_interpol=INTERPOL(y, x, x_new)
res_spline=INTERPOL(y*1.d, x, x_new, /spline)  ;; we compute in Double, IDL pb
;
if (MAX(ABS(res_interpol-y_new)) GT error_level) then ERRORS_ADD, errors, 'ERROR: extrapol. linear linear', /continue $
else if ~KEYWORD_SET(quiet) then MESSAGE, 'SUCCESS: extrapol. linear linear', /continue
;
if (MAX(ABS(res_spline-y_new)) GT error_level) then ERRORS_ADD, errors, 'ERROR: extrapol. line w. spline', /continue $
else if ~KEYWORD_SET(quiet) then MESSAGE, 'SUCCESS: extrapol. line w. spline', /continue
;
; ------------------- final message ------------------
ERRORS_CUMUL, cumul_errors, errors
;
BANNER_FOR_TESTSUITE, 'TEST_INTERPOL', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP    
;
end