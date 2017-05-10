;+
; NAME:
;	TVSCL
;
; PURPOSE:
;	Replicates the built-in TVSCL procedure in IDL for GDL
;
; CATEGORY:
;	Graphics
;
; CALLING SEQUENCE:
;	TVSCL, image, x,y
;
; INPUTS:
;	image	The image to be displayed, may be any numeric type.
;
; OPTIONAL INPUTS:
;	x, y	The position of the lower-left corner of the image.
;
; KEYWORD PARAMETERS:
;	/nan	If set, then treat non-numbers (infinity & nan) as
;		missing data.
;
;       top=
;
;	Any keyword accepted by TV is also accepted by TVSCL.
;
; SIDE EFFECTS:
;	An image is displayed on the current graphics device.
;
; MODIFICATION HISTORY:
;     Original: 14/03/2005; SJT
;     Modifications:
; 2010-04-14: Alain Coulais.
;        -- better managment of inputs
;        -- no use of "locs" if no /NaN !
; 2012-06-12: Alain Coulais. When first field in array is NaN,
; we don't known how the range will be compute --> we decide
; not to follow this rule ...
; 2017-03-01 : AC: adding TOP= (bug report 717) + details
; 2017-05-10 : GD: uses the fact that min is not needed and BYTSCL has Nan option now.
;                  remove options not supported by IDL.
;
; LICENCE:
; Copyright (C) 2005, SJT; 2012, A. Coulais
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
; 
;-
pro TVSCL, image, x, y, NaN=NaN, top=top, _extra = _extra

; silent:
compile_opt idl2, hidden
ON_ERROR, 2                     ; Return to caller on error.
;
if KEYWORD_SET(help) then begin
   print, 'pro TVSCL, image, x, y, NaN=NaN, top=top, $'
   print, '           help=help, verbose=verbose, test=test, _extra = _extra'
   return
endif
;
if N_PARAMS() EQ 0 then begin
   MESSAGE, 'Incorrect number of arguments.'
   return
endif
if (SIZE(image,/n_elements) EQ 0) then begin
   MESSAGE, ' Expression must be an array in this context'
   return
endif
;
if !d.table_size eq 0 then imax = !d.n_colors-1 else imax = !d.table_size-1
;
if (N_ELEMENTS(top) GT 0) then imax=top
;
if KEYWORD_SET(nan) then img = BYTSCL(image, max = dmax, top = imax, /nan) else img = BYTSCL(image, max = dmax, top = imax)

case N_PARAMS() of
   1: TV, img, _extra = _extra
   2: TV, img, x, _extra = _extra
   3: TV, img, x, y, _extra = _extra
endcase
;
end
