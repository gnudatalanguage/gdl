;+
; CIR_3PNT
;	Find the radius and centre of the circumcircle of three
;	points.
;
; Usage:
;	cir_3pnt, x, y, r, x0, y0
;
; Arguments:
;	x	float	The X coordinates (3-element array)
;	y	float	The Y coordinates (3-element array)
;	r	double	A variable to return the radius of the circle.
;	x0	double	A variable to return the X coordinate of the centre.
;	y0	double	A variable to return the Y coordinate of the
;	centre.
;
; Notes:
;	Since the version in IDL returns doubles for any input type,
;	so does this one.
;
; History:
;	Original: 16/12/20; James Tappin
;-
; LICENCE:
; Copyright (C) 2020: SJT
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   

pro cir_3pnt, x, y, r, x0, y0

  on_error, 2
  
  if n_params() ne 5 then message, $
     "CIR_3PNT: usage: cir_3pnt, x, y, r, x0, y0"

  if n_elements(x) ne 3 || n_elements(y) ne 3 then message, $
     "X & Y must be 3-element arrays"

; Make double copies for consistency with the IDL version that always
; returns doubles.
  
  xd = double(x)
  yd = double(y)

; Radius from: https://www.mathopenref.com/trianglecircumcircle.html
  
  sides = [sqrt((xd[0]-xd[1])^2 + (yd[0]-yd[1])^2), $
           sqrt((xd[1]-xd[2])^2 + (yd[1]-yd[2])^2), $
           sqrt((xd[2]-xd[0])^2 + (yd[2]-yd[0])^2)]

  bl = total(sides)  * (sides[0]+sides[1]-sides[2]) * $
       (sides[1]+sides[2]-sides[0]) * (sides[2]+sides[0]-sides[1])

  if bl eq 0. then begin        ; Points are co-linear no valid solution
     r = !values.d_infinity
     x0 = !values.d_nan
     y0 = !values.d_nan
     return
  endif

  r = product(sides)/sqrt(bl)
           
;; Circumcenter

;; Based on https://www.quora.com/What-is-the-formula-for-circumcentre?share=1
;; With a little algebraic juggling in WxMaxima
  

  xc = [xd[1]+xd[2], xd[0]+xd[2], xd[0]+xd[1]]/2.
  yc = [yd[1]+yd[2], yd[0]+yd[2], yd[0]+yd[1]]/2.

;; 2 calculate the slopes of the bisectors == 1/ (slope of sides)

  slope = [(xd[1]-xd[2])/(yd[1]-yd[2]), $
           (xd[0]-xd[2])/(yd[0]-yd[2]), $
           (xd[0]-xd[1])/(yd[0]-yd[1])] ; 1/gradient of side.
  

;; Will get a NaN from the formula if slope is infinite, but only one
;; slope can be infinite for a non-degenerate triangle.

  if ~finite(slope[0]) then begin
     x0 = xc[0]
     y0 = -slope[1] * (x0 - xc[1]) + yc[1]
  endif else if ~finite(slope[1]) then begin
     x0 = xc[1]
     y0 = -slope[0] * (x0 - xc[0]) + yc[0]
  endif else if ~finite(slope[2]) then begin
     x0 = xc[2]
     y0 = -slope[0] * (x0 - xc[0]) + yc[0]
  endif else begin
     x0 = (yc[0] - yc[1] - slope[1]*xc[1] + slope[0]*xc[0]) / $
          (slope[0]-slope[1])
     y0 = -slope[2] * (x0 - xc[2]) + yc[2]
  endelse

end
  
