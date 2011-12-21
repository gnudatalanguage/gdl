; part of GNU Data Language - GDL 
;-------------------------------------------------------------
;+
; NAME:
;       BILINEAR
; PURPOSE:
;       Bilinearly regrid a 2D array on a set of x,y coordinates  
; CATEGORY:
;       Array Manipulation.
; CALLING SEQUENCE:
;       z2 = bilinear(z,x,y,missing=missing)
; INPUTS:
;       Z = input 2-d array.                   in
;       X, Y : X and Y index positions where Z2 is to be estimated
;       from Z
;       missing: value for points falling outside the z boundaries
; KEYWORD PARAMETERS:
;       none
; OUTPUTS:
;       z2 = resulting array.                  out
;
; PROCEDURE:
;       Uses INTERPOLATE /GRID
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
;
function BILINEAR, z, x, y, missing=missing
;
ON_ERROR, 2                     ;Return to caller if error
;
if (N_PARAMS() ne 3) then MESSAGE, 'Incorrect number of arguments.'
;
dimx=SIZE(x,/n_dimensions)
dimy=SIZE(y,/n_dimensions)
;
if ((dimx gt 2) || (dimy gt 2)) then MESSAGE, 'X and Y must be 1D or 2D, max.'
if ( dimx ne dimy ) then MESSAGE, 'X and Y must be of same dimension(s).'
;
if ((dimx eq 1) && (dimy eq 1)) then begin
    return, INTERPOLATE(z, x, y, /GRID, missing=missing)
endif else begin
    return, INTERPOLATE(z, x, y, missing=missing)
endelse
;
end
