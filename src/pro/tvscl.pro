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
;     Original: 14/03/2005; SJT
;     Modification: 14/04/2010; Alain Coulais: 
;        -- better managment of inputs
;        -- no use of "locs" if no /NaN !
;
;-
pro TVSCL, image, x, y, nan = nan, _extra = _extra
;
ON_ERROR, 2                     ; Return to caller on error.
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
   dmin = MIN(image, max = dmax)
endelse

if !d.table_size eq 0 then imax = !d.n_colors-1 $
else imax = !d.table_size-1

img = BYTSCL(image, min = dmin, max = dmax, top = imax)

case N_PARAMS() of
    1: TV, img, _extra = _extra
    2: TV, img, x, _extra = _extra
    3: TV, img, x, y, _extra = _extra
endcase

end
