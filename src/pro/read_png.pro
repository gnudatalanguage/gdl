;+
;
; NAME: READ_PNG
;
; PURPOSE: Reads a PNG file into memory (Function OR Procedure)
;
; CATEGORY: Images (IO)
;
; CALLING SEQUENCE: 2 ways: Pro or Func
;
;   Function:  image=READ_PNG(filename,r,g,b)
;   Procedure: READ_PNG, filename, image, r,g,b
;
; KEYWORD PARAMETERS: 
;        ORDER: flip the image in the vertical 
;        VERBOSE: Not used yet
;        TRANSPARENT: Transparent
;
; OUTPUTS: For true color images, data is a three dimensional array
; with interleaving set by TRUE keyword
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
;         See "test_read_standard_images.pro" in testsuite/
;
; MODIFICATION HISTORY:
;  Written by: Christopher Lee 2004-05-23
;  2011-Aug-18, Alain Coulais : More checks on inputs; now verify if
;      compiled with ImageMagick support !
;  2012-Feb-02, Alain Coulais :  the effective order of reading was bad ...
;      now it is OK on all tested PNG images, including images with transparency
;      see 3 examples:
;  * testsuite/Saturn.jpg default conversion by Image Magick in PNG
;  * http://www.gnu.org/graphics/meditate_fel.png (big, no transparency)
;  * http://www.gnu.org/graphics/meditate.png (transparency)
;
;  2012-Feb-07, Alain Coulais : new test cases in testsuite:
;   test_read_standard_images.pro : 2 JPEG and 4 PNG (2 with transparency)
;   The transpose for 2D image is no more need.
;
;  2012-May-25, Alain Coulais : fake INTERNAL_READ_PNG to have both
;   pro/func working transparently without pre-compilation
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
function INTERNAL_READ_PNG, filename, red, green, blue, $
                            order=order, transparent=transparent, $
                            test=test, verbose=verbose

; Do we have access to ImageMagick functionnalities ??
;
if (MAGICK_EXISTS() EQ 0) then begin
    MESSAGE, /continue, "GDL was compiled without ImageMagick support."
    MESSAGE, "You must have ImageMagick support to use this functionaly."
endif
;
if (N_PARAMS() EQ 0) then MESSAGE, "Incorrect number of arguments."
if ~((N_PARAMS() EQ 1) OR (N_PARAMS() EQ 4)) then $
   MESSAGE, "Only 1 or 4 arguments allowed."
;
if (SIZE(filename,/type) NE 7) then MESSAGE, "String expression required in this context: filename"
if (N_ELEMENTS(filename) GT 1) then MESSAGE, "Only one file at once !"
if (STRLEN(filename) EQ 0) then MESSAGE, "Null filename not allowed."
if ((FILE_INFO(filename)).exists EQ 0) then MESSAGE, "Error opening file. File: "+filename
if (FILE_TEST(filename, /regular) EQ 0) then MESSAGE, "Not a regular File: "+filename
;
; testing whether the format is as expected
;
if ~MAGICK_PING(filename, 'PNG') then begin
   MESSAGE, /continue, "PNG error: Not a PNG file:"
   if MAGICK_PING(filename, 'JPEG') then MESSAGE, "seems to be a JPEG file"
   if MAGICK_PING(filename, 'GIF') then MESSAGE, "seems to be a GIF file"
   if MAGICK_PING(filename, 'PDF') then MESSAGE, "seems to be a PDF file"
   MESSAGE, "unknown/untested format file"   
endif
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
   ;; AC 2012-Feb-02 the effective order of reading was bad ...
   ;; now it is OK on all tested PNG images, including images with transparency
   image=MAGICK_READ(mid, rgb=1)
endelse
;
MAGICK_CLOSE, mid
;
if KEYWORD_SET(test) then STOP
;
return, image
;
end
;
; ----------------------------- Procedure ------------------------
;
pro READ_PNG, filename, image, red, green, blue, $
              order=order, transparent=transparent, $
              help=help, test=test, verbose=verbose
;
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
   print, 'pro READ_PNG, filename, red, green, blue, $'
   print, '              order=order, transparent=transparent, $'
   print, '              help=help, test=test, verbose=verbose'
   return
endif
;
image=INTERNAL_READ_PNG(filename, red, green, blue, $
                        order=order, transparent=transparent, $
                        test=test, verbose=verbose)
;
end
;
; ----------------------------- Function ------------------------
;
function READ_PNG, filename, red, green, blue, $
                   order=order, transparent=transparent, $
                   help=help, test=test, verbose=verbose
;
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
   print, 'function READ_PNG, filename, red, green, blue, $'
   print, '                   order=order, transparent=transparent, $'
   print, '                   help=help, test=test, verbose=verbose'
   return, -1
endif
;
image=INTERNAL_READ_PNG(filename, red, green, blue, $
                        order=order, transparent=transparent, $
                        test=test, verbose=verbose)
;
return, image
;
end
