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
;	Original: 14/3/05; SJT
;-

function dist, m, n

on_error, 2

m1 = m[0]
n1 = (n_params() eq 2) ? n[0] : m1

x = findgen(m1)
x = (x < (m1-x))^2

y = findgen(1, n1)
y = (y < (n1-y))^2

return, sqrt(x[*, intarr(n1)]+y[intarr(m1), *])

end
