;$Id: read_jpeg.pro,v 1.4 2010-01-20 11:41:59 slayoo Exp $

pro read_jpeg, filename, unit=unit,image, colortable,buffer=buffer,$
              colors=colors,dither=dither,grayscale=grayscale,order=order,$
              true=true,two_pass_quantize=two_pass_quantize
  on_error, 2
;+
;
;
;
; NAME: READ_JPEG
;
;
; PURPOSE: Reads a jpeg file into memory
;
;
;
; CATEGORY: Images (IO)
;
;
; CALLING SEQUENCE: read_jpeg,filename,image, colortable
;
;
;
;
;
; KEYWORD PARAMETERS: 
;        UNIT: not supported yet
;        BUFFER: not supported yet
;        COLORS: Number of colors to dither to (8->256)
;        DITHER: Method of dithering to use
;        GRAYSCALE: Return a grayscale image
;        ORDER: flip the image in the vertical 
;        TRUE: Interleaving (1:pixel, 2:line, 3:band)
;        TWO_PASS_QUANTIZE: Not supported yet
;
; OUTPUTS: For true color images, data is a three dimensional array
; with interleaving set by TRUE keyword
;
;
;
; OPTIONAL OUTPUTS: For pseudocolor only
;        COLORTABLE: the colortable produced (ncolors,3)
;
;
; SIDE EFFECTS:
;
;
; RESTRICTIONS:
;         Requires ImageMagick
;
;
; PROCEDURE:
;         Use ImageMagick to read the data as requested
;
; EXAMPLE:
;         
;
;
; MODIFICATION HISTORY:
; 	Written by: Christopher Lee 2004-05-17
;       Add convert to byte if 16-bit image     Joel Gales  02/05/06
;
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

if(keyword_set(unit)) then Message, "Keyword UNIT not supported"
if(keyword_set(buffer)) then Message, "Keyword BUFFER not supported"


if(not keyword_set(unit)) then mid=magick_open(filename)

;;DITHER if necessary
if(keyword_set(GRAYSCALE)) then begin
    magick_quantize, mid,/GRAYSCALE
endif else if(keyword_set(COLORS)) then begin
    if(colors lt 8 or color gt 256) then Message, "COLORS must be in the range 8 to 256"
    if(keyword_set(TWO_PASS_QUANTIZE)) then Message, "TWO_PASS_QUANTIZE not supported by ImageMagick."

    magick_quantize,mid,colors,dither=dither

endif


;;flip if order is set
if(keyword_set(order)) then magick_flip,mid


if(magick_IndexedColor(mid)) then begin
    image=magick_readIndexes(mid)
    magick_readcolormapRGB,mid,red,green,blue
    colortable=[[red],[green],[blue]]
endif else begin
    image=magick_read(mid)
endelse

; if 16-bit (unsigned short int) image convert to byte
sz = size(image)
type = sz[sz[0]+1]
if (type eq 2 or type eq 12) then begin
    print, 'Converting 16-bit image to byte'
    image = image / 256
    image = byte(image)
endif

if(not keyword_set(unit)) then magick_close,mid

;; "rotate" image to agree with IDL (JMG 08/18/04)
    tmp = image[0,*,*]
    image[0,*,*] = image[2,*,*]
    image[2,*,*] = tmp

if(keyword_set(TRUE)) then begin
    if(TRUE eq 1) then t=[0,1,2]
    if(TRUE eq 2) then t=[1,0,2]
    if(TRUE eq 3) then t=[1,2,0]
    
    image=transpose(image, t)
;    image=transpose(image[[2,1,0],*,*], t)
endif 
;else begin
;    image = image[[2,1,0],*,*]
;endelse

end



