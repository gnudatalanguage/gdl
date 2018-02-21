;
; Alain C., 1er/10/2010
; Simple way to test if MPFIT is working ...
; Examples derived from Craig Markwardt WEB site
;
; AC 26/10/2010: better management of error when CMSV lib. or MPFIT
; lib. are not in the path ...
;
; 16-Nov-2011
; G. Duvert submitted a patch when we have several occurences
; of MPFIT files in the GDL_PATH !
;
; this file is under GNU GPL v2 or later
;
; ---------------------------------------
; Modifications history :
;
; - 2018-FEB-19 : AC. plotting; no more tests on CMSV for GDL ...
;
; ---------------------------------------
;
function PARABOLE, x, params
;
return, params[0]*x*x+params[1]*x+params[2]
;
end
;
; ---------------------------------------
;
pro OTHER_TEST_MPFIT, cumul_errors, kgauss1=kgauss1, kpoly1=kpoly1, $
                      nbiter=nbiter, display=display, $
                      help=help, test=test, quiet=quiet, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro OTHER_TEST_MPFIT, cumul_errors, kgauss1=kgauss1, kpoly1=kpoly1, $'
   print, '                      nbiter=nbiter, display=display, $'
   print, '                      help=help, test=test, quiet=quiet, verbose=verbose'
   return
endif
;
if ~KEYWORD_SET(verbose) then quiet=1
;
nb_errors=0
npts = 200
;
; Independent variable 
x  = (DINDGEN(npts)-(npts/2)) * 0.1d ;5 ;1 - 10.
;
; computing "Ideal" Y variable
;
if KEYWORD_SET(kgauss1) then begin
   ;; GAUSS1 is provided by MPfit lib.
   expected=[2.2D, 1.4, 3000.]
   yi = GAUSS1(x, expected) ; "Ideal" Y variable
   name='GAUSS1'
   WINDOW, 1, title=name
endif
;
if KEYWORD_SET(kpoly1) then begin
   expected=[1,1,-10.]
   yi = PARABOLE(x, expected)
   name='PARABOLE'
   WINDOW, 2, title=name
endif
;
y  = yi + RANDOMN(seed, npts) * SQRT(300. + yi); Measured, w/ noise
sy = SQRT(300.D + y)                           ; Poisson errors
;
; Now fit a Gaussian to see how well we can recover
p0 = [2.D, 1., 10.]                   ; Initial guess (cent, width, area)
p = MPFITFUN(name, x, y, sy, p0, nbiter=nbiter, quiet=quiet)    ; Fit a function
;
yfit=CALL_FUNCTION(name, x, p)

std1=STDDEV(yfit-y)
std2=STDDEV(yfit-yi)

if KEYWORD_SET(verbose) then begin
   print, 'expected : ', expected
   print, 'computed : ', p
   print, std1, std2
endif
;
; AC 2018-feb-20 : no idea for a better criteria ...
; help and feedback welcome
;
; not OK : we accept relative 2% error on each value ...
; erreur=TOTAL(ABS((p-expected)/p*100.))
;print, erreur
if 4*std2 GT std1 then ERRORS_ADD, nb_errors, 'excessive error for '+name
;
if KEYWORD_SET(display) then begin
   PLOT, x, y
   yfit=CALL_FUNCTION(name, x, p)
   OPLOT, x, yfit, thick=2, col='00ff00'x
endif
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'OTHER_TEST_MPFIT', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
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
; ---------------------------------------
;
pro TEST_MPFIT_ON_GAUSS1D, cumul_errors, path=path, display=display, $
                           verbose=verbose, test=test, debug=debug, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_MPFIT_ON_GAUSS1D, cumul_errors, path=path, display=display, $'
   print, '                           verbose=verbose, test=test, debug=debug, help=help'
   return
endif
;
MESSAGE, /continue, 'Base on Craig Markwardt example, see:'
MESSAGE, /continue, 'http://www.physics.wisc.edu/~craigm/idl/mpfittut.html'
;
; We are checking if MPFIT Lib. is in the !PATH
;
paths=STRSPLIT(!PATH, PATH_SEP(/SEARCH_PATH), /EXTRACT)
mpfit_path=FILE_SEARCH(paths+'/mpfit.pro', count=nfiles)
if (nfiles EQ 0) then MESSAGE, 'MPfit Lib. not in PATH ...'
if (nfiles GT 1) then MESSAGE, /cont, 'multiple occurence of MPfit Lib., first used'
mpfit_path=mpfit_path[0]
;
datafile=FILE_SEARCH(paths+'/fakedata.sav', count=nfiles)
if (nfiles EQ 0) then MESSAGE, '<<fakedata.sav>> file missing ...'
if (nfiles GT 1) then MESSAGE, /cont, 'multiple occurence of <<fakedata.sav>> file, first used'
datafile=datafile[0]
;
; reading back the data
RESTORE, datafile
;
start = [950.D, 2.5, 1., 1000.]
expr = 'P[0] + GAUSS1(X, P[1:3])'
;
expected_result=[997.61864, 2.1550703, 1.4488421, 3040.2411]
;
if KEYWORD_SET(debug) then STOP
;
result1 = MPFITEXPR(expr,     t, r, rerr, start, /quiet)
result2 = MPFITFUN('MYGAUSS', t, r, rerr, start, /quiet)
;
nb_errors=0
tolerance=1e-4
;
if (TOTAL((expected_result-result1)^2) GT tolerance) then begin
   ERRORS_ADD, nb_errors, 'Bad result for way : MPFITEXPR'
   if KEYWORD_SET(verbose) then begin
      print, 'Expected: ', expected_result
      print, 'Computed: ', result1
      MESSAGE, /continue, 'Bad result for way : MPFITEXPR'
   endif
endif
if (TOTAL((expected_result-result2)^2) GT 1.e-4) then begin
   ERRORS_ADD, nb_errors, 'Bad result for way : MPFITFUN'
   if KEYWORD_SET(verbose) then begin
      print, 'Expected: ', expected_result
      print, 'Computed: ', result2
      MESSAGE, /continue, 'Bad result for way : MPFITFUN'
   endif
endif
;
if KEYWORD_SET(verbose) then begin
   print, 'Expected            : ', expected_result
   print, 'Computed (MPFITEXPR): ', result1
   print, 'Computed (MPFITFUN) : ', result2
endif
;
if KEYWORD_SET(display) then begin
   WINDOW, 0, title='MPfit Lib. reference'
   PLOT, t, r,/ynozero
   yfit=CALL_FUNCTION('MYGAUSS', t, expected_result)
   OPLOT, t, yfit, thick=2, col='00ff00'x
   OPLOT, t, MYGAUSS(t, result1), thick=2, col='ff0000'x
   OPLOT, t, MYGAUSS(t, result2), thick=2, col='0000ff'x
endif
;
; ----- final ----
;
if (nb_errors EQ 0) then MESSAGE, /continue, 'MPFITEXPR and MPFITFUN well computed'
BANNER_FOR_TESTSUITE, 'TEST_MPFIT_ON_GAUSS1D', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ------------------- Launcher ------------------
;
pro TEST_MPFIT, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_MPFIT, help=help, verbose=verbose, no_exit=no_exit, test=test'
   return
endif
;
CHECK_SAVE_RESTORE, verbose=verbose
if (!SAVE_RESTORE NE 1) then begin
    MESSAGE, /CONTINUE, "No SAVE/RESTORE capabilities detected"
    MESSAGE, /CONTINUE, "please re-run with /verbose for help."
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
cumul_errors=0
;
OTHER_TEST_MPFIT, cumul_errors, /kgauss, /display, verbose=verbose
OTHER_TEST_MPFIT, cumul_errors, /kpoly, /display, verbose=verbose
;
TEST_MPFIT_ON_GAUSS1D, cumul_errors, /display, verbose=verbose
;
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_MPFIT', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

