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
;       /INTERPOLATE means do bilinear interpolation. Kept for
;        compatibility but unused: interpolation IS linear.
;       CUBIC=cubic: use a cubic interpolation. Currently limited by
;       the availability of cubic interpolation in GDL's INTERPOLATE.
;       /CENTERED means assume pixels centered.  This means
;         the pixel at (0,[0,[0]]) is clipped to 1/4 size.
;         Default is that pixel start (not center) is at index.'
;       MISSING=missing gives a value for undefined pixels.
;       /MINUS_ONE: option will be ignored. MISSING can be used instead.
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
                  CENTER=center, $
                  INTERP=interp, $
                  CUBIC = cubic, $
                  MINUS_ONE=minus_one, $
                  MISSING=missing, $
                  HELP=help, test=test

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
    print,'   /INTERPOLATE means do bilinear interpolation. Kept for'
    print,'    compatibility but unused: interpolation IS linear.'
    print,'   /CENTERED means assume pixels centered.  This means'
    print,'   the pixel at (0,[0,[0]]) is clipped to 1/4 size.'
    print,'   Default is that pixel start (not center) is at index.'
    print,'   CUBIC=cubic: use a cubic interpolation. Currently limited by'
    print,'   the availability of cubic interpolation in INTERPOLATE.'
    print,'   /CENTERED means assume pixels centered.  This means'
    print,'     the pixel at (0,[0,[0]]) is clipped to 1/4 size.'
    print,'     Default is that pixel start (not center) is at index.'
    print,'   MISSING=missing gives a value for undefined pixels.'
    print,'   /MINUS_ONE: option will be ignored. MISSING can be used instead.'
    print, '  /HELP gives this help.'
    print,' NOTE: CONGRID performs a resampling. Does not conserve Fluxes.'
    return,''
endif
;
ON_ERROR, 2                   ;Return to caller if error
;
ndim = SIZE(t, /N_DIMENSIONS)
dims = SIZE(t, /DIMENSIONS)
;
if(ndim gt 3) then $
  MESSAGE, 'Warning, array dimensions >3, using non-standard CONGRID extension.', /informational
if keyword_set(minus_one) then $
  MESSAGE, 'Warning, /MINUS_ONE option will be ignored. You can use /MISSING.', /informational
;
cflag = KEYWORD_SET(centered)	; 0=start, 1=centered.
;
;  if (npar gt ndim+1) then message, 'Too many output dimensions.'
;
nopt=0
IF (N_ELEMENTS(mx) NE 0) THEN BEGIN
    fx=1.0D*dims[0]/mx
    x = (cflag gt 0) ?fx*FINDGEN(mx)+0.5 : fx*FINDGEN(mx)
    nopt=1
ENDIF
;
IF (N_ELEMENTS(my) NE 0 && ndim ge 2) THEN BEGIN
    fy=1.0D*dims[1]/my
    y = (cflag gt 0) ?fy*FINDGEN(my)+0.5 : fy*FINDGEN(my)
    nopt=2
ENDIF
;
IF (N_ELEMENTS(mz) NE 0 && ndim ge 3) THEN BEGIN
    fz=1.0D*dims[2]/mz
    z = (cflag gt 0) ?fz*FINDGEN(mz)+0.5 : fz*FINDGEN(mz)
    nopt=3
ENDIF
;
IF (nopt lt ndim) THEN BEGIN
    ;; create a transposition vector
    p=SHIFT(INDGEN(ndim),-nopt)
    temp=TRANSPOSE(t,p)
    CASE nopt OF
        3: temp2 = INTERPOLATE(temp,x,y,z,/grid,cubic=cubic,missing=missing)
        2: temp2 = INTERPOLATE(temp,x,y,/grid,cubic=cubic,missing=missing)
        1: temp2 = INTERPOLATE(temp,x,/grid,cubic=cubic,missing=missing)
    ENDCASE
    p=SHIFT(INDGEN(ndim),nopt)
    t2=TRANSPOSE(temp2,p)
ENDIF ELSE BEGIN
    CASE nopt OF
        3: t2 = INTERPOLATE(t,x,y,z,/grid,cubic=cubic,missing=missing)
        2: t2 = INTERPOLATE(t,x,y,/grid,cubic=cubic,missing=missing)
        1: t2 = INTERPOLATE(t,x,/grid,cubic=cubic,missing=missing)
    ENDCASE
ENDELSE
;
if KEYWORD_SET(test) then STOP
;
return,t2
;
end
