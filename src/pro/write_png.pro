;$Id: write_png.pro,v 1.5 2010-01-20 11:41:59 slayoo Exp $

pro write_png, filename, image,red, green, blue, $
               ORDER=ORDER,VERBOSE=VERBOSE,TRANSPARENT=TRANSPARENT
  on_error, 2
;+
;
;
;
; NAME: WRITE_PNG
;
;
; PURPOSE: write a image from memory to a bitmap
;
;
; CATEGORY: Images (IO)
;
;
; CALLING SEQUENCE: 
;         write_png,filename,image,red,green,blue,[ORDER=ORDER,$
;                                                  VERBOSE=VERBOSE, $  
;                                                  TRANSPARENT=TRANSPARENT  
;
;
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
;         Requires ImageMagick
;
;
; PROCEDURE:
;         Use ImageMagick to write the data as requested
;
; EXAMPLE:
;         
;
;
; MODIFICATION HISTORY:
; 	Written by: Christopher Lee 2004-05-28
;
;       JMG 08/30/06
;       Implement /ORDER keyword
;       Switch blue & red in transpose statements
;       Implement greyscale PNG if byte input
;
;-
; LICENCE:
; Copyright (C) 2004,
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;
;-
rgb = 1
n=size(image, /n_dimensions)
s=size(image,/dimensions)
sz = size(image)
ty = sz[sz[0]+1]

if(n eq 2) then begin
                                ;pseudocolor
    
    if(n_params() lt 5) then  $
    tvlct, red, green, blue, /get

    ; If BYTE image then use greyscale
    if (ty eq 1 and n_params() eq 2) then begin
        red   = bindgen(256)
        green = bindgen(256)
        blue  = bindgen(256)
    endif

                                ;colorvectors provided
    mid=magick_create(s[0],s[1])
    if(array_equal(size(image,/dimensions),$
                   size(transparent,/dimensions))) then begin
        print, "TRANSPARENT KEYWORD IS UNTESTED"
        _image=transpose([[[red[image]]],$
                          [[green[image]]],$
                          [[blue[image]]],$
                          [[transparent]]],$
                         [2,0,1])
        magick_matte, mid,/true
    endif else begin
        
       _image=transpose([[[red[image]]],$
                          [[green[image]]],$
                          [[blue[image]]]],$
                         [2,0,1])
    endelse
    
    magick_write,mid,_image,rgb=rgb
    if (keyword_set(order)) then magick_flip,mid
    magick_writefile,mid,filename,"PNG"
    magick_close,mid    

;    if(n_elements(red) eq n_elements(green) and $
;        n_elements(red) eq n_elements(blue)) then begin
;        magick_quantize,mid,long(n_elements(red))
;        magick_writefile,mid,filename,"PNG"
;        magick_close,mid    
;    endif
    
endif else if(n eq 3) then begin
    mid=magick_create(s[1],s[2])
    magick_write,mid,image,rgb=rgb
    magick_writefile,mid,filename,"PNG"
    magick_close,mid
endif else if(n eq 4) then begin
;this is untested, hence it's commented out, but in theory, it's all there.

;        mid=magick_create(s[1],s[2])
;        magick_matte, mid,/true
 ;       magick_write,mid,image,rgb=rgb
;        magick_writefile,mid,filename,"PNG"
 ;       magick_close,mid
endif





end



