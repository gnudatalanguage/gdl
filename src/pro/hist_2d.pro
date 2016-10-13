; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
; uses hist_nd() released under GPL by J.D. Smith
;
function HIST_2D, v1, v2, bin1=bin1, bin2=bin2, max1=max1, max2=max2, min1=min1, min2=min2, $
                  test=test, help=help
;
ON_ERROR, 2
;
if N_PARAMS() LT 2 then begin
   MESSAGE, 'Incorrect number of arguments.'
endif
if (SIZE(v1,/n_dim) EQ 0) AND  (SIZE(v2,/n_dim) EQ 0) then begin
   MESSAGE, 'one of the 2 Expressions must be an array in this context'
endif
;
if KEYWORD_SET(help) then begin
    print, 'function HIST_2D, v1, v2, $'
    print, '                  bin1=bin1, bin2=bin2, max1=max1, max2=max2, min1=min1, min2=min2, $'
    print, '                  test=test, help=help'
   return, -1
endif
;
;; bin-widths default to 1
if N_ELEMENTS(bin1) eq 0 then bin1 = 1
if N_ELEMENTS(bin2) eq 0 then bin2 = 1

;; min and max default to minimum and maximum values ... with some exceptions
if N_ELEMENTS(min1) eq 0 then begin
    if N_ELEMENTS(max1) ne 0 then min1 = 0 < min(v1, /nan) else min1 = 0 < min(v1, max=max1, /nan)
endif else if N_ELEMENTS(max1) eq 0 then max1 = max(v1, /nan)
if N_ELEMENTS(min2) eq 0 then begin
    if N_ELEMENTS(max2) ne 0 then min2 = 0 < min(v2, /nan) else min2 = 0 < min(v2, max=max2, /nan)
endif else if N_ELEMENTS(max2) eq 0 then max2 = max(v2, /nan)

;; sanity checks for some undefined-behaviour conditions
if bin1 le 0 then MESSAGE, "bin1 must be > 0"
if bin2 le 0 then MESSAGE, "bin2 must be > 0"
if ~FINITE(min1) || ~FINITE(min2) || ~FINITE(max1) || ~FINITE(max2) then $
   MESSAGE, "min1, min2, max1 and max2 must all be finite"
if min1 eq max1 then MESSAGE, "min1 must not be equal to max1"
if min2 eq max2 then MESSAGE, "min2 must not be equal to max2"

;; the shorter array determines how many elements are taken into account
n = N_ELEMENTS(v1) < N_ELEMENTS(v2)
if n eq 1 then begin
    ;; emulating IDL behaviour with 1-element arrays and scalars
    data = TRANSPOSE([[v1[0], max1 + 1], [v2[0], max2 + 1]])
endif else begin
    data = TRANSPOSE([[v1[0 : n - 1]], [v2[0 : n - 1]]])
endelse
;
ret = HIST_ND(TEMPORARY(data), [bin1, bin2], min=[min1, min2], max=[max1, max2])
;
if SIZE(ret,/n_dimensions) eq 1 then ret=REFORM(ret, N_ELEMENTS(ret), 1)
;
if KEYWORD_SET(test) then STOP
;
return, ret
;
end
