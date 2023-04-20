Function Defroi, Sx, Sy, Xverts, Yverts, X0=x0, Y0=y0, ZOOM = ZOOM, $
	NOREGION = Noregion, NOFILL = Nofill, RESTORE = restore
 ;Define an irregular Region of Interest.
;+
; NAME:			DEFROI
; PURPOSE:	Define an irregular region of interest of an image
;		using the image display system and the cursor/mouse.
; CATEGORY:	Image processing.
; CALLING SEQUENCE:
;	R = Defroi(Sx, Sy, X0, Y0)
; INPUTS:
;	Sx, Sy = Size of image, in pixels.
; Optional Inputs:
;	X0, Y0 = Coordinate of Lower left corner of image on display.
;	If omitted, (0,0) is assumed.  Screen device coordinates.
;	ZOOM = zoom factor, if omitted, 1 is assumed.
; OUTPUTS:
;	Function result = vector of subscripts of pixels inside the region.
;	Side effect: The lowest bit in which the write mask is enabled
;	is changed.
; OPTIONAL OUTPUTS:
;	Xverts, Yverts = Optional output parameters which will contain
;		the vertices enclosing the region.
; KEYWORD Parameters:
;	NOREGION = Setting NOREGION inhibits the return of the
;		pixel subscripts.
;	NOFILL = if set, inhibits filling of irregular region on completion.
;	RESTORE = if set, original image on display is restored to its
;		original state on completion. 
; COMMON BLOCKS:
;	None.
; SIDE EFFECTS:
;	Display is changed if RESTORE is not set.
; RESTRICTIONS:
;	Only works for interactive, pixel oriented devices with a
;		cursor and an exclusive or writing mode.
;	A region may have at most 1000 vertices.  If this is not enough
;		edit the line setting MAXPNTS.
; PROCEDURE:
;	The exclusive or drawing mode is used to allow drawing and
;	erasing objects over the original object.
;
;	The operator marks the vertices of the region, either by
;		dragging the mouse with the left button depressed or by
;		marking vertices of an irregular polygon by clicking the
;		left mouse button, or with a combination of both.
;	The center button removes the most recently drawn points.
;	Press the right mouse button when finished.
;	When the operator is finished, the region is filled using 
;		the polyfill function, and the polyfillv function is used
;		to compute the subscripts within the region.
;
; MODIFICATION HISTORY:  DMS, March, 1987.
; 	Revised for SunView, DMS, Nov, 1987.
;       Added additional argument checking, SNG April, 1991
;	Modified for devices without write masks: DMS, March, 1992.
;		Uses exclusive or mode rather than write masks.
;	20-Oct-93 (MDM) - Changed call to CURSOR to use the WAIT=3 option
;			  (instead of WAIT=1) since it is reading the same
;			  data value many times.
;			- WAIT=3 didn't work - adding a 0.2 sec wait
;-
;
on_error,2		;Return to caller if error
nc1 = !d.table_size-1	;# of colors available

if sx lt 1 or sy lt 1 then $		;Check some obvious things
    message, 'Dimensions of the region must be greater than zero.'

if sx gt !d.x_size then $
    message, 'The width of the region must be less than ' + $
	strtrim(string(!d.x_size),2)

sy = sy < !d.y_size

device, set_graphics=6             ;Set XOR mode
again:
n = 0
print,'Left button to mark point'
print,'Middle button to erase previous point'
print,'Right button to close region'
maxpnts = 1000			;max # of points.
xverts = intarr(maxpnts)		;arrays
yverts = intarr(maxpnts)
xprev = -1
yprev = -1
if n_elements(x0) le 0 then x0 = 0
if n_elements(y0) le 0 then y0 = 0
if n_elements(zoom) le 0 then zoom = 1
;
;Cursor, xx, yy, /WAIT, /DEV		;Get 1st point with wait
Cursor, xx, yy, WAIT=3, /DEV		;Get 1st point with wait. MDM made WAIT=3 20-Oct-93
wait, 0.2
repeat begin
	xx = (xx - x0) / zoom	;To image coords
	yy = (yy - y0) / zoom
	if (xx lt sx) and (yy lt sy) and (!err eq 1) and $
	    ((xx ne xprev) or (yy ne yprev)) then begin	    ;New point?
		xprev = xx
		yprev = yy
		if n ge (maxpnts-1) then begin
			print,'Too many points'
			n = n-1
			endif
		xverts(n) = xx
		yverts(n) = yy
		if n ne 0 then $
		  plots,xverts(n-1:n)*zoom+x0,yverts(n-1:n)*zoom + y0, $
			/dev,color=nc1,/noclip
		n = n + 1
		endif
;		We use 2 or 5 for the middle button because some Microsoft
;		compatible mice use 5.
	if ((!err eq 2) or (!err eq 5)) and (n gt 0) then begin
		n = n-1
		if n gt 0 then begin  ;Remove a vertex
		  plots,xverts(n-1:n)*zoom+x0,yverts(n-1:n)*zoom+y0,color=nc1,$
			/dev,/noclip
                  wait, .1           ;Dont erase too fast
		  endif
	endif
	;Cursor, xx, yy, /WAIT, /DEV    ;get x,y, no wait, device coords.
	Cursor, xx, yy, WAIT=3, /DEV    ;get x,y, no wait, device coords. MDM made WAIT=3 20-Oct-93
	wait, 0.2
	endrep until !err eq 4

if n lt 3 then begin
	print,'ROI - Must have 3 points for region.  Try again.'
	goto,again
	endif
xverts = xverts(0:n-1)		;truncate
yverts = yverts(0:n-1)

if keyword_set(restore) then $
	plots, xverts*zoom+x0, yverts *zoom + y0, /dev, color=nc1, /noclip $
else if keyword_set(nofill) then $
 	plots, [xverts(0),xverts(n-1)]*zoom+x0, $
	       [yverts(0),yverts(n-1)]*zoom+y0,$
		/dev,color = nc1,/noclip   $	 ;Complete polygon
else polyfill, xverts*zoom+x0, yverts*zoom+y0,$
		/dev,color = nc1,/noclip ;Complete polygon

if !order ne 0 then yverts = sy-1-yverts	;Invert Y?
device,set_graphics=3   ;Re-enable normal copy write

if keyword_set(noregion) then a = 0 $
  else a = polyfillv(xverts,yverts,sx,sy)	; get subscripts inside area.

return,a
end
