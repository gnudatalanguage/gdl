;$Id: read_pict.pro,v 1.1.1.1 2004-12-09 15:10:19 m_schellens Exp $

pro read_pict, filename, image,red, green, blue
;+
;
;
;
; NAME: READ_PICT
;
;
; PURPOSE: Reads a PICT file into memory
;
;
;
; CATEGORY: Images (IO)
;
;
; CALLING SEQUENCE: read_pict,filename,image,red,green,blue
;
;
;
; OUTPUTS: 
;    IMAGE: A 2d file containing the vector indices
;
;
;
; OPTIONAL OUTPUTS:
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
;         Use ImageMagick to read the data as requested
;
; EXAMPLE:
;         
;
;
; MODIFICATION HISTORY:
; 	Written by: Christopher Lee 2004-05-17
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

print, magick_IndexedColor(mid)
if(magick_IndexedColor(mid)) thaen begin
    image=magick_readIndexes(mid)
    magick_readcolormapRGB,mid,red,green,blue
endif else begin
    image=magick_read(mid)
endelse

magick_close,mid

end



