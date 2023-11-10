;
; under GNU GPL v2 or later
; Alain Coulais, 20-Jan-2012
; 
; -------------------------------------------------
;
; We compare the output from QROMB with exact velues on
; well know integrales.
; We also check that the dimensions of the outputs are OK
;
; You can also provide a known function TOTO but also TOTO_INTEGRAL ...
;
; -------------------------------------------------
;
function SLOPE, x
return, x
end
;
function SQUARE, x
return, x*x
end
;
function SLOPE_INTEGRAL, x
return, 0.5*x*x+1d-3
end
;
function SQUARE_INTEGRAL, x
return, 1./3.*x*x*x
end
;
; -------------------------------------------------
;
pro TEST_QROMB_DIM, cumul_errors, function_name=function_name, eps=eps, $
                    help=help, verbose=verbose, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_QROMB_DIM, cumul_errors, function_name=function_name, eps=eps, $'
   print, '                    help=help, verbose=verbose, test=test'
   return
endif
;
; error tolerance
if ~KEYWORD_SET(eps) then eps=1e-5
nb_errors=0
;
; ranges
;
a=0.0
b_1d=1.+FINDGEN(10)
b_2d=REFORM(b_1d, 2, 5)
;
if N_ELEMENTS(function_name) EQ 0 then function_name='SLOPE'
function_intgrl=function_name+'_INTEGRAL'
;
if (EXECUTE('res='+function_name+'(0)') EQ 0) then begin
   MESSAGE, 'Missing function : '+function_name
endif
;
if (EXECUTE('res='+function_name+'_INTEGRAL(0)') EQ 0) then begin
   MESSAGE, 'Missing function : '+function_name+'_INTEGRAL'
endif
;
; --- simple cases : one of the 2 inputs is "singleton" element
;
resuQR=QROMB(function_name, a, b_1d)
resuIN=CALL_FUNCTION(function_intgrl,b_1d)-CALL_FUNCTION(function_intgrl,a)
if TOTAL((resuQR-resuIN)^2) GT eps then ERRORS_ADD, nb_errors, function_name+' case a, b_1D'
;
resuQR=QROMB(function_name, a, b_2d)
resuIN=CALL_FUNCTION(function_intgrl,b_2d)-CALL_FUNCTION(function_intgrl,a)
if TOTAL((resuQR-resuIN)^2) GT eps then ERRORS_ADD, nb_errors, function_name+' case b_2D'
;
resuQR=QROMB(function_name, b_1d, a)
resuIN=CALL_FUNCTION(function_intgrl,a)-CALL_FUNCTION(function_intgrl,b_1d)
if TOTAL((resuQR-resuIN)^2) GT eps then ERRORS_ADD, nb_errors, function_name+' case b_1D, a'
;
resuQR=QROMB(function_name, b_2d, a)
resuIN=CALL_FUNCTION(function_intgrl,a)-CALL_FUNCTION(function_intgrl,b_2d)
if TOTAL((resuQR-resuIN)^2) GT eps then ERRORS_ADD, nb_errors, function_name+' case b_2D, a'
;
; --- both inputs are more than one element arrays
;
a_1d=REPLICATE(0, 4)
a_2d=REFORM(a_1d, 2, 2)
;
resuQR=QROMB(function_name, a_1d, b_1d)
resuIN=CALL_FUNCTION(function_intgrl,b_1d)-CALL_FUNCTION(function_intgrl,a_1d)
if TOTAL((resuQR-resuIN)^2) GT eps then ERRORS_ADD, nb_errors, function_name+' case  a_1d, b_1d'
;
resuQR=QROMB(function_name, a_1d, b_2d)
resuIN=CALL_FUNCTION(function_intgrl,b_2d)-CALL_FUNCTION(function_intgrl,a_1d)
if TOTAL((resuQR-resuIN)^2) GT eps then ERRORS_ADD, nb_errors, function_name+' case  a_1d, b_2d'
;
resuQR=QROMB(function_name, a_2d, b_1d)
resuIN=CALL_FUNCTION(function_intgrl,b_1d)-CALL_FUNCTION(function_intgrl,a_2d)
if TOTAL((resuQR-resuIN)^2) GT eps then ERRORS_ADD, nb_errors, function_name+' case  a_2d, b_1d'
;
resuQR=QROMB(function_name, a_2d, b_2d)
resuIN=CALL_FUNCTION(function_intgrl,b_2d)-CALL_FUNCTION(function_intgrl,a_2d)
if TOTAL((resuQR-resuIN)^2) GT eps then ERRORS_ADD, nb_errors, function_name+' case  a_2d, b_2d'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, pref='TEST_QROMB_DIM ', function_name, nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ----------------------------------
;
pro TEST_QROMB, help=help, test=test, verbose=verbose, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_QROMO, help=help, test=test, verbose=verbose, no_exit=no_exit'
   return
endif 
;
cumul_errors=0
;
TEST_QROMB_DIM, cumul_errors, function_name='SLOPE'
TEST_QROMB_DIM, cumul_errors, function_name='SQUARE'
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_QROMB', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
