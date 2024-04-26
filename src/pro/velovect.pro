; LICENCE:
; Copyright (C) 2022: SJT
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   

;+
; VELOVECT
;	Produce a two-dimensional velocity field plot. A directed
;	arrow is drawn at each point showing the direction and
;	magnitude of the field
;
; Usage:
;	velovect, vx, vy[, x, y]
;
; Arguments:
;	vx	real	The x component of the field, must be a 2-D
;			array.
;	vy	real	The y component of the field, must have the
;			same dimensions as vx
;	x	real	The X locations of the points, must be a 1-D
;			array with the length of the first dimension
;			of vx & vy 
;	y	real	The Y locations of the points, must be a 1-D
;			array with the length of the second dimension
;			of vx & vy
;
; Keywords:
;	length	real	The length factor for the arrows, 1.0
;			corresponds to the longest arrow having a
;			length equal to the mean spacing of the more
;			densely sampled axis.
;	missing	real	A "missing" data value, points with magnitude
;			greater than this are omitted.
;	/dots		If set then "missing" values are plotted as
;			dots.
;	symsize	real	Sets the size of the arrow head, the default
;			is 1/4 of the vector length. (N.B. this is an
;			extension over the IDL version).
;	/overplot	If set, then draw the field on the existing
;			axes and coordinates.
;			
;	Other keys to the PLOT procedure are also accepted EXCEPT
;	that, POLAR, XLOG & YLOG are ignored (at least for the time
;	being) as they don't look to work sensibly in IDL.
;
; Common blocks:
;	velovect_parameters	Passes some geometry parameters to
;				vv_arrow so they don't need to
;				be recomputed for every arrow.
;
; History:
;	Original: 27-28/9/22; SJT
;-

pro vv_arrow, x0, y0, lx, ly, color = color, thick = thick

; VV_ARROW
; 	Draws an individual arrow for VELOVECT, input coordinates are
; 	in NDC.
  
  compile_opt hidden
  on_error, 2

  common velovect_parameters, scx0, scy0, vv_symsize

  if lx eq 0. && ly eq 0 then begin
                                ; WX driver at any rate does not
                                ; reliably mark a single point. 
     dc = floor(convert_coord(x0, y0, /norm, /to_dev))
     plots, dc[0]+[0, 1], dc[1]+[0, 1], /dev
     plots, dc[0]+[0, 1], dc[1]+[1, 0], /dev

  endif else begin
                                ; Draw the shaft
     x1 = x0+lx
     y1 = y0+ly
     plots, [x0, x1], [y0, y1], /norm, color = color, thick = thick

                                ; This scaling is needed to ensure
                                ; that the arrow is actually
                                ; symmetrical about the shaft.
     
     th = atan(-ly/scy0, -lx/scx0)
     lvec = sqrt((lx/scx0)^2+(ly/scy0)^2)
     
     open = !dpi/6

                                ; Draw the head
     
     xx = x1 + [cos(th+open), 0., cos(th-open)]*lvec/4. * scx0*vv_symsize
     yy = y1 + [sin(th+open), 0., sin(th-open)]*lvec/4. * scy0*vv_symsize
     plots, xx, yy, /norm, color = color, thick = thick
  endelse
end

pro velovect, vx, vy, x, y, xrange = xrange, yrange = yrange, $
              overplot = overplot, length = length, color = color, $
              symsize = symsize, thick = thick, missing = missing, $
              dots = dots, polar = polar, xlog = xlog, ylog = ylog, $
              _extra = _extra

  on_error, 2
  common velovect_parameters, scx0, scy0, vv_symsize

                                ; Sanity checks on the inputs.
  
  sx = size(vx)
  sy = size(vy)

  if keyword_set(polar) || keyword_set(xlog) || keyword_set(ylog) then $
     message, /continue, ["POLAR, XLOG and YLOG are not (yet) supported", $
                          "[don't work sensibly in IDL], ignoring."]
  
  if sx[0] ne 2 || sy[0] ne 2 then message, $
     "VX & VY arrays must both be 2-dimensional."

  if sx[1] ne sy[1] || sx[2] ne sy[2] then message, $
     "VX & VY arrays must have the same dimensions."

  if n_params() lt 3 then x = dindgen(sx[1]) $
  else if size(x, /n_dim) ne 1 || n_elements(x) ne sx[1] then $
     message, ["If given, X must be 1-D and have the same number of", $
               "elements as the first dimension of VX & VY"]
  
  if n_params() lt 4 then y = dindgen(sy[2]) $
  else if size(y, /n_dim) ne 1 || n_elements(y) ne sy[2] then $
     message, ["If given, Y must be 1-D and have the same number of", $
               "elements as the second dimension of VX & VY"]

  if ~keyword_set(length) then length = 1.d
  if keyword_set(symsize) then vv_symsize = symsize $
  else vv_symsize = 1.d
  
                                ; How much space is needed beyond the
                                ; data points to accomodate the
                                ; arrows. Based on the average point
                                ; spacing.
  
  if ~keyword_set(overplot) then begin
     dxa = mean(x[1:*]-x)*length
     dya = mean(y[1:*]-y)*length

     if ~keyword_set(xrange) then $
        xrange = [min(x, max = mx)-dxa, mx+dxa]
     if ~keyword_set(yrange) then $
        yrange = [min(y, max = my)-dya, my+dya]

     plot, findgen(2), /nodata, xrange = xrange, yrange = yrange, $
           color = color, thick = thick, _extra = _extra
  endif
  
                                ; Ensure that when VX==VY the arrow is
                                ; at 45 degrees.

  xbsize = double(!d.x_size)
  ybsize = double(!d.y_size)
  if xbsize gt ybsize then begin
     scx0 = ybsize/xbsize
     scy0 = 1.d
  endif else begin
     scy0 = xbsize/ybsize
     scx0 = 1.d
  endelse
  
  scx = scx0*double(length)/double(max(sx[1:2]))
  scy = scy0*double(length)/double(max(sx[1:2]))

                                ; Check for points to omit and scale
                                ; the lengths to the mean point
                                ; spacing.
  
  vabs = sqrt(vx^2+vy^2)
  if keyword_set(missing) then begin
     vmax = max(vabs) < missing
     locs = where(vabs gt missing, ngt)
     if ngt gt 0 then vabs[locs] = -1.d
  endif else begin
     vmax = max(vabs)
     ngt = 0
  endelse
  
  lx = scx*vx/vmax
  ly = scy*vy/vmax

  if ngt gt 0 then begin
     lx[locs] = 0.d
     ly[locs] = 0.d
  endif

                                ; Draw an arrow for each point. Note
                                ; that the actual specification of the
                                ; arrows is in NDC.
  
  for j = 0, sx[1]-1 do $
     for i = 0, sy[2]-1 do begin

     if keyword_set(missing) && ~keyword_set(dots) && $
        vabs[j, i] lt 0 then continue
     og = convert_coord(x[j], y[i], /data, /to_norm)
     
     vv_arrow, og[0], og[1], lx[j, i], ly[j, i], color = color, $
               thick = thick
  endfor

end

  
