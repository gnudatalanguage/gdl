;$Id: read_dicom.pro,v 1.1.1.1 2004-12-09 15:10:19 m_schellens Exp $

function read_dicom, filename, red, green, blue, image_index=image_index
;+
;
;
;
; NAME: READ_DICOM
;
;
; PURPOSE: Reads a dicom file into memory
;
;
;
; CATEGORY: Images (IO)
;
;
; CALLING SEQUENCE: data=read_dicom(filename,red,green,blue,[IMAGE_INDEX=index])
;
;
;
;
;
; KEYWORD PARAMETERS: 
;     IMAGE_INDEX : The image index to retrieve
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
; 	Written by: Christopher Lee 2004-05-26
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
f=filename
if(keyword_set(IMAGE_INDEX)) then f=filename+"["+string(IMAGE_INDEX)+"]"

mid=magick_open(f)

if(magick_IndexedColor(mid)) then begin
    data=magick_readIndexes(mid)
    magick_readcolormapRGB,mid,red,green,blue
endif else begin
    data=magick_read(mid)
endelse

magick_close,mid
return, data
end



