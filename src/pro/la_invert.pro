;
; A trivial wrap around for LA_INVERT,
; for compatibility because it is used by iCosmo;
; Alain C., 23-JAN-2012
;
function LA_INVERT, input, status=status, double=double
;
return, INVERT(input, status, double=double)
;
end

