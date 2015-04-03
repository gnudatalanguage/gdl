;+
;
; NAME: WRITE_TIFF
;
; PURPOSE: write a tiff image from memory to a bitmap
;
; CATEGORY: Images (IO)
;
; CALLING SEQUENCE: 
;    WRITE_TIFF, filename, image, bits_per_sample, red, green, blue, $
;                compression, verbose=verbose, help=help, test=test, $
;                debug=debug
;
; KEYWORD PARAMETERS: 
;     ORDER      : 1 = top-bottom, 0 = bottom-top
;     VERBOSE    : Not Used
;     TRANSPARENT: 
;           
;
; OPTIONAL INPUTS:
;        bits_per_sample: bits per sample
;        compression: compression method
;        For pseudocolor only
;        red  : the Red colormap vector (for PseudoColor images)
;        green: the Green colormap vector (for PseudoColor images)
;        blue : the Blue colormap vector (for PseudoColor images)
;
; RESTRICTIONS:
;         Requires ImageMagick or GraphicsMagick
;
; PROCEDURE:
;         Use ImageMagick to write the data as requested
;
; EXAMPLE: 
;         Currently no example is available
;
; MODIFICATION HISTORY:
;  Written by: Jeongbin Park 2015-04-03, based on write_png.pro
; 
;-
; LICENCE:
; Copyright (C) 2015: JP
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-
;
pro WRITE_TIFF, filename, image, bits_per_sample, red=red, green=green, blue=blue, $
                compression=compreesion, verbose=verbose, help=help, test=test, $
                debug=debug
;
; this line allows to compile also in IDL ...
FORWARD_FUNCTION MAGICK_EXISTS, MAGICK_PING, MAGICK_READ
;
;if ~KEYWORD_SET(debug) then ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
    print, 'pro WRITE_TIFF, filename, image, bits_per_sample, red=red, green=green, blue=blue, $'
    print, '                compression=compression, verbose=verbose, help=help, test=test, $'
    print, '                debug=debug $'
    return
endif
;
; Do we have access to ImageMagick functionnalities ??
;
if (MAGICK_EXISTS() EQ 0) then begin
    MESSAGE, /continue, "GDL was compiled without ImageMagick support."
    MESSAGE, "You must have ImageMagick support to use this functionaly."
endif
;
rgb=1
;
nb_dims=SIZE(image, /n_dimensions)
;
if (nb_dims LT 2) OR (nb_dims GT 3) then begin
   MESSAGE, "Image array must be (n,m) or (k,n,m)."
endif
if (nb_dims eq 3) then begin
   MESSAGE, "Image array with (k,n,m) is not supported yet!"
endif
;
im_size=SIZE(image,/dimensions)
;
if (nb_dims EQ 2) then mid=MAGICK_CREATE(im_size[0],im_size[1])
;
if KEYWORD_SET(red) AND KEYWORD_SET(green) AND KEYWORD_SET(blue) then begin
    TVLCT, red, green, blue, /get
endif else begin
    red   = BINDGEN(256)
    green = BINDGEN(256)
    blue  = BINDGEN(256)
endelse
MAGICK_WRITECOLORTABLE, mid, red, green, blue
_image=TRANSPOSE([[[red  [image]]],$
                  [[green[image]]],$
                  [[blue [image]]]],$
                  [2,0,1])

MAGICK_WRITE, mid, _image, rgb=1
MAGICK_WRITEFILE, mid, filename, "TIFF"
MAGICK_CLOSE, mid
;
if KEYWORD_SET(test) OR KEYWORD_SET(debug) then STOP
;
end


