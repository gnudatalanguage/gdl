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
;   21-Jul-2005 : PC, faster algorithm
;
; LICENCE:
; Copyright (C) 2005, SJT, P. Chanial
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-

function dist, m, n

on_error, 2

 if n_elements(n) eq 0 then n = m
 x = findgen(m)
 x = (x < (m-x))^2

 result = fltarr(m, n, /nozero)
 
 result[*,0] = sqrt(x)
 for i = 1l, n/2 do begin
    dist = sqrt(x + i^2.) ; i^2. is slightly faster than float(i)^2
    result[0,i  ] = dist  ; [0,i] is way faster than [*,i]
    result[0,n-i] = dist
 endfor

return, result

end
