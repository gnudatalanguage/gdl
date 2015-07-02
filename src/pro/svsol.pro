;
; This is the second step of SVDC/SVSOL usage
;
; Code snippset by Eric Hivon, June 2015
; Tested (IDL example + "big" random matrix) by Ilia N
;
; Compared to "slow" SVDC, since this code is rather fast enough
; even for large (512x512) matrix, we decided not to code it in C++
; (on i5 with input 512x512, SVDC ~ 2 sec., SVSOL 0.03 sec.)
;
; This code is under GNU GPL V2 or later
; Eric H., Ilia N., Alain C., July 2015
;
function SVSOL, u, w, v, vector, double=double, column=column, $
                test=test, help=help
;
if KEYWORD_SET(help) then begin
    print, 'You must call first SVDC !'
    print, ''
    print, 'function SVSOL, u, w, v, vector, double=double, column=column, $'
    print, '                test=test, help=help'
    return, -1
endif
;
nw=N_ELEMENTS(w)
;
w_threshold = MAX(ABS(w))
;
if KEYWORD_SET(double) OR ISA(w,'double') then begin
    Wp = DBLARR(nw,nw)
    w_threshold = w_threshold * 1.0d-06
endif else begin
    Wp = FLTARR(nw,nw)
    w_threshold = w_threshold * 1.0e-06    
endelse
;
; find large eigenvalues
large = WHERE(ABS(w) GT w_threshold, count)
;
; only invert those large eigenvalues
if (count gt 0) then Wp[large, large] = 1./w[large]
;
if KEYWORD_SET(column) then begin
     result = V # ( Wp # (TRANSPOSE(U) # Vector))
endif else begin
    ;; the usual way (default)
    result = TRANSPOSE(V) # ( Wp # (U # Vector))
endelse
;
if KEYWORD_SET(test) then STOP
;
return, result
;
end

