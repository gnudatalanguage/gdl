;$Id: read_jpeg.pro,v 1.10 2012-01-28 01:16:07 alaingdl Exp $

pro READ_JPEG, filename, unit=unit, image, colortable, buffer=buffer, $
               colors=colors, dither=dither, grayscale=grayscale, order=order, $
               true=true, two_pass_quantize=two_pass_quantize, $
               help=help, test=test, debug=debug
;
ON_ERROR, 2
;+
;
; NAME: READ_JPEG
;
; PURPOSE: Reads a jpeg file into memory
;
; CATEGORY: Images (IO)
;
; CALLING SEQUENCE: 
;      READ_JPEG, filename, image, colortable
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
;         file=FILE_WHICH('Saturn.jpg')
;         READ_JPEG, file, image
;
; MODIFICATION HISTORY:
;    Written by: Christopher Lee 2004-05-17
;    2006-May-02, Joel Gales    : Add convert to byte if 16-bit image
;    2011-Aug-18, Alain Coulais : More checks on inputs, verify if
;       compiled with ImageMagick support !
;    2011-Nov-09, Alain Coulais : correction for bug 3435468
;       Grayscale (2D case)
;
;-
; LICENCE:
; Copyright (C) 2004, 2011
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-
;
if KEYWORD_SET(help) then begin
    print, 'pro READ_JPEG, filename, unit=unit, image, colortable, buffer=buffer, $'
    print, '               colors=colors, dither=dither, grayscale=grayscale, order=order, $'
    print, '               true=true, two_pass_quantize=two_pass_quantize, $'
    print, '               help=help, test=test, debug=debug'
    return
endif
;
; Do we have access to ImageMagick functionnalities ??
;
if (MAGICK_EXISTS() EQ 0) then begin
    MESSAGE, /continue, "GDL was compiled without ImageMagick support."
    MESSAGE, "You must have ImageMagick support to use this functionaly."
endif
;
; AC 2011-Aug-18: this test will be wrong when UNIT will be available
if (N_PARAMS() EQ 0) then MESSAGE, "Incorrect number of arguments."
;
if (STRLEN(filename) EQ 0) then MESSAGE, "Null filename not allowed."
if ((FILE_INFO(filename)).exists EQ 0) then MESSAGE, "Error opening file. File: "+filename
if (FILE_TEST(filename, /regular) EQ 0) then MESSAGE, "Not a regular File: "+filename
;
if KEYWORD_SET(unit) then MESSAGE, "Keyword UNIT not supported"
if KEYWORD_SET(buffer) then MESSAGE, "Keyword BUFFER not supported"
;
if (not KEYWORD_SET(unit)) then mid=MAGICK_OPEN(filename)
;
;;DITHER if necessary
if (KEYWORD_SET(grayscale)) then begin
    MAGICK_QUANTIZE, mid, /GRAYSCALE
endif else begin
   if (KEYWORD_SET(colors)) then begin
      if ((colors LT 8) OR (colors GT 256)) then MESSAGE, "COLORS must be in the range 8 to 256"
      if (KEYWORD_SET(two_pass_quantize)) then MESSAGE, "TWO_PASS_QUANTIZE not supported by ImageMagick."
      MAGICK_QUANTIZE, mid, colors, dither=dither
   endif
endelse
;
;;flip if order is set
if (KEYWORD_SET(order)) then MAGICK_FLIP, mid
;
if (MAGICK_INDEXEDCOLOR(mid)) then begin
    image=MAGICK_READINDEXES(mid)
    MAGICK_READCOLORMAPRGB, mid, red, green, blue
    colortable=[[red],[green],[blue]]
    ;;
    ;; try to catch a problem in ImageMagick
    ;; (should be renormalized in, but not, as is on 28/01/2012)
    ;; bug report 3471918 (see min/max)
    if (KEYWORD_SET(grayscale)) then begin
       temp=image
       for ii=0, N_ELEMENTS(red)-1 do begin
          ok=WHERE(image EQ ii, nbp)
          if nbp GT 0 then temp[OK]=red[ii]
       endfor
       image=temp
    endif
endif else begin
    image=MAGICK_READ(mid)
endelse
;
if KEYWORD_SET(debug) then STOP
;
; if 16-bit (unsigned short int) image convert to byte
sz = SIZE(image)
type = sz[sz[0]+1]
if ((type EQ 2) OR (type EQ 12)) then begin
    print, 'Converting 16-bit image to byte'
    image = image / 256
    image = BYTE(image)
endif
;
if (not KEYWORD_SET(unit)) then MAGICK_CLOSE, mid
;
if (sz[0] EQ 2) then begin
   image=ROTATE(image,7)
endif
if (sz[0] EQ 3) then begin
   ;; "rotate" image to agree with IDL (JMG 08/18/04)
   tmp = image[0,*,*]
   image[0,*,*] = image[2,*,*]
   image[2,*,*] = tmp
   ;;
   if KEYWORD_SET(TRUE) then begin
      if (TRUE eq 1) then t=[0,1,2]
      if (TRUE eq 2) then t=[1,0,2]
      if (TRUE eq 3) then t=[1,2,0]
      ;;
      image=TRANSPOSE(image, t)
      ;;  image=transpose(image[[2,1,0],*,*], t)
   endif
endif
if (sz[0] GT 3) then begin
    MESSAGE, /continue, $
             "Dimensions of image > 3 : we don't know how to process now"
endif
;else begin
;    image = image[[2,1,0],*,*]
;endelse
;
if KEYWORD_SET(test) then STOP
;
end



