;+
; C_CORRELATE
;	Cross-correlation function.
;
; Usage:
;	xcf = c_correlate(x, y, lag)
;
; Returns:
;	The cross-correlation of X & Y at the specified lags.
;
; Arguments:
;	x, y	float	The datasets whose XCF is required
;	lag	int	The offsets at which the XCF is required.
;
; Keywords:
;	/covariance	If set, then return the autocovariance instead.
;	/double		If set then do the computations in double
;			precision (even if the input is single
;			precision).
;
; History:
;	Original (after a_correlate): 27/4/20; SJT
;-
; Copyright (C) 2020: James Tappin
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   

function c_correlate, x, y, lag, covariance = covariance, double = double

  on_error, 2
  
  nlag = n_elements(lag)
  nvals = n_elements(x)
  if n_elements(y) ne nvals then message, $
     "X & Y must be the same length"
  
  if keyword_set(double) then begin
     xtmp = double(x)
     ytmp = double(y)
     rv = dblarr(nlag)
  endif else begin
     xtmp = x
     ytmp = y
     if size(x, /type) eq 5 || size(y, /type) eq 5 then $
        rv = dblarr(nlag) $
     else rv = fltarr(nlag)
  endelse

  xtmp -= mean(xtmp)
  ytmp -= mean(ytmp)
  
  alag = abs(lag)
  llp = where(lag ge 0, nlp, comp = lln, ncomp = nln)
  
  for j = 0, nlp-1 do rv[llp[j]] = total(xtmp[0:-alag[llp[j]]-1] * $
                                         ytmp[alag[llp[j]]:*])
  for j = 0, nln-1 do rv[lln[j]] = total(ytmp[0:-alag[lln[j]]-1] * $
                                         xtmp[alag[lln[j]]:*])

  if keyword_set(covariance) then rv /= nvals $
  else rv /= sqrt(total(xtmp^2)*total(ytmp^2))

  return, rv
  
end
