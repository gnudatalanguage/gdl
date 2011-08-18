;$Id: read_png.pro,v 1.3 2011-08-18 17:26:43 alaingdl Exp $
;
pro READ_PNG, filename, image, red, green, blue, $
              order=order, verbose=verbose, transparent=transparent, $
              help=help, test=test
;
image=READ_PNG(filename, red, green, blue, $
              order=order, verbose=verbose, transparent=transparent, $
              help=help, test=test)
;
end
;
; ---------------------------------
;
function READ_PNG, filename, red, green, blue, $
                   order=order, verbose=verbose, transparent=transparent, $
                   help=help, test=test
;
ON_ERROR, 2
;+
;
; NAME: READ_PNG
;
;
; PURPOSE: Reads a PNG file into memory
;
; CATEGORY: Images (IO)
;
;
; CALLING SEQUENCE: image=READ_PNG(filename,r,g,b)
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
;         Requires ImageMagick (tests added)
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
;       2011-Aug-18, Alain Coulais : More checks on inputs; now verify if
;       compiled with ImageMagick support !
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
;
if KEYWORD_SET(help) then begin
    print, 'function READ_PNG, filename, red, green, blue, $'
    print, '                   order=order, verbose=verbose, transparent=transparent, $'
    print, '                   help=help, test=test'
    return, -1
endif
;
; Do we have access to ImageMagick functionnalities ??
;
if (MAGICK_EXISTS() EQ 0) then begin
    MESSAGE, /continue, "GDL was compiled without ImageMagick support."
    MESSAGE, "You must have ImageMagick support to use this functionaly."
endif
;
if (N_PARAMS() EQ 0) then MESSAGE, "Incorrect number of arguments."
;
if (STRLEN(filename) EQ 0) then MESSAGE, "Null filename not allowed."
if ((FILE_INFO(filename)).exists EQ 0) then MESSAGE, "Error opening file. File: "+filename
;
mid=MAGICK_OPEN(filename)
;
;;flip if order is set
;
if (KEYWORD_SET(order)) then MAGICK_FLIP, mid

if (magick_IndexedColor(mid)) then begin
    image=MAGICK_READINDEXES(mid)
    MAGICK_READCOLORMAPRGB, mid, red, green, blue
    colortable=[[red],[green],[blue]]
endif else begin
    image=MAGICK_READ(mid)
endelse
;
MAGICK_CLOSE, mid
;
if KEYWORD_SET(test) then STOP
;
return, image
;
end

