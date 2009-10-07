;+
; NAME:
;	DIST
;
;
; PURPOSE:
;	Return an array with its values equal to the distance from the
;	origin. 
;
;
; CATEGORY:
;	General utilities
;
;
; CALLING SEQUENCE:
;	array = dist(m[,n])
;
;
; INPUTS:
;	m,n	The dimensions of the array.
;
;
; OUTPUTS:
;	array	float	The resultant array.
;
;
; MODIFICATION HISTORY:
;   14-Mar-2005 : written by SJT
;   21-Jul-2005 : PC faster algorithm
;   06-Oct-2009 : AC
;       -- managing DIST([12,24]) and DIST([12,24],13)
;       -- managing 0 or Neg Values
;
; LICENCE:
; Copyright (C) 2005, SJT, P. Chanial
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-

function DIST, m, n

on_error, 2
;
; When "m" or "n" contain more than one element,
; only the first one is used !
;
if (N_ELEMENTS(m) GT 1) then m=m[0]
if (N_ELEMENTS(n) GT 1) then n=n[0]
;
; if only "m" is provided
if (N_ELEMENTS(n) EQ 0) then n = m
;
if ((m LE 0) OR (n LE 0)) then begin
   MESSAGE, "Array dimensions must be greater than 0."
endif
                           ;
x = FINDGEN(m)
x = (x < (m-x))^2

result = FLTARR(m, n, /nozero)

result[*,0] = SQRT(x)
for i = 1l, n/2 do begin
   dist = SQRT(x + i^2.)        ; i^2. is slightly faster than float(i)^2
   result[0,i  ] = dist         ; [0,i] is way faster than [*,i]
   result[0,n-i] = dist
endfor

return, result

end
