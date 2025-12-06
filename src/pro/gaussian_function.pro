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
;   Original: 2020-03-27; James Tappin
;   2020-06-08 : AC, managing Double, preserving odd/even ...
;   2025-12-02 : AC internal GAUSS1  renamed to INTERNAL_GAUSS1
;   to avoid potential conflict with GAUSS1 provided by MPFIT
;   2025-12-03 : AC : several pbs : /norm, max= + odd/even
;      now the associated test is OK
;
;-
; LICENCE:
; Copyright (C) 2020: SJT
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
function INTERNAL_GAUSS1, sigma, width
;
x = DINDGEN(width)-width/2
c =  (x/sigma)^2 / 2.
gf = EXP(-c)
;
return, gf
;  
end
;
; ---------------------
;
function GAUSSIAN_FUNCTION, sigma, width = width, maximum = maximum, $
                            normalize = normalize, double=double


ndims = N_ELEMENTS(sigma)
if ndims EQ 0 then begin
   sigma=1
   ndims=1
endif

  ;; managing Widths (odd + ndims) 
  if ~KEYWORD_SET(width) then width = (CEIL(sigma*3) or 1)*2+1
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
     gf1 = INTERNAL_GAUSS1(sigma[j], width[j])
     gf1r = gf1[*, INTARR(nf/width[j])]
     gf[*] *= gf1r[*]
  endfor

  if KEYWORD_SET(maximum) then begin
     gf *= maximum
  endif else begin
     if KEYWORD_SET(normalize) then gf=gf/TOTAL(gf)
  endelse

  if KEYWORD_SET(double) OR ISA(maximum, 'double') then begin
     return, gf
  endif else begin
     return, FLOAT(gf)
  endelse
end
