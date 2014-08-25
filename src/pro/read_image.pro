;+
;
; NAME: READ_IMAGE
;
; PURPOSE: Reads a image file (and colors tables) into memory
;
; CATEGORY: Images (IO)
;
; CALLING SEQUENCE: 
;      READ_JPEG, filename, red, green, blue, image_index=image_index, $
;                 help=help, test=test
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
; OUTPUTS: [n,m], [2,n,m], [3,n,m], [4,n,m] following image properties
;          (transparency adds one extra Dim)
;
; OPTIONAL OUTPUTS: For pseudocolor only: Red, Green, Blue
;
; SIDE EFFECTS: 
;
; RESTRICTIONS:
;         Requires ImageMagick (that means that GDL must have been
;         compiled with ImageMagick)
;
; PROCEDURE:
;         Use ImageMagick to read the data as requested
;
; EXAMPLE: An image of Saturn should be around in the GDL CVS
;
;         file=FILE_WHICH('Saturn.jpg')
;         image=READ_IMAGE(file, image)
;         TV, image, /true
;
; MODIFICATION HISTORY:
;  Initial version written by: Alain Coulais, 2012-02-15
;  2012-Feb-12, Alain Coulais :
;
;-
; LICENCE:
; Copyright (C) 2012
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-
;
function READ_IMAGE, filename, red, green, blue, $
                     image_index=image_index, $
                     help=help, test=test
;
ON_ERROR, 2
;
; this line allows to compile also in IDL ...
FORWARD_FUNCTION MAGICK_EXISTS, MAGICK_PING, MAGICK_READ
;
if KEYWORD_SET(help) then begin
   print, 'function READ_IMAGE, filename, red, green, blue, $'
   print, '                    image_index=image_index, $'
   print, '                    help=help, test=test'
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
if ((N_PARAMS() EQ 0) OR (N_PARAMS() GT 4)) then $
   MESSAGE, "Incorrect number of arguments."
;
if (N_ELEMENTS(filename) GT 1) then MESSAGE, "Only one file at once !"
if (STRLEN(filename) EQ 0) then MESSAGE, "Null filename not allowed."
if ((FILE_INFO(filename)).exists EQ 0) then MESSAGE, "Error opening file. File: "+filename
if (FILE_TEST(filename, /regular) EQ 0) then MESSAGE, "Not a regular File: "+filename
;
; First, we have to test whether the file is here
;
status=QUERY_IMAGE(filename, info)
;
if (status EQ 0) then begin
   MESSAGE, 'Not a valid image file: '+filename
endif
;
case info.type of
   'JPEG' : begin
      if (info.has_palette EQ 1) then begin
         READ_JPEG, filename, image, colortable, colors=256
         red=REFORM(colortable[*,0])
         green=REFORM(colortable[*,1])
         blue=REFORM(colortable[*,2])
      endif else begin
         READ_JPEG, filename, image
      endelse
   end
   'PNG' : READ_PNG, filename, image, red, green, blue
   'GIF' : READ_GIF, filename, image, red, green, blue
   else: MESSAGE, 'This format is not managed today, please contribute'
endcase
;
if KEYWORD_SET(test) then STOP
;
return, image
;
end
