;
; under GNU GPL v2 or later
; Benjamin Laurent, 18-June-2012
; long term contact : Alain Coulais
;
; 06-July-2012: we change Func1 to a better one !
; 
; -------------------------------------------------
; Modifications history :
;
; 2019-Feb-14 : AC. in fact, the implemented version of FX_ROOT
; was not able to invert Ricati equation as used in a simulator
; for Astro-F FTS (the so called Fouks-Schubert model).
; I re-implemented the fx_root and clean-up the test
;
; -------------------------------------------------
;
; AC 06/07/2012 the example in IDL doc is stupid !
function FUNC1_STUPID, X  
  return, EXP(SIN(X)^2 + COS(X)^2 - 1) - 1.
end
; alternative example found in Matlab Tuto
function FUNC1, X  
  return, COS(x)-x
end
;
; from GDL doc
function FUNC2, X
  return, X^2-5.
end
;
; from http://objectmix.com/idl-pvwave/169016-better-root-finder.html
; three roots, depending the range where we are computing
function FUNC3, x
return, x^3-8.0
end
;
; -------------------------------------------------
;
pro TEST_FX_ROOT_ON_FUNC, cumul_errors, init, expected, $
                          iter=iter, eps=eps, tolerance=tolerance, $
                          function_name=function_name, $
                          verbose=verbose, help=help, test=test
;
name=ROUTINE_NAME()
if KEYWORD_SET(help) then begin
    print, 'pro '+name+' , cumul_errors, init, expected, iter=iter, $'
    print, '           eps=eps, function_name=function_name, $'
    print, '           verbose=verbose, help=help, test=test'
    return
endif
;
;Error tolerance
if (N_ELEMENTS(eps) EQ 0) then eps=1e-4
;
if N_ELEMENTS(function_name) EQ 0 then function_name='FUNC1'
;
;return one root
resuFX=FX_ROOT(init,function_name, ITMAX=iter, /DOUBLE,STOP=1, TOL=tolerance)
resuNW=NEWTON(4.,function_name) 
;
if KEYWORD_SET(verbose) then begin
   print, 'Tol., Eps, iter, init : ', tolerance, eps, iter, init
   print, 'expected : ', expected
   print, 'FX_ROOT : ',  resuFX
   print, 'FX_NEWTON : ', resuNW
endif
;
; comparing
;
nb_errors=0
;
if ABS(CALL_FUNCTION(function_name,resuFX)) GT eps then $
   ERRORS_ADD, nb_errors, 'bad direct calculation'
;
if ABS(resuFX-resuNW) GT eps then $
   ERRORS_ADD, nb_errors, 'bad comparison with Newton method'
;
if ABS(resuFX-expected) GT eps then $
   ERRORS_ADD, nb_errors, 'bad comparison with expected value'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, name, nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------------
;
; depending the [x0,x1,x2] values, we can converge to different roots
;
pro WHERE_DO_WE_CONVERGE, cumul_errors, test=test, verbose=verbose
;
name=ROUTINE_NAME()
;
tol=1.e-5
expected_module=2.
re_r1=-1
;
root1=FX_ROOT([-10,0,.5], 'FUNC3')
root2=FX_ROOT([0,.5,3], 'FUNC3')
;
nb_errors=0
; checking module values
if ABS(ABS(root1)-expected_module) GT tol then $
   ERRORS_ADD, nb_errors, 'root1 inaccurate'
;
if ABS(ABS(root2)-expected_module) GT tol then $
   ERRORS_ADD, nb_errors, 'root2 inaccurate'
;
; checking roots values
if ABS(real_part(root1)-re_r1) GT tol then $
   ERRORS_ADD, nb_errors, 'unexpected root1'
;
if ABS(real_part(root2)-expected_module) GT tol then $
   ERRORS_ADD, nb_errors, 'unexpected root21'
;
if nb_errors EQ 0 then $
   MESSAGE, /continue, 'convergence on expected roots well done'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, name, nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------------
;
pro TEST_FX_ROOT, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FX_ROOT, help=help, test=test, $'
   print, '              no_exit=no_exit, verbose=verbose'
   return
endif
;
init = 1.+ CINDGEN(3)
;
expected=0.73908514
TEST_FX_ROOT_ON_FUNC, cumul_errors, FLOAT(init), expected, $
                      verbose=verbose, function_name='FUNC1', iter=100
TEST_FX_ROOT_ON_FUNC, cumul_errors, FLOAT(init), expected, eps=0.001, $
                      verbose=verbose, function_name='FUNC1', iter=10
TEST_FX_ROOT_ON_FUNC, cumul_errors, FLOAT(init), expected, eps=0.001, $
                      verbose=verbose, function_name='FUNC1', tol=0.01

TEST_FX_ROOT_ON_FUNC, cumul_errors, init, expected, eps=0.001, $
                      verbose=verbose, function_name='FUNC1', iter=100
TEST_FX_ROOT_ON_FUNC, cumul_errors, init, expected, eps=0.001, $
                      verbose=verbose, function_name='FUNC1', iter=10
TEST_FX_ROOT_ON_FUNC, cumul_errors, init, expected, eps=0.001, $
                      verbose=verbose, function_name='FUNC1', tol=0.01
;
expected=2.2360680
TEST_FX_ROOT_ON_FUNC, cumul_errors, init, expected, $
                      verbose=verbose, function_name='FUNC2', tol=0.0001
TEST_FX_ROOT_ON_FUNC, cumul_errors, FLOAT(init), expected, $
                      verbose=verbose, function_name='FUNC2', tol=0.0001
;
; another test on FUNC3
WHERE_DO_WE_CONVERGE, cumul_errors
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, ROUTINE_NAME(), cumul_errors, short=short
;
if (cumul_errors NE 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
;
