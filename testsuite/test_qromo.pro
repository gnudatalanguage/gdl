;
; under GNU GPL v2 or later
; Alain Coulais, 20-Jan-2012
; 
; -------------------------------------------------
;
; to be extend ...
;
; -------------------------------------------------
;
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
print, QROMO('AGNESI_SQUARE', debut, /MIDEXP)
;
end
;
pro TEST_QROMO
TEST_QROMO_ON_AGNESI
end
