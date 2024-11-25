; part of GNU Data Language - GDL 
;-------------------------------------------------------------
;+
; NAME:
;       CONGRID
; PURPOSE:
;       Shrink or expands an array (any dimensions) to any size,
;       however only the first 3 dims (max) will be resized.
; CATEGORY:
;       Array Manipulation.
; CALLING SEQUENCE:
;       t2 = CONGRID(t,mx[,my[,mZ]])
; INPUTS:
;       t = input array.                       in
;       mx, my, mz = new (output) array size.  in
;       t2 = resulting array.                  out
; KEYWORD PARAMETERS:
;       /INTERPOLATE means do bilinear interpolation for 1D or 2D (3D is always linear).
;       By default, 1D and 2D interpolation is NEAREST_NEIGHBOUR (IDL idiosyncrasy).
;       CUBIC=cubic: use a cubic interpolation. -0 to -1. -0.5 is recommended
;       /CENTER means assume pixels centered.  This means
;         the pixel at (0,[0,[0]]) is clipped to 1/4 size.
;         Default is that a pixel starts at its index.
;       /MINUS_ONE:  will prevent CONGRID from extrapolating one row or
;               column beyond the bounds of the input array. 
;       /HELP gives this help.'
;
; NOTE:
;       CONGRID performs a resampling. Does not conserve Fluxes.
; OUTPUTS:
;       t2 = resulting array.                  out
;
; MODIFICATION HISTORY:
; 	Written by: 2011-12-17 G. Duvert (Initial import)
;
;-
; LICENCE:
; Copyright (C) 2011
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-
function CONGRID, t, mx, my, mz, $
   CENTER=doCenter, $
   INTERP=interp, $
   CUBIC = cubic, $
   MINUS_ONE=minus_one, $
   HELP=help, test=test
  
  compile_opt idl2, hidden
  
  npar=N_PARAMS()
  if (npar lt 2) or (npar gt 4) or KEYWORD_SET(help) then begin
     print,' Shrink or expands an array (any dimensions) to any size,'
     print,' however only the first 3 dims (max) will be resized.'
     print,' CALLING SEQUENCE:'
     print,'   t2 = congrid2(t,mx[,my][,mz])'
     print,'   t = input array.                       in'
     print,'   mx, my, mz = new (output) array size.  in'
     print,'   t2 = resulting array.                  out'
     print,' Keywords:'
     print,'   /INTERPOLATE means do bilinear interpolation for 1D or 2D (3D is always linear).'
     print,'    By default, 1D and 2D interpolation is NEAREST_NEIGHBOUR (IDL idiosyncrasy).'
     print,'   /CENTER means assume pixels centered.  This means'
     print,'   the pixel at (0,[0,[0]]) is clipped to 1/4 size.'
     print,'   Default is that pixel start (not center) is at index.'
     print,'   CUBIC=cubic: use a cubic interpolation. -0 to -1. -0.5 is recommended.'
     print, '  /HELP gives this help.'
     print,' NOTE: CONGRID performs a resampling. Does not conserve Fluxes.'
     return,''
  endif
;
  On_error, 2                   ;Return to caller if error
;
  ndim = size(t, /n_dimensions)
  dims = size(t, /dimensions)
; remove spurious dimensions 1:
  while dims[ndim-1] eq 1 do begin
     ndim--
     dims=dims[0:-2]
  endwhile
;
  if ndim eq 0 or ndim gt 3 then message, 'array must have 1, 2, or 3 dimensions.'
; default values
  interpol = keyword_set(interp)
  minus1 = keyword_set(minus_one)
  docubic = (n_elements(cubic) gt 0) ? cubic : 0
  if (docubic ne 0) then interpol = 1 ;will interpolate
  offset = keyword_set(doCenter) ? 0.5 : 0.0
  
                                ; x, y and z will be the coordinates of the output interpolation.
                                ; Not necessary to do this with dim=2 and no interpol (nearest neighbor) since POLY-2D is used (faster).
  if ((ndim ne 2) or ((ndim eq 2) and interpol)) then begin
     switch ndim of 
        3: z = float(dims[2]-minus1)/(mz-minus1)*(findgen(mz) + offset) - offset
        2: y = float(dims[1]-minus1)/(my-minus1)*(findgen(my) + offset) - offset
        1: x = float(dims[0]-minus1)/(mx-minus1)*(findgen(mx) + offset) - offset
     endswitch
  endif
  
  case ndim of
     1: t2 = (interpol) ? interpolate(t, x, cubic = docubic) : t[round(x)]
     2: begin 
        if interpol then t2 = interpolate(t, x, y, /grid, cubic=docubic) else begin
           expand = (mx gt dims[0])
           x1 = (minus1 or expand) ? mx-1 : mx ; this to insure that congrid will agree with rebin.
           p = [0,0,(dims[0]-minus1)/float(x1),0]
           Q = [0,(dims[1]-minus1)/float(my-minus1),0,0]
           t2 = POLY_2D(t, P, Q , interpol,mx,my) 
        endelse
     end
     3: t2 = interpolate(t, x, y, z, /GRID)
  endcase
;
  if KEYWORD_SET(test) then STOP
;
  return,t2
;
end
