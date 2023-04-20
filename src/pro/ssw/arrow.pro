PRO ARROW, x0, y0, x1, y1, HSIZE = hsize, COLOR = color, HTHICK = hthick, $
	THICK = thick, DATA = data, DEVICE = device, NORMALIZED = norm, $
	SOLID = solid
;+
; NAME:				ARROW
; PURPOSE:	Draw a vector(s) with an arrow head
; CATEGORY:	Graphics
; CALLING SEQUENCE:
;	ARROW, x0, y0, x1, y1
; INPUTS:
;	(x0, y0) = coordinates of beginning of vector(s).  May be arrays
;		or scalars. Coordinates are in DEVICE coordinates
;		unless otherwise specified.
;	(x1, y1) = coordinates of endpoint (head) of vector.  
;		x0, y0, x1, y1 must all have the same number of elements.
; KEYWORD PARAMETERS:
;	DATA - if set, implies that coordinates are in data coords.
;	NORMALIZED - if set, coordinates are specified in normalized coords.
;	HSIZE = size of arrowhead.  Default = 1/64th the width of the device,
;		(!D.X_SIZE / 64.).
;		If the size is positive, it is assumed to be in device
;		coordinate units.  If it is NEGATIVE, then the head length
;		is set to the vector length * abs(hsize), giving heads
;		proportional in size to the bodies.  The size is defined as
;		the length of each of the lines (separated by 60 degrees) 
;		that make the head.
;	COLOR = drawing color.  Default = highest color index.
;	HTHICK = thickness of heads.  Default = 1.0.
;	SOLID = if set, make a solid arrow, using polygon fills, looks better
;		for thick arrows.
;	THICK = thickness of body.    Default = 1.0.
;	
; OUTPUTS:
;	No explicit outputs.
; SIDE EFFECTS:
; RESTRICTIONS:
; PROCEDURE:
;	Straightforward.
;	Examples:
;  		Draw an arrow from (100,150) to (300,350) in DEVICE units.
;	ARROW, 100, 150,  300, 350
;
;		Draw a sine wave with arrows from the line Y=0 to
;		sin(x/4).
;	X = FINDGEN(50)
;	Y = SIN(x/4)		;Make sin wave
;	PLOT, X, Y
;	ARROW, X, REPLICATE(0,50), X, Y, /DATA
; MODIFICATION HISTORY:
;	DMS, Feb, 1992.
;	DMS, Sept, 1992.  Added /SOLID.
;-

;  Draw an arrow with a head from (x0,y0) to (x1, y1).  Params may be
;		vectors.

;  Set up keyword params

if n_elements(thick) eq 0 then thick = 1.
if n_elements(hthick) eq 0 then hthick = thick

				;Head size in device units
if n_elements(hsize) eq 0 then arrowsize = !d.x_size/64. * (hthick/2. > 1) $
    else arrowsize = float(hsize)
if n_elements(color) eq 0 then color = !P.color

mcost = -.866		;We use 30 degrees for head angle
sint = .500
msint = - sint

for i = 0, n_elements(x0)-1 do begin		;Each vector
	if keyword_set(data) then $		;Convert?
	    p = convert_coord([x0(i),x1(i)],[y0(i),y1(i)], /data, /to_dev) $
	else if keyword_set(norm) then $
	    p = convert_coord([x0(i),x1(i)],[y0(i),y1(i)], /norm, /to_dev) $
	else p = [[x0(i), y0(i)],[x1(i), y1(i)]]

	xp0 = p(0,0)
	xp1 = p(0,1)
	yp0 = p(1,0)
	yp1 = p(1,1)

	dx = float(xp1-xp0)
	dy = float(yp1-yp0)
	zz = sqrt(dx^2 + dy^2)	;Length

	if zz gt 1e-6 then begin
		dx = dx/zz		;Cos th
		dy = dy/zz		;Sin th
	endif else begin
		dx = 1.
		dy = 0.
		zz = 1.
	endelse
	if arrowsize gt 0 then a = arrowsize $  ;a = length of head
	else a = -zz * arrowsize

	xxp0 = xp1 + a * (dx*mcost - dy * msint)
	yyp0 = yp1 + a * (dx*msint + dy * mcost)
	xxp1 = xp1 + a * (dx*mcost - dy * sint)
	yyp1 = yp1 + a * (dx*sint  + dy * mcost)

	if keyword_set(solid) then begin	;Use polyfill?
	  b = a * mcost*.9	;End of arrow shaft (Fudge to force join)
	  plots, [xp0, xp1+b*dx], [yp0, yp1+b*dy], /DEVICE, $
		COLOR = color, THICK = thick
	  polyfill, [xxp0, xxp1, xp1, xxp0], [yyp0, yyp1, yp1, yyp0], $
		/DEVICE, COLOR = color
	endif else begin
	  plots, [xp0, xp1], [yp0, yp1], /DEVICE, COLOR = color, THICK = thick
	  plots, [xxp0,xp1,xxp1],[yyp0,yp1,yyp1], /DEVICE, COLOR = color, $
			THICK = hthick
	endelse
	ENDFOR
end
