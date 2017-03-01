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
; REESTRICTIONS: (obsolete mention)
;       Has to use a slightly cumbersome procedure as the nan key is
;	not yet implemented in MIN, MAX and BYTSCL.
; REESTRICTIONS: (current version)
;       When data contains NaN, we have no way to know what will
;       happen in basic arithmetic ( a > 0 if a contains NaN may
;       return 0. or NaN ...). Then we decide not to follow the rule
;       and to manage NaN when /Nan not set ... Result becomes more
;       predictable ...
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
; 2017-03-01 : AC: adding TOP= (bug report 717)
;
; LICENCE:
; Copyright (C) 2005, SJT; 2012, A. Coulais
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
; 
;-
pro TVSCL, image, x, y, NaN=NaN, top=top, $
           help=help, verbose=verbose, test=test, _extra = _extra
;
ON_ERROR, 2                     ; Return to caller on error.
;
if KEYWORD_SET(help) then begin
   print, 'pro TVSCL, image, x, y, NaN=NaN, $'
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
if KEYWORD_SET(nan) then begin
   locs = WHERE(FINITE(image), nf)
   if (nf EQ 0) then MESSAGE, "No finite values found in image"
   dmin = MIN(image[locs], max = dmax)
endif else begin
   ;; unpredictable result if image does contain NaN values (see MIN
   ;; doc. or Minimum and Maximum Operators in IDL doc.)
   ;;
   ;; this case is not well managed in IDL too (no warning)
   dmin = MIN(image, max = dmax)
   if ~FINITE(dmin) OR ~FINITE(dmax) then begin
      MESSAGE, /continue, 'Data range is wrong due to NaN/Inf, we used /NaN'
      dmin = MIN(image, max = dmax, /nan)
   endif
endelse
;
if !d.table_size eq 0 then imax = !d.n_colors-1 else imax = !d.table_size-1
;
if KEYWORD_SET(top) then imax=top
;
if KEYWORD_SET(verbose) then print, 'Range, imax :', dmin, dmax, imax
;
img = BYTSCL(image, min = dmin, max = dmax, top = imax)

case N_PARAMS() of
   1: TV, img, _extra = _extra
   2: TV, img, x, _extra = _extra
   3: TV, img, x, y, _extra = _extra
endcase
;
if KEYWORD_SET(test) then STOP
;
end
