;+
; NAME: ROT
;
; PURPOSE: 
;   Rotate an image (2D array) by a given angle (in Degree, CLOCKWISE)
;   Magnification and demagnification are possibles
;      (using input param "magnification")
;   Translation also (using input params "xNewCenter", "yNewCenter")
;
; CATEGORY:
;
; CALLING SEQUENCE:
;
;   resu=ROT(image, angle, [mag, [Xnew, Ynew]], 
;             interp=interp, missing=missing, $
;             pivot=pivot, cubic=cubic, $
;             help=help, test=test, debug=debug, verbose=verbose)
;
; INPUTS:
;  -- image : a 2D array (this is enforced)
;  -- angle : a given angle (in Degree, CLOCKWISE)
;
; OPTIONAL INPUTS:
;  -- magnification : will be set to 1. if not provided
;       1: no effect, > 1.: magnification, < 1.: demagnification
;  -- xNewCenter, yNewCenter :
;
; KEYWORD PARAMETERS:
;
;
;
; OUTPUTS: a 2D array with same dimension than the imput 2D image.
;
; OPTIONAL OUTPUTS: none
;
; COMMON BLOCKS: none
;
; SIDE EFFECTS: none
;
; RESTRICTIONS: we do have a known pending in POLY_2D for extrapolation
;
; PROCEDURE: we use the c++ internal POLY_2D code to do the job.
;
; EXAMPLE:
;
;+
;
; MODIFICATION HISTORY:
; - Initial import by Alain Coulais 12-Aug-2012
;
;-
; LICENCE:
; Copyright (C) 2012; AC
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;-
;
function ROT, image, angle, magnification, xNewCenter, yNewCenter, $
              INTERP=interp, MISSING=missing, $
              PIVOT=pivot, CUBIC=cubic, $
              help=help, test=test, debug=debug, verbose=verbose
;
;
if ~KEYWORD_SET(debug) then ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
    print, 'function ROT, image, angle, magnification, xNewCenter, yNewCenter, $'
    print, '              INTERP=interp, MISSING=missing, $'
    print, '              PIVOT=pivot, CUBIC=cubic, $'
    print, '              help=help, test=test, debug=debug, verbose=verbose'
    print, ''
    print, 'angle: must be in Degree'
    return, -1
endif
;
sz=SIZE(image)
if sz[0] NE 2 then MESSAGE, 'input image must 2D array'
;
; which magnification factor ?
if (N_PARAMS(0) LT 3) then magnification=1.
;
; we don't used it if only "xcenter" is provided
;
if (N_PARAMS(0) LT 5) then begin
   xNewCenter = (sz[1]-1)/2.
   yNewCenter = (sz[2]-1)/2.
endif
;
if KEYWORD_SET(pivot) then begin
   xCenter=xNewCenter
   yCenter=yNewCenter
endif else begin
   xCenter = (sz[1]-1)/2.
   yCenter = (sz[2]-1)/2.
endelse
;
; input is an Angle in degrees CLOCKWISE.
angle_Radian=(-angle mod 360)*!dpi/180
;
c = COS(angle_Radian)*magnification
s = SIN(angle_Radian)*magnification
;
; usual math. for rotation
; http://en.wikipedia.org/wiki/Rotation_matrix
kx = -xCenter+c*xNewCenter-s*yNewCenter
ky = -yCenter+s*xNewCenter+c*yNewCenter
knorm = 1./(1.+s^2/c^2)
;
;
; rot. informations used in POLY_2D
cx = knorm* [s/c^2*ky+kx/c, s/c^2, 1/c   ,0.]
cy = knorm*[-s/c^2*kx+ky/c, 1/c  , -s/c^2,0.]
;
; which kind of options for POLY_2D ?
;
; interpolation is a flag, "bilinear" corresponds to "1"
do_interp=0
if KEYWORD_SET(interp) then do_interp=1
;
; cubic is a value in (0,1,2)
if (N_ELEMENTS(cubic) EQ 0) then cubic=0
;
if N_ELEMENTS(missing) eq 0 then begin
   resu=POLY_2D(image, cx, cy, do_interp, CUBIC=cubic)
endif else begin
   resu=POLY_2D(image, cx, cy, do_interp, MISSING = missing, CUBIC=cubic)
endelse
;
if KEYWORD_SET(test) OR KEYWORD_SET(debug) then STOP
;
return, resu
;
end
