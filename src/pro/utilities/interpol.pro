;
; under GNU GPL v2 or later
;
; just a stub calling GDL_INTERPOL, but insures that x is sorted
; (and y of course) y in the 3 parameter case.
;
function INTERPOL, p0, p1, p2, _EXTRA=extra
;
COMPILE_OPT idl2, HIDDEN
ON_ERROR, 2
;
if n_params() eq 3 then begin
   s=sort(p1)
   p1=p1[s]
   p0=p0[s]
   return,GDL_INTERPOL(p0, p1, p2, _EXTRA=extra)
endif else return,GDL_INTERPOL( p0, p1, _EXTRA=extra)
end
