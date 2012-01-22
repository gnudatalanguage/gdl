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
pro TEST_QROMB_DIM, function_name=function_name, eps=eps, $
                    help=help, verbose=verbose, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_QROMB_dim, function_name=function_name, eps=eps, $'
   print, '                    help=help, verbose=verbose, test=test'
   return
endif
;
; error tolerance
if (N_ELEMENTS(eps) EQ 0) then eps=1e-5
nb_errors=0
;
; ranges
;
a=0.0
b_1d=1.+findgen(10)
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
if TOTAL((resuQR-resuIN)^2) GT eps then nb_errors=nb_errors+1
;
resuQR=QROMB(function_name, a, b_2d)
resuIN=CALL_FUNCTION(function_intgrl,b_2d)-CALL_FUNCTION(function_intgrl,a)
if TOTAL((resuQR-resuIN)^2) GT eps then nb_errors=nb_errors+1
;
resuQR=QROMB(function_name, b_1d, a)
resuIN=CALL_FUNCTION(function_intgrl,a)-CALL_FUNCTION(function_intgrl,b_1d)
if TOTAL((resuQR-resuIN)^2) GT eps then nb_errors=nb_errors+1
;
resuQR=QROMB(function_name, b_2d, a)
resuIN=CALL_FUNCTION(function_intgrl,a)-CALL_FUNCTION(function_intgrl,b_2d)
if TOTAL((resuQR-resuIN)^2) GT eps then nb_errors=nb_errors+1
;
;
; --- both inputs are more than one element arrays
;
a_1d=replicate(0, 4)
a_2d=REFORM(a_1d, 2, 2)
;
resuQR=QROMB(function_name, a_1d, b_1d)
resuIN=CALL_FUNCTION(function_intgrl,b_1d)-CALL_FUNCTION(function_intgrl,a_1d)
if TOTAL((resuQR-resuIN)^2) GT eps then nb_errors=nb_errors+1
;
resuQR=QROMB(function_name, a_1d, b_2d)
resuIN=CALL_FUNCTION(function_intgrl,b_2d)-CALL_FUNCTION(function_intgrl,a_1d)
if TOTAL((resuQR-resuIN)^2) GT eps then nb_errors=nb_errors+1
;
resuQR=QROMB(function_name, a_2d, b_1d)
resuIN=CALL_FUNCTION(function_intgrl,b_1d)-CALL_FUNCTION(function_intgrl,a_2d)
if TOTAL((resuQR-resuIN)^2) GT eps then nb_errors=nb_errors+1
;
resuQR=QROMB(function_name, a_2d, b_2d)
resuIN=CALL_FUNCTION(function_intgrl,b_2d)-CALL_FUNCTION(function_intgrl,a_2d)
if TOTAL((resuQR-resuIN)^2) GT eps then nb_errors=nb_errors+1
;
if (nb_errors GT 0) then begin
    MESSAGE, /continue, STRING(nb_errors)+' Errors founded'
    if ~KEYWORD_SET(test) then EXIT, status=1
endif else begin
    MESSAGE, /continue, 'function '+function_name+' : No Errors founded'
endelse
;
if KEYWORD_SET(test) then STOP
;
end
;
pro TEST_QROMB
;
TEST_QROMB_DIM, function_name='SLOPE'
TEST_QROMB_DIM, function_name='SQUARE'
;
end
