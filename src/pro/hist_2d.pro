; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
; uses hist_nd() released under GPL by J.D. Smith
function hist_2d, v1, v2, bin1=bin1, bin2=bin2, max1=max1, max2=max2, min1=min1, min2=min2
  on_error, 2

  ; bin-widths default to 1
  if n_elements(bin1) eq 0 then bin1 = 1
  if n_elements(bin2) eq 0 then bin2 = 1

  ; min and max default to minimum and maximum values ... with some exceptions
  if n_elements(min1) eq 0 then begin
    if n_elements(max1) ne 0 then min1 = 0 < min(v1, /nan) else min1 = 0 < min(v1, max=max1, /nan)
  endif else if n_elements(max1) eq 0 then max1 = max(v1, /nan)
  if n_elements(min2) eq 0 then begin
    if n_elements(max2) ne 0 then min2 = 0 < min(v2, /nan) else min2 = 0 < min(v2, max=max2, /nan)
  endif else if n_elements(max2) eq 0 then max2 = max(v2, /nan)

  ; sanity checks for some undefined-behaviour conditions
  if bin1 le 0 then message, "bin1 must be > 0"
  if bin2 le 0 then message, "bin2 must be > 0"
  if ~finite(min1) || ~finite(min2) || ~finite(max1) || ~finite(max2) then $
    message, "min1, min2, max1 and max2 must all be finite"
  if min1 eq max1 then message, "min1 must not be equal to max1"
  if min2 eq max2 then message, "min2 must not be equal to max2"

  ; the shorter array determines how many elements are taken into account
  n = n_elements(v1) < n_elements(v2)
  if n eq 1 then begin
    ; emulating IDL behaviour with 1-element arrays and scalars
    data = transpose([[v1[0], max1 + 1], [v2[0], max2 + 1]])
  endif else begin
    data = transpose([[v1[0 : n - 1]], [v2[0 : n - 1]]])
  endelse

  ret = hist_nd(temporary(data), [bin1, bin2], min=[min1, min2], max=[max1, max2])
  if size(ret,/n_dimensions) eq 1 then return, reform(ret, n_elements(ret), 1)
  return, ret

end
