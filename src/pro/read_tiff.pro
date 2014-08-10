;$Id: read_tiff.pro,v 1.4 2014-08-10 06:54:54 alaingdl Exp $
;
function READ_TIFF, filename, red, green, blue, channels=channels, $
                    geotiff=geotiff, image_index=image_index, $
                    interleave=interleave, orientation=orientation, $
                    planarconfig=planarconfig, sub_rect=sub_rect, $
                    verbose=verbose
;+
;
;
;
; NAME: READ_TIFF
;
; PURPOSE: Reads a tiff file into memory
;
; CATEGORY: Images (IO)
;
; CALLING SEQUENCE: image=read_tiff(filename,r,g,b)
;
; KEYWORD PARAMETERS: 
;        CHANNELS: (scalar or vector) specify which channels to
;                  retrieve
;        GEOTIFF: Returns a geotiff structure
;        IMAGE_INDEX: select the image number to read
;        INTERLEAVE: pixel:0, row:1, column: 2
;        ORIENTATION: Named variable whose value depends on the
;                   rotation of the image in the file 
;                   row 0, column 0 is
;                    0- bottom left
;                    1- top left
;                    2- top right
;                    3- bottom right
;                    4- bottom left
;                    5- left top
;                    6- right top
;                    7- right bottom
;                    8- left bottom
;
;        PLANARCONFIG: Named variable describing the interleave of the
;                      file
;        SUB_RECT : Four element vector describing the sub rectangle
;                   to read in [x,y,width,height], from the lower left
;                   hand corner
;        VERBOSE: Be more verbose
;        
;
; OUTPUTS: For true color images, data is a three dimensional array
; with interleaving set by the INTERLEAVE keyword
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
; MODIFICATION HISTORY:
; 	Written by: Christopher Lee 2004-05-23
;       Small revision by Alain Coulais, 2014-08-09
;
;-
; LICENCE:
; Copyright (C) 2004, 2014
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;
;-
;
ON_ERROR, 2
;
; this line allows to compile also in IDL ...
FORWARD_FUNCTION MAGICK_READ
;
;
; Do we have access to ImageMagick functionnalities ??
;
if (MAGICK_EXISTS() EQ 0) then begin
    MESSAGE, /continue, "GDL was compiled without ImageMagick support."
    MESSAGE, "You must have ImageMagick support to use this functionaly."
endif


if (N_ELEMENTS(filename) GT 1) then MESSAGE, "Only one file at once !"

f=filename
if(keyword_set(IMAGE_INDEX)) then f=filename+"["+string(IMAGE_INDEX)+"]"
mid=magick_open(f)


;if(keyword_set(VERBOSE)) then message, "VERBOSE Keyword not used yet."
;message, "Orientation and planarconfig may not be accurate."
print, "Orientation and planarconfig may not be accurate."

orientation=0
planarconfig=1


if(magick_IndexedColor(mid)) then begin
    image=magick_readIndexes(mid)
    magick_readcolormapRGB,mid,red,green,blue
    colortable=[[red],[green],[blue]]
endif else begin
    m=["R","G","B","A"]
    if(n_elements(channels)) then begin
        map=m[channels]
    endif
    image=magick_read(mid,sub_rect=sub_rect)
    if(keyword_set(interleave)) then begin
        if (interleave eq 0) then image=transpose(image, [0,1,2])
        if (interleave eq 1) then image=transpose(image, [1,0,2])
        if (interleave eq 2) then image=transpose(image, [1,2,0])
    endif
endelse

magick_close,mid
return,image

end



