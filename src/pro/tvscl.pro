;+
; NAME:
;	TVSCL
;
;
; PURPOSE:
;	Replicates the built-in TVSCL procedure in IDL for GDL
;
;
; CATEGORY:
;	Graphics
;
;
; CALLING SEQUENCE:
;	TVSCL, image, x,y
;
;
; INPUTS:
;	image	The image to be displayed, may be any numeric type.
;
;
; OPTIONAL INPUTS:
;	x, y	The position of the lower-left corner of the image.
;
;
; KEYWORD PARAMETERS:
;	/nan	If set, then treat non-numbers (infinity & nan) as
;		missing data.
;
;	Any keyword accepted by TV is also accepted by TVSCL.
;
;
; SIDE EFFECTS:
;	An image is displayed on the current graphics device.
;
; REESTRICTIONS:
;	Has to use a slightly cumbersome procedure as the nan key is
;	not yet implemented in MIN, MAX and BYTSCL.
;
; MODIFICATION HISTORY:
;	Original: 14/3/05; SJT
;-
 
pro tvscl, image, x, y, nan = nan, _extra = _extra

on_error, 2                     ; Return to caller on error.

if keyword_set(nan) then begin
    locs = where(finite(image), nf)
    if nf eq 0 then message, "No finite values found in image"
endif else locs = lindgen(n_elements(image))

dmin = min(image[locs], max = dmax)

if !d.table_size eq 0 then imax = !d.n_colors-1 $
else imax = !d.table_size-1

img = bytscl(image, min = dmin, max = dmax, top = imax)

case n_params() of
    1: tv, img, _extra = _extra
    2: tv, img, x, _extra = _extra
    3: tv, img, x, y, _extra = _extra
endcase

end
