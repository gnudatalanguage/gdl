;$Id: read_png.pro,v 1.1.1.1 2004-12-09 15:10:19 m_schellens Exp $

function read_png, filename, red,green,blue,order=order,verbose=verbose,transparent=transparent
;+
;
;
;
; NAME: READ_PNG
;
;
; PURPOSE: Reads a png file into memory
;
;
;
; CATEGORY: Images (IO)
;
;
; CALLING SEQUENCE: image=read_png(filename,r,g,b)
;
;
;
;
;
; KEYWORD PARAMETERS: 
;        ORDER: flip the image in the vertical 
;        VERBOSE: Not used yet
;        TRANSPARENT: Transparent
;
; OUTPUTS: For true color images, data is a three dimensional array
; with interleaving set by TRUE keyword
;
;
;
; OPTIONAL OUTPUTS: For pseudocolor only
;        red  : the Red colormap vector (for PseudoColor images)
;        green: the Green colormap vector (for PseudoColor images)
;        blue : the Blue colormap vector (for PseudoColor images)
;
; RESTRICTIONS:
;         Requires ImageMagick
;
; PROCEDURE:
;         Use ImageMagick to read the data as requested
;
; EXAMPLE:
;         
;
;
; MODIFICATION HISTORY:
; 	Written by: Christopher Lee 2004-05-23
;
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
mid=magick_open(filename)


;;flip if order is set
if(keyword_set(order)) then magick_flip,mid

if(magick_IndexedColor(mid)) then begin
    image=magick_readIndexes(mid)
    magick_readcolormapRGB,mid,red,green,blue
    colortable=[[red],[green],[blue]]
endif else begin
    image=magick_read(mid)
endelse

magick_close,mid
return,image

end



