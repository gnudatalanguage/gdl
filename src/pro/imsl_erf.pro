; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
;
; ---------------------------------------
; Modifications history :
;
; - 2025-Dec-08 : AC. in fact, no /double in ERF() !
; ---------------------------------------
;
function IMSL_ERF, x, double=dbl, inverse=inv
;
on_error, 2
;
if ~KEYWORD_SET(inv) then return, ERF(x)
;
if TYPENAME(x) EQ 'COMPLEX' or TYPENAME(x) EQ 'DCOMPLEX' then $
   MESSAGE, 'Type (D)COMPLEX are not OK'
;
;  elegant way to test if we have values above 1 or below -1
if TOTAL([x] ge 1 or [x] le -1) gt 0 then $
   MESSAGE, 'The inverse error function is defined only for -1 < x < 1'
;
return, GDL_ERFINV(x, double=dbl)
;
end
