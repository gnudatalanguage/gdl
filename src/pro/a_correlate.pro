;+
; A_CORRELATE
;	Autocorrelation function.
;
; Usage:
;	acf = a_correlate(x, lag)
;
; Returns:
;	The autocorrelation of X at the specified lags.
;
; Arguments:
;	x	float	The dataset whose ACF is required
;	lag	int	The offsets at which the ACF is required.
;
; Keywords:
;	/covariance	If set, then return the autocovariance instead.
;	/double		If set then do the computations in double
;			precision (even if the input is single
;			precision).
;
; History:
;	Original: 27/4/20; SJT
;-
; Copyright (C) 2020: James Tappin
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   

function a_correlate, x, lag, covariance = covariance, double = double

  on_error, 2
  
  nlag = n_elements(lag)
  nvals = n_elements(x)

  if keyword_set(double) then begin
     xtmp = double(x)
     rv = dblarr(nlag)
  endif else begin
     xtmp = x
     if size(x, /type) eq 5 then rv = dblarr(nlag) $
     else rv = fltarr(nlag)
  endelse

  xtmp -= mean(xtmp)
  alag = abs(lag)
  
  for j = 0, nlag-1 do rv[j] = total(xtmp[0:-alag[j]-1] * $
                                     xtmp[alag[j]:*])

  if keyword_set(covariance) then rv /= nvals $
  else rv /= total(xtmp^2)

  return, rv
  
end
