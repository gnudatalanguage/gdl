;+
;
; NAME: READ_ANYGRAPHICSFILEWITHMAGICK
;
; PURPOSE: Reads any graphic file into memory using imagemagick
;
; CATEGORY: Images (IO)
;
; CALLING SEQUENCE: 
;      READ_ANYGRAPHICSFILEWITHMAGICK, filename, image, colortable
;
; KEYWORD PARAMETERS: 
;        COLORS: Number of colors to dither to (8->256)
;        DITHER: Method of dithering to use
;        GRAYSCALE: Return a grayscale image
;        ORDER: flip the image in the vertical if needed 
;        TRUE: Interleaving (1:pixel, 2:line, 3:band)
;
; OUTPUTS: For true color images, data is a three dimensional array
; with interleaving set by TRUE keyword
;
; OPTIONAL OUTPUTS: For pseudocolor only
;        COLORTABLE: the colortable produced (ncolors,3)
;
;
; SIDE EFFECTS:
;
;
; RESTRICTIONS:
;         Requires ImageMagick (that means that GDL must have been
;         compiled with ImageMagick)
;
; PROCEDURE:
;         Use ImageMagick to read the data as requested
;
; EXAMPLE: An image of Saturn should be around in the GDL CVS
;         "file=FILE_WHICH('Saturn.jpg')"
;         "READ_JPEG, file, image" --> calls just "READ_ANYGRAPHICSFILEWITHMAGICK, file, image"
;          after checking that file is of type 'JPG'.
;
; MODIFICATION HISTORY:
;  Written by: Copyright 2021 G. Duvert
;-
; LICENCE:
; Copyright (C) 2021
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-
;

pro READ_ANYGRAPHICSFILEWITHMAGICK, filename, image, colortable, $
               colors=ncolors, dither=dither, grayscale=grayscale, order=order, $
               true=true

  compile_opt hidden, idl2

  ON_ERROR, 2
   CATCH, Error_status
   IF Error_status NE 0 THEN BEGIN
      CATCH, /CANCEL
      MESSAGE, /REISSUE_LAST
      RETALL
   ENDIF
;
mid=MAGICK_OPEN(filename)

; to get something in colortable, colors need to be defined
if (KEYWORD_SET(ncolors)) then begin 
   ncolors = ncolors < 252 > 1 ; > 8
   if (~KEYWORD_SET(grayscale)) then  ncolors = ncolors < 32 > 1 ; to be refined. not clear.
   MAGICK_QUANTIZE, mid, ncolors, grayscale=grayscale, dither=dither
endif else if (KEYWORD_SET(grayscale)) then begin
   MAGICK_QUANTIZE, mid, /GRAYSCALE, dither=dither
endif
image=MAGICK_READ(mid); [3,n,m]
indexed=(magick_colormapsize(mid) gt 0) ; not the case with 
;;flip if order is set
if (KEYWORD_SET(order)) then MAGICK_FLIP, mid
;
if (indexed) then begin
   MAGICK_READCOLORMAPRGB,mid,r,g,b
   colortable=[[r],[g],[b]]
   ; apply palette compression, using r
   index=intarr(256)
   for i=0,n_elements(r)-1 do index[r[i]]=i
   image=reform(image[0,*,*])
   image[*]=index[image[*]]
endif

; if 16-bit (unsigned short int) image convert to byte
sz = SIZE(image)
type = sz[sz[0]+1]
if ((type EQ 2) OR (type EQ 12)) then begin
    print, 'Converting 16-bit image to byte'
    image = image / 256
    image = BYTE(image)
endif

if (sz[0] EQ 3) then begin
   if KEYWORD_SET(TRUE) then begin
      if (TRUE eq 1) then t=[0,1,2]
      if (TRUE eq 2) then t=[1,0,2]
      if (TRUE eq 3) then t=[1,2,0]
      ;;
      image=TRANSPOSE(image, t)
   endif
endif
;
end



