;
; under GNU GPL v2 or later
;
; just a stub calling GDL_INTERPOL, but insures that x is sorted
; (and y of course) y in the 3 parameter case.
;
function INTERPOL, p0, p1, p2, _EXTRA=extra
;
COMPILE_OPT idl2, HIDDEN
ON_ERROR, 1
;
; test for valid params
; P0 (aka V) must not be a string
if isa(P0,/STRING) then Message,"Operation illegal with strings."
; sort arrays if necessary
if n_params() eq 3 then begin
   s=sort(p1)
   p1=p1[s]
   p0=p0[s]
endif
; treat complex types as two real types, below
if isa(P0,/COMPLEX) then begin
    if n_params() eq 3 then begin
       real=GDL_INTERPOL(real_part(p0), p1, p2, _EXTRA=extra)
       imag=GDL_INTERPOL(imaginary(p0), p1, p2, _EXTRA=extra)
       return,complex(real,imag)
    endif else begin
        real=GDL_INTERPOL(real_part(p0), p1, _EXTRA=extra)
        imag=GDL_INTERPOL(imaginary(p0), p1, _EXTRA=extra)
        return,complex(real,imag)
    endelse
endif

; all others
if n_params() eq 3 then return,GDL_INTERPOL(p0, p1, p2, _EXTRA=extra) else return,GDL_INTERPOL( p0, p1, _EXTRA=extra)
end
