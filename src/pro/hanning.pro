;+
; HANNING
;	Implementation of the HANNING window.
;
; Usage:
;	knl = HANNING(n[, n1])
;
; Returns:
;	A 1 or 2 dimensional Hanning window.
;
; Arguments:
;	n	int	The SIZE (or X dimension) of the window required.
;	n1	int	The Y dimension of a 2-dimensional window.
;
; Keywords:
;	alpha	float	The width parameter, must be in the range
;			0.5-1. 0.5 (default) gives a hanning window,
;			0.54 gives the hamming window.
;	/double		If set, then do the calculations in double
;			precision.
;
; History:
;	Original: 2020-Mars-24; James Tappin
;-
; LICENCE:
; Copyright (C) 2020: SJT
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   

function HANNING, n, n1, alpha = alpha, double = double

if KEYWORD_SET(double) then begin
   if ~KEYWORD_SET(alpha) then alpha = 0.5d
   twopi = 2.d*!dpi
endif else begin
   if ~KEYWORD_SET(alpha) then alpha = 0.5
   twopi = 2.*!pi
endelse

h0 = alpha - (1.-alpha)*cos(twopi*FINDGEN(n)/FLOAT(n))

if N_PARAMS() eq 2 then begin
   h1 = alpha - (1.-alpha)*COS(twopi*FINDGEN(1, n1)/FLOAT(n1))
   h = h0[*, INTARR(n1)]*h1[INTARR(n), *]
   return, h
endif else return, h0

end
