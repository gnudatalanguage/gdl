;$Id: read_bmp.pro,v 1.3 2012-08-14 14:21:37 alaingdl Exp $

function read_bmp, filename, red, green, blue, rgb=rgb
  on_error, 2
;+
;
;
;
; NAME: READ_BMP
;
;
; PURPOSE: Reads a bitmap file into memory
;
;
;
; CATEGORY: Images (IO)
;
;
; CALLING SEQUENCE: data=read_bmp(filename,red,green,blue,[RGB=RGB])
;
;
;
;
;
; KEYWORD PARAMETERS: 
;     RGB : Set to reverse the color ordering from BGR  
;
;
; OUTPUTS: For true color images, data is a three dimensional array
; with pixel interleaving (i.e [3,columns,rows])
;
;
;
; OPTIONAL OUTPUTS: For pseudocolor only
;        red  : the Red colormap vector (for PseudoColor images)
;        green: the Green colormap vector (for PseudoColor images)
;        blue : the Blue colormap vector (for PseudoColor images)
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
;       Flip image: Marc  Schellens 2004-09-01 
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

if (N_ELEMENTS(filename) GT 1) then MESSAGE, "Only one file at once !"

mid=magick_open(filename)

if(magick_IndexedColor(mid)) then begin
    data=magick_readIndexes(mid)
    magick_readcolormapRGB,mid,red,green,blue

    ;; flip image
    sz  = size( data, /DIM)
    rev = sz[1] - 1L - lindgen( sz[1])
    data = data[ *, rev]

endif else begin
    data=magick_read(mid,rgb=rgb)

    ;; flip image
    sz  = size( data, /DIM)
    rev = sz[2] - 1L - lindgen( sz[2])
    data = data[ *, rev, *]

endelse

magick_close,mid
return, data
end



