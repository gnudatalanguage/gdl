;+
; NAME:
;	Box_Cursor
; PURPOSE:
;	Emulate the operation of a variable sized box cursor.
;	The box is constrained to lie entirely within the window.
; CATEGORY:
;	Interactive graphics.
; CALLING SEQUENCE:
;	Box_Cursor, x0, y0, nx, ny [, /INIT, /FIXED_SIZE, /ADJACENT, /CONTIN ]
; INPUTS:
;	No required input parameters.
; OPTIONAL INPUT PARAMETERS:
;	x0, y0, nx, & ny : initial location and size of the box,
;		required if the keyword INIT is set.
;		Otherwise, the box is initially in the center of the screen.
; KEYWORD PARAMETERS:
;	/INIT : x0,y0, nx,ny are used as the initial parameters for the box.
;	/FIXED_SIZE : nx and ny contain the initial size of the box,
;			and this size cannot be changed by the user.
;	/ADJACENT : cursor is kept adjacent to box (one pixel lower than box).
;	/CENTERED : cursor is kept centered in box.
;	/CONTINUOUS : cursor movement is read continuously,
;		any mouse button pressing will exit. Default is to click
;		left button to move, middle to change size, right to exit.
;	/KEEP_BOX : box is left drawn in window, otherwise it is erased on exit.
; OUTPUTS:
;	x0 = X value of lower left corner of box.
;	y0 = Y value of lower left corner of box.
;	nx = width of box in pixels.
;	ny = height of box in pixels. 
; SIDE EFFECTS:
;	A box is drawn in the currently active window.  It is erased on exit.
; RESTRICTIONS:
;	Works only with window system drivers.
; EXTERNAL CALLS:
;	pro box_draw	(if /KEEP_BOX)
; PROCEDURE:
;	The graphics function is set to 6 for eXclusive OR.  This allows the
;	box to be drawn and erased without disturbing the contents of window.
;	Operation is as follows:
;	Left Mouse button:  Move the box by dragging or clicking.
;	Middle Mouse button:  Resize the box by dragging or clicking.
;	Right mouse button:  Exit procedure, returning current box parameters.
; MODIFICATION HISTORY:
;	DMS, April, 1990.
;	FV, 1992, rearranged and added more options: /ADJACENT, /CONTINUOUS
;	FV, 1994, added more options: /KEEP_BOX, /CENTERED
;       GD, 2022, added 'as is' to GDL procedures.
;-

pro Box_Cursor, x0, y0, nx, ny, INIT=init, FIXED_SIZE=fixed_size, $
		ADJACENT=adjacent, CONTINUOUS=contin, WAIT=wsec, $
		CENTERED=centered, KEEP_BOX=keep

if N_elements( wsec ) NE 1 then wsec=0.1
if keyword_set( contin ) then fixed_size=1

if keyword_set( fixed_size ) then begin
	if N_elements( nx ) NE 1 then nx = !d.x_size/8
	if N_elements( ny ) NE 1 then ny = !d.y_size/8
   endif else centered=0

if keyword_set( centered ) then begin
	nxh = nx/2
	nyh = ny/2
   endif

if keyword_set( init ) then begin
	nx = nx < (!d.x_size-1)
	ny = ny < (!d.y_size-1)
	x0 = ( x0 < (!d.x_size-1 - nx) ) > 0	;Never outside window
	y0 = ( y0 < (!d.y_size-1 - ny) ) > 0
	if keyword_set( centered ) then begin
		tvcrs,x0+nxh,y0+nyh,/DEV
	 endif else if keyword_set( adjacent ) then begin
		tvcrs,x0-1,y0-1,/DEV
	  endif else tvcrs,x0,y0,/DEV
  endif else begin
	if NOT keyword_set( fixed_size ) then begin
		nx = !d.x_size/8   ;no fixed size.
		ny = !d.x_size/8
	   endif
	x0 = !d.x_size/2 - nx/2
	y0 = !d.y_size/2 - ny/2
   endelse

old_corner = (corner = 0)
x00 = x0
y00 = y0
px = [x0, x0 + nx, x0 + nx, x0, x0]	;X points
py = [y0, y0, y0 + ny, y0 + ny, y0]	;Y values

device, get_graphics = old, set_graphics = 6	;Set XOR graphics mode.
plots, px, py, col=255, /dev			;Draw the box
!err=0

while 1 do begin

	cursor, x, y, /DEV, CHANGE=contin	;get cursor Location

	if keyword_set( centered ) then begin
		x = x - nxh	;keep cursor centered in box.
		y = y - nyh
	 endif else if keyword_set( adjacent ) then begin
		x = x+1
		y = y+1		;keep box just right next to cursor (to see it).
	   endif

	if (!err eq 4) OR $
	   (keyword_set( contin ) AND (!err GT 0)) then begin
		plots, px, py, col=255, /dev	;Erase previous box
		empty
		device,set_graphics = old
		if keyword_set( keep ) then box_draw, POS=[x0,y0], SIZE=[nx,ny]
		return
	 endif else if (!err eq 2) and (NOT keyword_set(fixed_size)) then begin
		corner = 0
		if x le x00 then begin
			x0 = x & x1 = x00
		 endif else begin
			x0 = x00 & x1 = x & corner = 1	;Right side
		  endelse
		if y le y00 then begin
			y0 = y & y1 = y00
		 endif else begin
			y0 = y00 & y1 = y   & corner = corner + 2
		  endelse
		nx = abs(x0-x1) > 1
		ny = abs(y0-y1) > 1
	  endif else begin		;New corner?
		if corner and 1 then  x00 = (x-nx) > 0  else  x00 = x > 0
		if (corner and 2) ne 0 then  y00 = (y-ny) > 0  else  y00 = y > 0
		x0=x00 & y0=y00
	   endelse

	plots, px, py, col=255, /dev	;Erase previous box
	if old_corner ne corner then empty	;Decwindow bug
	old_corner = corner

	x0 = x0 < (!d.x_size-1 - nx)	;Never outside window
	y0 = y0 < (!d.y_size-1 - ny)

	px = [x0, x0 + nx, x0 + nx, x0, x0]	;X points
	py = [y0, y0, y0 + ny, y0 + ny, y0]	;Y values

	plots, px, py, col=255, /dev	;Draw the box
	wait,wsec			;Dont hog it all
   endwhile
end
