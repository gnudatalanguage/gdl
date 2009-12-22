; NAME: POLY_AREA
;
;
; PURPOSE:  Calculates the area of the polygon defined by the
; vectors x and y
;
;
; CATEGORY: Algebra/ mathematics
;
;
; CALLING SEQUENCE: result = POLY_AREA(X, Y[, /DOUBLE] [, /SIGNED])
;
;
; OUTPUTS: result : area of polygon defined by X and Y
;
; PROCEDURE: <http://en.wikipedia.org/wiki/Polygon>
;
; EXAMPLE:   x = [1, 0, -1, 0]
;            y = [0, 1, 0, -1]
;            print, poly_area(x, y)
;            2.00000
;
; MODIFICATION HISTORY:
;       Written by: John Serafin 2009-12-21
;
;-
; LICENCE:
; Copyright (C) 2009,
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;
function poly_area, x, y, double=double, signed=signed

if keyword_set(double) then begin
   xs = double(shift(x, -1))
   ys = double(shift(y, -1))
endif else begin
   xs = shift(x, -1)
   ys = shift(y, -1)
endelse
area = total(x*ys - xs*y)/2
if not keyword_set(signed) then area = abs(area)
return, area

end
