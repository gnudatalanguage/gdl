;
; testin INTERPOL on few basic cases ...
;
; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
;
; Extended by Alain Coulais on March 5, 2012
;
pro TEST_INTERPOL, test=test, quiet=quiet, help=help
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_INTERPOL, test=test, quiet=quiet, help=help'
    return
endif
;
error_level=1e-6
nb_errors=0
;
; test data - a parabola probed at four points
x = [0.,1.,2.,3.]
y = x*x
;
; 2-parameter case, linear interpolation, sanity check
;
if ~ARRAY_EQUAL(y, INTERPOL(y, 4)) then begin
    MESSAGE, 'ERROR: 2p, linear', /continue
    nb_errors=nb_errors+1
endif else begin
    if ~KEYWORD_SET(quiet) then MESSAGE, 'SUCCESS: 2p, linear:', /continue
endelse

; 2-parameter case, linear vs. spline interpolation
;
wh = WHERE(INTERPOL(y, 7, /spline) gt INTERPOL(y, 7), cnt)
if (cnt NE 0) then begin
    MESSAGE, 'ERROR: 2p, spline', /continue
    nb_errors=nb_errors+1
endif else begin
    if ~KEYWORD_SET(quiet) then MESSAGE, 'SUCCESS: 2p, spline', /continue
endelse

; 3-parameter case, linear vs. spline interpolation
;
mid = [0.5,1.5,2.5]
wh = WHERE(INTERPOL(y, x, mid, /spline) gt INTERPOL(y, x, mid), cnt)
if (cnt NE 0) then begin
    MESSAGE, 'ERROR: 3p, spline', /conti
    nb_errors=nb_errors+1
endif else begin
    if ~KEYWORD_SET(quiet) then MESSAGE, 'SUCCESS: 3p, spline', /continue
endelse
;
; ensuring INTERPOL(/spline) returns the same as spl_interp() 
; (intentionally swapping x,y with y,x)
;
res_interpol=INTERPOL(x, y, mid, /spline)
res_spline=SPL_INTERP(y, x, SPL_INIT(y, x), mid)
if ~ARRAY_EQUAL(res_interpol,res_spline) then begin
    MESSAGE, 'ERROR: INTERPOL(/spline) != spl_interp', /continue
    nb_errors=nb_errors+1
endif else begin
    if ~KEYWORD_SET(quiet) then MESSAGE, 'SUCCESS: INTERPOL(/spline) != spl_interp', /continue
endelse
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
if (MAX(ABS(res_interpol)) GT error_level) then begin
    MESSAGE, 'ERROR: extrapol. flat linear', /continue
    nb_errors=nb_errors+1
endif else begin
    if ~KEYWORD_SET(quiet) then MESSAGE, 'SUCCESS: extrapol. flat linear', /continue
endelse
;
if (MAX(ABS(res_spline)) GT error_level) then begin
    MESSAGE, 'ERROR: extrapol. flat spline', /continue
    nb_errors=nb_errors+1
endif else begin
    if ~KEYWORD_SET(quiet) then MESSAGE, 'SUCCESS: extrapol. flat spline', /continue
endelse
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
if (MAX(ABS(res_interpol-y_new)) GT error_level) then begin
    MESSAGE, 'ERROR: extrapol. linear linear', /continue
    nb_errors=nb_errors+1
endif else begin
    if ~KEYWORD_SET(quiet) then MESSAGE, 'SUCCESS: extrapol. linear linear', /continue
endelse
;
if (MAX(ABS(res_spline-y_new)) GT error_level) then begin
    MESSAGE, 'ERROR: extrapol. line w. spline', /continue
    nb_errors=nb_errors+1
endif else begin
    if ~KEYWORD_SET(quiet) then MESSAGE, 'SUCCESS: extrapol. line w. spline', /continue
endelse
;
; ------------------- final errors count ------------------
;
if (nb_errors GT 0) then begin
    MESSAGE, /continue, STRING(nb_errors)+' Errors founded'
    if ~KEYWORD_SET(test) then EXIT, status=1
endif else begin
    MESSAGE, /continue, 'No Errors founded'
endelse
;
if KEYWORD_SET(test) then STOP    
;
end
