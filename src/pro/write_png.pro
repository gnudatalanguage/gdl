;+
;
; NAME: WRITE_PNG
;
; PURPOSE: write a image from memory to a bitmap
;
; CATEGORY: Images (IO)
;
; CALLING SEQUENCE: 
;    WRITE_PNG, filename, image, red, green, blue, $
;               order=order, transparent=transparent, $
;               test=test, verbose=verbose, help=help, debug=debug
;
; KEYWORD PARAMETERS: 
;     ORDER      : 1 = top-bottom, 0 = bottom-top
;     VERBOSE    : Not Used
;     TRANSPARENT: 
;           
;
; OPTIONAL INPUTS: For pseudocolor only
;        red  : the Red colormap vector (for PseudoColor images)
;        green: the Green colormap vector (for PseudoColor images)
;        blue : the Blue colormap vector (for PseudoColor images)
;
; RESTRICTIONS:
;         Requires ImageMagick or GraphicsMagick (this is tested)
;
; PROCEDURE:
;         Use ImageMagick to write the data as requested
;
; EXAMPLE: 
;         See "image_test.pro" in testsuite/
;
; MODIFICATION HISTORY:
;  Written by: Christopher Lee 2004-05-28
;
;  JMG 08/30/06
;    Implement /ORDER keyword
;    Switch blue & red in TRANSPOSE statements
;    Implement greyscale PNG if BYTE input
;
;  Modifications by Alain Coulais 09-Aug-2012:
;  - now  /order should be active not only for 2D images !
;  - 2D images can be writen (but not the best way today :(
;  (help welcome, same problem than in WRITE_JPEG)
; 
;  Modifications by Alain Coulais 17-Jul-2013:
;  -correcting bug 553 (color mixing in 2D+RBG)
;  test case: next image must be red !!
;  WRITE_PNG,'test.png', DIST(256), INDGEN(256), INTARR(256), INTARR(256)
; 
;-
; LICENCE:
; Copyright (C) 2004, 2011, 2012: CL, JMG, AC
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-
;
pro WRITE_PNG, filename, image, red, green, blue, $
               order=order, transparent=transparent, $
               verbose=verbose, help=help, test=test, debug=debug
;
; this line allows to compile also in IDL ...
FORWARD_FUNCTION MAGICK_EXISTS, MAGICK_PING, MAGICK_READ
;
;if ~KEYWORD_SET(debug) then ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
    print, 'pro WRITE_PNG, filename, image, red, green, blue, $'
    print, '               order=order, transparent=transparent, $'
    print, '               verbose=verbose, help=help, test=test, debug=debug'
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
;
nb_dims=SIZE(image, /n_dimensions)
;
if (nb_dims LT 2) OR (nb_dims GT 3) then begin
   MESSAGE, "Image array must be (n,m) or (1-4,n,m)."
endif
;
im_size=SIZE(image,/dimensions)
;
; basic init, to be change for 3D case !
if (nb_dims EQ 2) then nb_channels=1  ;;(pure 2D image)
;
if (nb_dims EQ 3) then begin
   if (im_size[0] GT 4) then begin
      MESSAGE, "Image array must be (n,m) or (1-4,n,m)."
   endif
   nb_channels=im_size[0]
endif
;
sz = SIZE(image)
ty = sz[sz[0]+1]
;
if KEYWORD_SET(verbose) then begin
   print, 'The dimension of the image to be written is : ', im_size
   print, 'Channel deep is : ', nb_channels
endif
;
if (nb_channels eq 1) then begin
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
   mid=MAGICK_CREATE(im_size[0],im_size[1])
   if (ARRAY_EQUAL(SIZE(image,/dimensions),$
                   SIZE(transparent,/dimensions))) then begin
      print, "TRANSPARENT KEYWORD IS UNTESTED"
      _image=TRANSPOSE([[[red[image]]],$
                        [[green[image]]],$
                        [[blue[image]]],$
                        [[transparent]]],$
                       [2,0,1])
      MAGICK_MATTE, mid
   endif else begin
      _image=TRANSPOSE([[[red[image]]],$
                        [[green[image]]],$
                        [[blue[image]]]],$
                       [2,0,1])
   endelse
   ;;
   MAGICK_WRITECOLORTABLE, mid, red, green, blue
;   MAGICK_WRITE, mid, reform(image,1,im_size[0],im_size[1]) ;, rgb=rgb
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
endif
;
; usual 3D case [3,N,M]
;
if(nb_channels EQ 3) then begin
   mid=MAGICK_CREATE(im_size[1],im_size[2])
   MAGICK_WRITE, mid, image, rgb=rgb
   if (KEYWORD_SET(order)) then MAGICK_FLIP, mid
   MAGICK_WRITEFILE, mid, filename,"PNG"
   MAGICK_CLOSE, mid
endif
;
; transparent 3D case [2,N,M] or [4,N,M]
;
if (nb_channels EQ 4) then begin
   mid=MAGICK_CREATE(im_size[1],im_size[2])
   MAGICK_MATTE, mid
   MAGICK_WRITE, mid, image, rgb=rgb
   if (KEYWORD_SET(order)) then MAGICK_FLIP, mid
   MAGICK_WRITEFILE, mid, filename, "PNG"
   MAGICK_CLOSE, mid
endif
if (nb_channels EQ 2) then begin
   mid=MAGICK_CREATE(im_size[1],im_size[2])
   MAGICK_MATTE, mid
   MAGICK_WRITE, mid, image
   if (KEYWORD_SET(order)) then MAGICK_FLIP, mid
   MAGICK_WRITEFILE, mid, filename, "PNG"
   MAGICK_CLOSE, mid
endif
;
if KEYWORD_SET(test) OR KEYWORD_SET(debug) then STOP
;
end



