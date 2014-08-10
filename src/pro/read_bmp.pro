;+
;
; NAME: READ_BMP
;
; PURPOSE: Reads a bitmap file into memory
;
; CATEGORY: Images (IO)
;
; CALLING SEQUENCE: data=read_bmp(filename,red,green,blue,[RGB=RGB])
;
; KEYWORD PARAMETERS: 
;     RGB : Set to reverse the color ordering from BGR  
;
; OUTPUTS: For true color images, data is a three dimensional array
;          with pixel interleaving (i.e [3,columns,rows])
;
; OPTIONAL OUTPUTS: For pseudocolor only
;        red  : the Red colormap vector (for PseudoColor images)
;        green: the Green colormap vector (for PseudoColor images)
;        blue : the Blue colormap vector (for PseudoColor images)
;
; SIDE EFFECTS:
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
;
function READ_BMP, filename, red, green, blue, rgb=rgb, $
                   help=help, test=test, verbose=verbose
;
ON_ERROR, 2
;
; this line allows to compile also in IDL ...
FORWARD_FUNCTION MAGICK_EXISTS, MAGICK_PING, MAGICK_READ
;
if KEYWORD_SET(help) then begin
   print, 'function READ_BMP, filename, red, green, blue, rgb=rgb, $'
   print, '                   help=help, test=test, verbose=verbose'
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
if (N_ELEMENTS(filename) GT 1) then MESSAGE, "Only one file at once !"
;
mid=MAGICK_OPEN(filename)
;
if (MAGICK_INDEXEDCOLOR(mid)) then begin
   ;;
   data=MAGICK_READINDEXES(mid)
   MAGICK_READCOLORMAPRGB, mid, red, green, blue
   ;;
   ;; flip image
   sz  = SIZE( data, /DIM)
   rev = sz[1] - 1L - LINDGEN( sz[1])
   data = data[ *, rev]
   ;;
endif else begin
   ;;
   data=MAGICK_READ(mid,rgb=rgb)
   ;;
   ;; flip image
   sz  = SIZE( data, /DIM)
   rev = sz[2] - 1L - LINDGEN( sz[2])
   data = data[ *, rev, *]
   ;;
endelse
;
MAGICK_CLOSE, mid
;
if KEYWORD_SET(test) then stop
;
return, data
;
end



