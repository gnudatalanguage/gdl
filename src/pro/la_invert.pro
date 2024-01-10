;
; A trivial wrap around for LA_INVERT,
; for compatibility because it is used by iCosmo;
; Alain C., 23-JAN-2012
;
function LA_INVERT, input, status=status, double=double
;
tmp=INVERT(input, status, double=double)
;
if (status GT 0) then begin
   mess='Singular matrix encountered, STATUS='
   mess=mess+STRCOMPRESS(STRING(status),/remove_all)+'.'
   MESSAGE, mess,/continue
endif
;
return, tmp
;
end

