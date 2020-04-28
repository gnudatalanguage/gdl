;+
; GAUSSIAN_FUNCTION
;	Return an n-dimensional gaussian function.
;
;  Usage:
;	gf = GAUSSIAN_FUNCTION(sigma)
;
; Returns:
;	The gaussian function
;
; Arguments:
;	sigma	float	The standard deviation(s).
;
; Keywords:
;	maximum	float	The maximum of the gaussian function
;	/normalize	If set, then Normalize the sum of the returned
;			function to 1
;	width	int	The width of the function to return.
;       /double         If set, compute and return in double
;
; Notes:
;	Attempt to implement GAUSSIAN_FUNCTION (uses definition from
;	Abramovitz & Stegun).
;       Quite easy to allocate too much memeory :(
;
; History:
;	Original: 2020-03-27; James Tappin
;       2020-06-08 : AC, managing Double, preserving odd/even ...
;
;-
; LICENCE:
; Copyright (C) 2020: SJT
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
; AC 2020408 : all computation in Gauss1 are now in Double
;
function GAUSS1, sigma, width, normalize = normalize
x = DINDGEN(width)-(width-1)/2.
c =  (x/sigma)^2 / 2.
gf = EXP(-c)
;;
if KEYWORD_SET(normalize) then gf /= sigma*SQRT(2.*!pi)
;;
return, gf
;  
end
;
; ---------------------
;
function GAUSSIAN_FUNCTION, sigma, width = width, maximum = maximum, $
                            normalize = normalize, double=double

  ndims = N_ELEMENTS(sigma)

  ;; managing Widths (odd + ndims) 
  if ~KEYWORD_SET(width) then width = ROUND(sigma*6)
  ;;
  ;; AC 2020-04-21 : the Gaussians can be Odd or Even ...
  ;; locs = WHERE(width mod 2 eq 0, nev)
  ;; if nev ne 0 then width[locs]++
  
  if ndims gt 1 && N_ELEMENTS(width) eq 1 then width = $
     REPLICATE(width, ndims)

  gf = DBLARR(width) +1.
  nf = N_ELEMENTS(gf)
  
  perm = SHIFT(INDGEN(ndims), 1)
  
  for j =  ndims-1, 0, -1 do begin
     gf = TRANSPOSE(gf, perm)
     gf1 = GAUSS1(sigma[j], width[j], normalize = normalize)
     gf1r = gf1[*, INTARR(nf/width[j])]
     gf[*] *= gf1r[*]
  endfor

  if KEYWORD_SET(maximum) && ~KEYWORD_SET(normalize) then $
     gf *= maximum

  if KEYWORD_SET(double) OR ISA(maximum, 'double') then begin
     return, gf
  endif else begin
     return, FLOAT(gf)
  endelse
end
