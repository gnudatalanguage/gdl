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

pro READ_ANYGRAPHICSFILEWITHMAGICK, filename, image, colortable, colors=ncolors, dither=dither, grayscale=grayscale,background_color=bgc, order=order, true=true

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
  
  if (keyword_set(order)) then magick_flip,mid

  if (KEYWORD_SET(ncolors)) then begin 
     if (KEYWORD_SET(grayscale)) then ncolors = ncolors < 256 > 1 else ncolors = ncolors < !D.N_COLORS > 1 ; 252 IDL default
     MAGICK_QUANTIZE, mid, ncolors, grayscale=grayscale, dither=dither
  endif else if (KEYWORD_SET(grayscale)) then begin
     MAGICK_QUANTIZE, mid, /GRAYSCALE, dither=dither
  endif
  indexed=(magick_class(mid) eq "PseudoClass") ; new command
  if (indexed) then begin
     image=MAGICK_READINDEXES(mid) ; may be [2,n,m] --> unsupported by IDL
     if ((size(image))[0] eq 3) then image=reform(image[0,*,*]) ; get only index image, not alpha channel
     MAGICK_READCOLORMAPRGB,mid,r,g,b,background_color=bgc
     colortable=[[r],[g],[b]]
  endif else begin
     image=MAGICK_READ(mid)     ; just read! [3,n,m] or [4,n,m]
  endelse
  
; no more use of MAGICK? close it:
  MAGICK_CLOSE,mid
  
  sz = SIZE(image)
  
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



