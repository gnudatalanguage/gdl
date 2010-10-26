;
; Alain C., 1er/10/2010
; Simple way to test if MPFIT is working ...
; Examples derived from Craig Markwardt WEB site
;
; AC 26/10/2010: better managment of error when CMSV lib. or MPFIT
; lib. are not in the path ...
;
function PARABOLE, x, params
;
return, params[2]*x*params[1]*x+x+params[0]
;
end
;
pro  OTHER_TEST_MPFIT, kgauss1=kgauss1, kpoly1=kpoly1, $
                       nbiter=nbiter, display=display
;
npts = 200
x  = dindgen(npts) * 0.1 - 10.                  ; Independent variable 
; "Ideal" Y variable

if keyword_set(kgauss1) then begin
    ;; GAUSS1 is provided by MPfit lib.
    yi = gauss1(x, [2.2D, 1.4, 3000.]) ; "Ideal" Y variable
    name='GAUSS1'
endif
if keyword_set(kpoly1) then begin
    yi = PARABOLE(x, [1,1,-2])
    name='PARABOLE'
endif
y  = yi + randomn(seed, npts) * sqrt(1000. + yi); Measured, w/ noise
sy = sqrt(1000.D + y)                           ; Poisson errors
;
; Now fit a Gaussian to see how well we can recover
p0 = [1.D, 1., 1000.]                   ; Initial guess (cent, width, area)
p = MPFITFUN(name, x, y, sy, p0, nbiter=nbiter)    ; Fit a function
print, p
;
if KEYWORD_SET(display) then begin
   PLOT, x, y
   yfit=CALL_FUNCTION(name, x, p)
   OPLOT, x, yfit, thick=2
endif
;
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------------------------------
; this is a derivated work from
; http://www.physics.wisc.edu/~craigm/idl/mpfittut.html
;
function MYGAUSS, X, P
  return, P[0] + GAUSS1(X, P[1:3])
end
;
pro TEST_MPFIT_ON_GAUSS1D, path=path, error=error, $
                           test=test, verbose=verbose, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_MPFIT_ON_GAUSS1D, path=path, error=error, $'
   print, '                           test=test, verbose=verbose, help=help'
   return
endif
;
MESSAGE, /continue, 'Base on Craig Markwardt example, see:'
MESSAGE, /continue, 'http://www.physics.wisc.edu/~craigm/idl/mpfittut.html'
;
; We are checking if MPFIT Lib. is in the !PATH
;
paths=STRSPLIT(!PATH, PATH_SEP(/SEARCH_PATH), /EXTRACT)
mpfit_path=FILE_SEARCH(paths+'/mpfit.pro')
if STRLEN(mpfit_path) EQ 0 then MESSAGE, 'MPfit Lib. not in PATH ...'
;
datafile=FILE_SEARCH(paths+'/fakedata.sav')
if STRLEN(datafile) EQ 0 then MESSAGE, '<<fakedata.sav>> file missing ...'
;
; reading back the data
RESTORE, datafile
;
start = [950.D, 2.5, 1., 1000.]
expr = 'P[0] + GAUSS1(X, P[1:3])'
;
expected_result=[997.61864, 2.1550703, 1.4488421, 3040.2411]
;
result1 = MPFITEXPR(expr,     t, r, rerr, start, /quiet)
result2 = MPFITFUN('MYGAUSS', t, r, rerr, start, /quiet)
;
error=0
if (TOTAL((expected_result-result1)^2) GT 1.e-4) then begin
   error=1
   print, 'Expected: ', expected_result
   print, 'Computed: ', result1
   MESSAGE, /continue, 'Bad result for way : MPFITEXPR'
endif
if (TOTAL((expected_result-result2)^2) GT 1.e-4) then begin
   error=error+1
   print, 'Expected: ', expected_result
   print, 'Computed: ', result2
   MESSAGE, /continue, 'Bad result for way : MPFITFUN'
endif
;
if KEYWORD_SET(verbose) then begin
   print, 'Expected            : ', expected_result
   print, 'Computed (MPFITEXPR): ', result1
   print, 'Computed (MPFITFUN) : ', result2
endif
;
if (error EQ 0) then MESSAGE, /continue, 'MPFITEXPR and MPFITFUN well computed'
;
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------- Launcher ------------------
;
pro TEST_MPFIT, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_MPFIT, help=help'
   return
endif
;
if (EXECUTE('res=CMSVLIB(/QUERY)') EQ 0) then begin
    MESSAGE, /CONTINUE, "Missing CMSVlib in your GDL_PATH or IDL_PATH"
    MESSAGE, /CONTINUE, "please read HELP or RESTORE header for help."
    EXIT, status=77
endif
;
if (EXECUTE('res=GAUSS1([1], [1,1,1])') EQ 0) then begin
    MESSAGE, /CONTINUE, "Missing GAUSS1 in your GDL_PATH or IDL_PATH"
    MESSAGE, /CONTINUE, "please add MPfit lib (http://www.physics.wisc.edu/~craigm/idl/fitting.html)"
    EXIT, status=77
endif
;
if (EXECUTE('res=MPFIT()') EQ 0) then begin
    MESSAGE, /CONTINUE, "Missing MPFIT in your GDL_PATH or IDL_PATH"
    MESSAGE, /CONTINUE, "please add MPfit lib (http://www.physics.wisc.edu/~craigm/idl/fitting.html)"
    EXIT, status=77
endif
;
error=0
TEST_MPFIT_ON_GAUSS1D, error=error
;
if (error GT 0) then EXIT, status=1
;
end
