;
; under GNU GPL v2 or later
; Alain Coulais, 20-Jan-2012
; Mathieu Pinter, 12-June-2012
; 
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
; -------------------------------------------------
;
; http://www.mathcurve.com/courbes2d/agnesi/agnesi.shtml
function AGNESI_SQUARE, x
common agnesi_param, a
return, (a^3/(a^2+x^2))^2
end
;
pro TEST_QROMO_ON_AGNESI, cumul=cumul, no_exit=no_exit, test=test, verbose=verbose
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
eps=1e-3
;
nb_errors=0
if (ABS(expected1-comp1) GT eps) then nb_errors=nb_errors+1
if (ABS(expected2-comp2) GT eps) then nb_errors=nb_errors+1
if (ABS(expected3-comp3) GT eps) then nb_errors=nb_errors+1
;
if KEYWORD_SET(verbose) then begin
    print, 'AGNESI_SQUARE espec/comp/err: ', expected1, comp1, expected1-comp1
    print, 'AGNESI_SQUARE espec/comp/err: ', expected2, comp2, expected2-comp2
    print, 'AGNESI_SQUARE espec/comp/err: ', expected3, comp3, expected3-comp3
endif
;
if (nb_errors GT 0) then begin
    MESSAGE, /continue, STRING(nb_errors)+' Numerical Errors founded'
endif else begin
    MESSAGE, /continue, 'No Numerical Errors founded'
endelse
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if ARG_PRESENT(cumul) then begin
    if KEYWORD_SET(cumul) then cumul=cumul+nb_errors else cumul=nb_errors
endif
;
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
pro TEST_QROMO_RANGE, function_name=function_name, range=range, cumul=cumul, $
                      no_exit=no_exit, test=test, verbose=verbose
;
if (N_ELEMENTS(range) NE 2) then begin
   print, 'No range provided ! (2 elements expected)'
   return
endif else begin
   debut=range[0]
   fin=range[1]
endelse
;
if (N_ELEMENTS(eps) EQ 0) then eps=1e-6
nb_errors=0
;
resuQR=QROMO(function_name, debut, fin)
resuIN=CALL_FUNCTION(function_name+'_INTEGRAL', fin)
resuIN=resuIN-CALL_FUNCTION(function_name+'_INTEGRAL', debut)
if TOTAL((resuQR-resuIN)^2) GT eps then nb_errors=nb_errors+1
;
resuQR=QROMO(function_name, debut, fin,/double)
resuIN=CALL_FUNCTION(function_name+'_INTEGRAL', fin)
resuIN=resuIN-CALL_FUNCTION(function_name+'_INTEGRAL', debut)
if TOTAL((resuQR-resuIN)^2) GT eps then nb_errors=nb_errors+1
;
mess='function ' + function_name+ ' : '
if (nb_errors GT 0) then begin
    MESSAGE, /continue, mess+STRING(nb_errors)+' Errors founded'
endif else begin
    MESSAGE, /continue, mess+'No Errors founded'
endelse
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if ARG_PRESENT(cumul) then begin
    if KEYWORD_SET(cumeul) then cumul=cumul+nb_errors else cumul=nb_errors
endif
;
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------
;
pro TEST_QROMO_DIM, function_name=function_name, cumul=cumul, $
                    no_exit=no_exit, test=test, verbose=verbose
;
debut=1.0
fin=2.0
infini=1.0e30
;
if (N_ELEMENTS(eps) EQ 0) then eps=1e-6
nb_errors=0
;
resuQR=QROMO(function_name, debut, fin, JMAX=7)
resuIN=CALL_FUNCTION(function_name+'_INTEGRAL', fin)-CALL_FUNCTION(function_name+'_INTEGRAL', debut)
if TOTAL((resuQR-resuIN)^2) GT eps then nb_errors=nb_errors+1
;
resuQR=QROMO(function_name, debut, fin, /MIDPNT)
resuIN=CALL_FUNCTION(function_name+'_INTEGRAL', fin)-CALL_FUNCTION(function_name+'_INTEGRAL', debut)
if TOTAL((resuQR-resuIN)^2) GT eps then nb_errors=nb_errors+1
;
resuQR=QROMO(function_name, debut, /MIDEXP)
resuIN=CALL_FUNCTION(function_name+'_INTEGRAL', infini)-CALL_FUNCTION(function_name+'_INTEGRAL', debut)
if TOTAL((resuQR-resuIN)^2) GT eps then nb_errors=nb_errors+1
;
resuQR=QROMO(function_name, debut, fin, /DOUBLE)
resuIN=CALL_FUNCTION(function_name+'_INTEGRAL', fin)-CALL_FUNCTION(function_name+'_INTEGRAL', debut)
if TOTAL((resuQR-resuIN)^2) GT eps then nb_errors=nb_errors+1
;
resuQR=QROMO(function_name, debut, /MIDEXP, EPS=1e-13)
resuIN=CALL_FUNCTION(function_name+'_INTEGRAL', infini)-CALL_FUNCTION(function_name+'_INTEGRAL', debut)
if TOTAL((resuQR-resuIN)^2) GT eps then nb_errors=nb_errors+1
;
resuQR=QROMO(function_name, debut, fin, /MIDINF)
resuIN=CALL_FUNCTION(function_name+'_INTEGRAL', fin)-CALL_FUNCTION(function_name+'_INTEGRAL', debut)
if TOTAL((resuQR-resuIN)^2) GT eps then nb_errors=nb_errors+1
;
resuQR=QROMO(function_name, debut, fin, /MIDSQL)
resuIN=CALL_FUNCTION(function_name+'_INTEGRAL', fin)-CALL_FUNCTION(function_name+'_INTEGRAL', debut)
if TOTAL((resuQR-resuIN)^2) GT eps then nb_errors=nb_errors+1
;
resuQR=QROMO(function_name, debut, fin, /MIDSQU)
resuIN=CALL_FUNCTION(function_name+'_INTEGRAL', fin)-CALL_FUNCTION(function_name+'_INTEGRAL', debut)
if TOTAL((resuQR-resuIN)^2) GT eps then nb_errors=nb_errors+1
;
resuQR=QROMO(function_name, debut, fin, K=6)
resuIN=CALL_FUNCTION(function_name+'_INTEGRAL', fin)-CALL_FUNCTION(function_name+'_INTEGRAL', debut)
if TOTAL((resuQR-resuIN)^2) GT eps then nb_errors=nb_errors+1
;
mess='function ' + function_name+ ' : '
if (nb_errors GT 0) then begin
    MESSAGE, /continue, mess+STRING(nb_errors)+' Errors founded'
endif else begin
    MESSAGE, /continue, mess+'No Errors founded'
endelse
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if ARG_PRESENT(cumul) then begin
    if KEYWORD_SET(cumul) then cumul=cumul+nb_errors else cumul=nb_errors
endif
;
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------------
;
pro TEST_QROMO
;
cumul=0
;
TEST_QROMO_DIM, function_name='EXPON', cumul=cumul, /no_exit
TEST_QROMO_DIM, function_name='INV', cumul=cumul, /no_exit
;
TEST_QROMO_RANGE, function_name='ALGEBRAIC_LOG', range=[0,1], $
                  cumul=cumul, /no_exit
;
TEST_QROMO_ON_AGNESI, cumul=cumul, /no_exit
;
if (cumul GT 0) then begin
    MESSAGE, /continue, STRING(cumul)+' Errors founded'
    EXIT, status=1
endif else begin
    MESSAGE, /continue, 'No Errors founded'
endelse
;
end
