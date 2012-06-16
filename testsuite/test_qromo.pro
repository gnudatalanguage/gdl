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
; You can also provide a known function TOTO but also TOTO_INTEGRAL ...
; -------------------------------------------------
;
; http://www.mathcurve.com/courbes2d/agnesi/agnesi.shtml
function AGNESI_SQUARE, x
common agnesi_param, a
return, (a^3/(a^2+x^2))^2
end
;
pro TEST_QROMO_ON_AGNESI, value, param
;
common agnesi_param, a
if N_ELEMENTS(param) EQ 0 then a=1. else a=param[0]
;
if N_ELEMENTS(param) EQ 0 then debut=2.0 else debut=value[0]
;
; hum, not all is solved in QROMO :((
;
print, 'infinite integration:'
DEFSYSV, '!gdl', exists=isGDL
if ~isGDL then print, QROMO('AGNESI_SQUARE', debut, /MIDEXP)
if isGDL then print, QROMO('AGNESI_SQUARE', debut, 0., /MIDEXP)
;
print, 'finite integration:'
print, QROMO('AGNESI_SQUARE', debut, 4.)
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
; -------------------------------
;
pro TEST_QROMO_DIM, function_name=function_name
;
debut=[1.0]
fin=[2.0]
infini=[1.0e30]             
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
if (nb_errors GT 0) then begin
    MESSAGE, /continue, STRING(nb_errors)+' Errors founded'
    if ~KEYWORD_SET(test) then EXIT, status=1
endif else begin
    MESSAGE, /continue, 'function ' + function_name + ': No Errors founded'
endelse
;
if KEYWORD_SET(test) then STOP
;
end
;
pro TEST_QROMO
;
TEST_QROMO_DIM, function_name='EXPON'
TEST_QROMO_DIM, function_name='INV'
;
TEST_QROMO_ON_AGNESI
;
end
