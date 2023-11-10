;
; under GNU GPL v2 or later
; Alain Coulais, 20-Jan-2012
; Mathieu Pinter, 12-June-2012
; 
; -------------------------------------------------
; WARNING : on many cases, GDL outputs are numérically way better than
; IDL one (e.g. : print, QROMO('ALGEBRAIC_LOG', 0,1) )
; -------------------------------------------------
;
; We compare the output from QROMO with exact values on
; well know integrales.
; We also check that the dimensions of the outputs are OK
;
; You can also provide a known function TOTO but also TOTO_INTEGRAL
; ...
;
; AC 2012-10-10
; Thanks to extensive tests of Planck Sky Model software
; http://www.apc.univ-paris7.fr/~delabrou/PSM/psm.html
; we gain new case with problems for QROMO. See ALGEBRAIC_LOG case ...
;
; AC 2023-11-09
; IDL is clearly less robust than GDL, we have to relax the tests
; to have them working in IDL too ... (eps ...)
; -------------------------------------------------
;
; http://www.mathcurve.com/courbes2d/agnesi/agnesi.shtml
function AGNESI_SQUARE, x
common agnesi_param, a
return, (a^3/(a^2+x^2))^2
end
;
; ----------------------------------
;
pro TEST_QROMO_ON_AGNESI, cumul_errors, eps=eps, test=test, verbose=verbose
;
common agnesi_param, a
;
a=1.
debut=2.0
fin=1.
fin2=12.
;
expected1=0.031382
expected2=0.6427
expected3=0.14251
;
comp1=QROMO('AGNESI_SQUARE', debut, /MIDEXP)
comp2=QROMO('AGNESI_SQUARE', 0, fin)
comp3=QROMO('AGNESI_SQUARE', fin, fin2)
;
; you can turn "eps" to negative to test the logic ...
if ~KEYWORD_SET(eps) then eps=1e-3
;
nb_errors=0
if (ABS(expected1-comp1) GT eps) then ERRORS_ADD, nb_errors, 'case 1'
if (ABS(expected2-comp2) GT eps) then ERRORS_ADD, nb_errors, 'case 2'
if (ABS(expected3-comp3) GT eps) then ERRORS_ADD, nb_errors, 'case 3'
;
if KEYWORD_SET(verbose) then begin
    print, 'AGNESI_SQUARE espec/comp/err: ', expected1, comp1, expected1-comp1
    print, 'AGNESI_SQUARE espec/comp/err: ', expected2, comp2, expected2-comp2
    print, 'AGNESI_SQUARE espec/comp/err: ', expected3, comp3, expected3-comp3
endif
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_QROMO_ON_AGNESI', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------
;
function EXPON, x
return, exp(-x)
end
;
function EXPON_INTEGRAL, x
return, -exp(-x)
end
;
function INV, x
return, 1/(1+x^2)+exp(-x)
end
;
function INV_INTEGRAL, x
return, atan(x)-exp(-x)
end
;
;https://www.gnu.org/software/gsl/doc/html/integration.html#adaptive-integration-example
; should return -4.0 in the range [0,1]
function ALGEBRAIC_LOG, x
alpha=1.0
return, ALOG(alpha*x)/SQRT(x)
end
; http://www.wolframalpha.com/input/?i=ln%28x%29+%2Fsqrt%28x%29
function ALGEBRAIC_LOG_INTEGRAL, x
alpha=1.0
if ABS(x) LT 1.e-10 then begin
   return, 0.0
endif else begin
   return, 2*SQRT(x)*(ALOG(alpha*x)-2.)
endelse
end
;
; -------------------------------
;
pro TEST_QROMO_RANGE, cumul_errors, function_name=function_name, $
                      range=range, eps=eps, test=test, verbose=verbose
;
if (N_ELEMENTS(range) NE 2) then begin
   print, 'No range provided ! (2 elements expected)'
   return
endif else begin
   debut=range[0]
   fin=range[1]
endelse
;
if (N_ELEMENTS(eps) EQ 0) then eps=1e-4
nb_errors=0
;
resuQR=QROMO(function_name, debut, fin)
resuIN=CALL_FUNCTION(function_name+'_INTEGRAL', fin)
resuIN=resuIN-CALL_FUNCTION(function_name+'_INTEGRAL', debut)
if TOTAL((resuQR-resuIN)^2) GT eps then ERRORS_ADD, nb_errors, 'case 1 float : '+function_name
;
resuQR=QROMO(function_name, debut, fin,/double)
resuIN=CALL_FUNCTION(function_name+'_INTEGRAL', fin)
resuIN=resuIN-CALL_FUNCTION(function_name+'_INTEGRAL', debut)
if TOTAL((resuQR-resuIN)^2) GT eps then ERRORS_ADD, nb_errors, 'case 2 double : '+function_name
;
mess='function ' + function_name+ ' : '
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_QROMO_RANGE', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------
;
pro TEST_QROMO_DIM, cumul_errors, function_name=function_name, $
                    eps=eps, test=test, verbose=verbose
;
nb_errors=0
;
debut=1.0
fin=2.0
infini=1.0e30
;
; you can turn "eps" to negative to test the logic ...
if ~KEYWORD_SET(eps) then eps=1e-4
;
; because QROMO in IDL is not good/accurate by default,
; we need to adjust the "eps" for IDL
;
if (GDL_IDL_FL() EQ 'IDL') then eps_idl=1.e-3 else eps_idl=1.e-7 
;
if ~KEYWORD_SET(function_name) then MESSAGE, 'You shoudl provide a function name'
;
resuQR=QROMO(function_name, debut, fin, JMAX=7)
resuIN=CALL_FUNCTION(function_name+'_INTEGRAL', fin)-CALL_FUNCTION(function_name+'_INTEGRAL', debut)
if TOTAL((resuQR-resuIN)^2) GT eps then ERRORS_ADD, nb_errors, function_name+' case JMAX=7'
;
resuQR=QROMO(function_name, debut, fin, /MIDPNT)
resuIN=CALL_FUNCTION(function_name+'_INTEGRAL', fin)-CALL_FUNCTION(function_name+'_INTEGRAL', debut)
if TOTAL((resuQR-resuIN)^2) GT eps then ERRORS_ADD, nb_errors, function_name+' case /MIDPNT'
;
resuQR=QROMO(function_name, debut, /MIDEXP, eps=eps_idl)
resuIN=CALL_FUNCTION(function_name+'_INTEGRAL', infini)-CALL_FUNCTION(function_name+'_INTEGRAL', debut)
if TOTAL((resuQR-resuIN)^2) GT eps then ERRORS_ADD, nb_errors, function_name+' case /MIDEXP'
stop
;
resuQR=QROMO(function_name, debut, fin, /DOUBLE)
resuIN=CALL_FUNCTION(function_name+'_INTEGRAL', fin)-CALL_FUNCTION(function_name+'_INTEGRAL', debut)
if TOTAL((resuQR-resuIN)^2) GT eps then ERRORS_ADD, nb_errors, function_name+' case /DOUBLE'
;
res=EXECUTE("resuQR=QROMO(function_name, debut, /MIDEXP, EPS=1e-12, /double)")
if (res EQ 1) then begin
   resuIN=CALL_FUNCTION(function_name+'_INTEGRAL', infini)-CALL_FUNCTION(function_name+'_INTEGRAL', debut)
   if TOTAL((resuQR-resuIN)^2) GT eps then ERRORS_ADD, nb_errors, function_name+' case /MIDEXP, EPS=1e-12'
endif else begin
   MESSAGE, /continue, "this test is known to fail on IDL"
endelse
;
resuQR=QROMO(function_name, debut, fin, /MIDINF)
resuIN=CALL_FUNCTION(function_name+'_INTEGRAL', fin)-CALL_FUNCTION(function_name+'_INTEGRAL', debut)
if TOTAL((resuQR-resuIN)^2) GT eps then ERRORS_ADD, nb_errors, function_name+' case /MIDINF'
;
resuQR=QROMO(function_name, debut, fin, /MIDSQL)
resuIN=CALL_FUNCTION(function_name+'_INTEGRAL', fin)-CALL_FUNCTION(function_name+'_INTEGRAL', debut)
if TOTAL((resuQR-resuIN)^2) GT eps then ERRORS_ADD, nb_errors, function_name+' case /MIDSQL'
;
resuQR=QROMO(function_name, debut, fin, /MIDSQU)
resuIN=CALL_FUNCTION(function_name+'_INTEGRAL', fin)-CALL_FUNCTION(function_name+'_INTEGRAL', debut)
if TOTAL((resuQR-resuIN)^2) GT eps then ERRORS_ADD, nb_errors, function_name+' case /MIDSQU'
;
resuQR=QROMO(function_name, debut, fin, K=6)
resuIN=CALL_FUNCTION(function_name+'_INTEGRAL', fin)-CALL_FUNCTION(function_name+'_INTEGRAL', debut)
if TOTAL((resuQR-resuIN)^2) GT eps then ERRORS_ADD, nb_errors, function_name+' case K=6'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, pref='TEST_QROMO_DIM ', function_name, nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ----------------------------------
;
pro TEST_QROMO, help=help, test=test, verbose=verbose, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_QROMO, help=help, test=test, verbose=verbose, no_exit=no_exit'
   return
endif 
;
cumul_errors=0
;
TEST_QROMO_DIM, cumul_errors, function_name='EXPON'
TEST_QROMO_DIM, cumul_errors, function_name='INV'
;
TEST_QROMO_RANGE, cumul_errors, function_name='ALGEBRAIC_LOG', range=[0.01,1]
TEST_QROMO_RANGE, cumul_errors, function_name='EXPON', range=[1,2]
TEST_QROMO_RANGE, cumul_errors, function_name='INV', range=[1,2]
;
TEST_QROMO_ON_AGNESI, cumul_errors
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_QROMO', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

