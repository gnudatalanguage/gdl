;+
;
; NAME: WRITE_PNG
;
; PURPOSE: write a image from memory to a bitmap
;
; CATEGORY: Images (IO)
;
; CALLING SEQUENCE: 
;         WRITE_PNG, filename, image, red, green, blue, $
;                    [order=order, verbose=verbose,
;transparent=transparent, 
;
; KEYWORD PARAMETERS: 
;     ORDER      :1 = top-bottom, 0 = bottom-top
;     VERBOSE    : Not Used
;     TRANSPARENT: 
;           
;
; OPTIONAL INPUTS: For pseudocolor only
;        red  : the Red colormap vector (for PseudoColor images)
;        green: the Green colormap vector (for PseudoColor images)
;        blue : the Blue colormap vector (for PseudoColor images)
;
;
; RESTRICTIONS:
;         Requires ImageMagick (this is tested)
;
; PROCEDURE:
;         Use ImageMagick to write the data as requested
;
; EXAMPLE: 
;         See "image_test.pro" in testsuite/
;
; MODIFICATION HISTORY:
; 	Written by: Christopher Lee 2004-05-28
;
;       JMG 08/30/06
;       Implement /ORDER keyword
;       Switch blue & red in TRANSPOSE statements
;       Implement greyscale PNG if BYTE input
; 
;-
; LICENCE:
; Copyright (C) 2004, 2011
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-
;
pro WRITE_PNG, filename, image, red, green, blue, $
               order=order, verbose=verbose, transparent=transparent, $
               help=help, test=test, debug=debug
;
if ~KEYWORD_SET(debug) then ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
    print, 'pro WRITE_PNG, filename, image, red, green, blue, $'
    print, '               order=order, verbose=verbose, transparent=transparent, $'
    print, '               help=help, test=test, debug=debug'
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
rgb = 1
n=SIZE(image, /n_dimensions)
s=SIZE(image,/dimensions)
sz = SIZE(image)
ty = sz[sz[0]+1]
;
if (n eq 2) then begin
   ;; pseudocolor
   if (N_PARAMS() lt 5) then TVLCT, red, green, blue, /get
   ;; If BYTE image then use greyscale
   if ((ty eq 1) and (N_PARAMS() eq 2)) then begin
      red   = BINDGEN(256)
      green = BINDGEN(256)
      blue  = BINDGEN(256)
   endif
   ;;
   ;;colorvectors provided
   mid=MAGICK_CREATE(s[0],s[1])
   if (ARRAY_EQUAL(SIZE(image,/dimensions),$
                   SIZE(transparent,/dimensions))) then begin
      print, "TRANSPARENT KEYWORD IS UNTESTED"
      _image=TRANSPOSE([[[red[image]]],$
                        [[green[image]]],$
                        [[blue[image]]],$
                        [[transparent]]],$
                       [2,0,1])
      MAGICK_MATTE, mid,/true
   endif else begin
      _image=TRANSPOSE([[[red[image]]],$
                        [[green[image]]],$
                        [[blue[image]]]],$
                       [2,0,1])
   endelse
   
   MAGICK_WRITE, mid, _image, rgb=rgb
   if (KEYWORD_SET(order)) then MAGICK_FLIP, mid
   MAGICK_WRITEFILE, mid, filename, "PNG"
   MAGICK_CLOSE, mid
   ;;
   ;;    if(N_ELEMENTS(red) eq N_ELEMENTS(green) and $
   ;;        N_ELEMENTS(red) eq N_ELEMENTS(blue)) then begin
   ;;        MAGICK_QUANTIZE,mid,long(N_ELEMENTS(red))
   ;;        MAGICK_WRITEfile,mid,filename,"PNG"
   ;;        MAGICK_CLOSE,mid    
   ;;    endif
   ;;
endif else if(n eq 3) then begin
   mid=MAGICK_CREATE(s[1],s[2])
   MAGICK_WRITE, mid, image, rgb=rgb
   MAGICK_WRITEFILE, mid, filename,"PNG"
   MAGICK_CLOSE, mid
endif else if(n eq 4) then begin
;this is untested, hence it's commented out, but in theory, it's all there.
;        mid=MAGICK_CREATE(s[1],s[2])
;        MAGICK_MATTE, mid,/true
;        MAGICK_WRITE,mid,image,rgb=rgb
;        MAGICK_WRITEfile,mid,filename,"PNG"
;        MAGICK_CLOSE,mid
endif
;
if KEYWORD_SET(test) OR KEYWORD_SET(debug) then STOP
;
end



