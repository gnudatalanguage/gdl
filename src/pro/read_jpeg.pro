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
;  Written by: Christopher Lee 2004-05-17
;  2006-May-02, Joel Gales    : Add convert to byte if 16-bit image
;  2011-Aug-18, Alain Coulais : More checks on inputs, verify if
;       compiled with ImageMagick support !
;  2011-Nov-09, Alain Coulais : correction for bug 3435468
;       Grayscale (2D case)
;  2012-Feb-07, Alain Coulais : new test cases in testsuite:
;   test_read_standard_images.pro : 2 JPEG and 4 PNG (2 with transparency)
;   The transpose for 2D image is no more need.
;
;-
; LICENCE:
; Copyright (C) 2004, 2011, 2012
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-
;

pro READ_JPEG, filename, unit=unit, image, colortable, buffer=buffer, $
               colors=ncolors, dither=dither, grayscale=grayscale, order=order, $
               true=true, two_pass_quantize=two_pass_quantize
  
compile_opt hidden, idl2
ON_ERROR, 2
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
; Cannot proceed further if:
if KEYWORD_SET(unit) then MESSAGE, "Keyword UNIT not supported"
if KEYWORD_SET(buffer) then MESSAGE, "Keyword BUFFER not supported"
;
if (N_ELEMENTS(filename) GT 1) then MESSAGE, "Only one file at once !"
if (STRLEN(filename) EQ 0) then MESSAGE, "Null filename not allowed."
;
if ((FILE_INFO(filename)).exists EQ 0) then MESSAGE, "Error opening file. File: "+filename
if (FILE_TEST(filename, /regular) EQ 0) then MESSAGE, "Not a regular File: "+filename
;
; testing whether the format is as expected
;
if ( ~MAGICK_PING(filename, 'JPEG') and ~MAGICK_PING(filename, 'JNG') )then begin
   MESSAGE, "JPEG error: Not a JPEG file."
endif
; if colortable IS present, irrespective of the presence of colors,
; will get a colormap, so will quantize the jpeg:
if (arg_present(colortable) && ~keyword_set(ncolors)) then ncolors=keyword_set(grayscale)?256:252; IDL default
READ_ANYGRAPHICSFILEWITHMAGICK, filename, image, colortable, colors=ncolors, $
                                dither=dither, grayscale=grayscale, order=order, $
                                true=true
end



