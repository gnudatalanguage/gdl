function READ_JPEG2000, filename, red, green, blue, order=order, discard_levels=discard_level, max_layers=max_layers, region=region
;

compile_opt hidden, idl2

ON_ERROR, 2

;+
;
; NAME: READ_JPEG2000
;
; PURPOSE: Reads a jpeg2000 file into memory
;
; CATEGORY: Images (IO)
;
; CALLING SEQUENCE: 
;      image=READ_JPEG2000(filename, [red, green, blue] [,/order])  
;
; KEYWORD PARAMETERS: 
;        ORDER: flip the image in the vertical 
;
; OUTPUTS: image is a three dimensional array [ncomponents,xsize,ysize]
; ncomponents must be 3???
;
; OPTIONAL OUTPUTS: For pseudocolor only
;        Red,Green,Blue: 3 color vectors
;
;
; SIDE EFFECTS:
;
;
; RESTRICTIONS:
;         Requires ImageMagick (that means that GDL must have been
;         compiled with ImageMagick AND jasper library)
;
; PROCEDURE:
;         Use ImageMagick to read the data as requested
;
; EXAMPLE: An image of Saturn should be around in the GDL CVS
;         file='test.jp2'
;         image=READ_JPEG(file)
;
; MODIFICATION HISTORY:
;  2013-Oct-09, GD.
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
; Do we have access to ImageMagick functionnalities ??
;
if (MAGICK_EXISTS() EQ 0) then begin
    MESSAGE, /continue, "GDL was compiled without ImageMagick support."
    MESSAGE, "You must have ImageMagick support to use this functionaly."
endif
;
if (N_PARAMS() NE 1 and N_PARAMS() ne 4 ) then MESSAGE, "Incorrect number of arguments."
;
if (N_ELEMENTS(filename) GT 1) then MESSAGE, "Only one file at once !"
if (STRLEN(filename) EQ 0) then MESSAGE, "Null filename not allowed."
;
if ((FILE_INFO(filename)).exists EQ 0) then MESSAGE, "Error opening file. File: "+filename
if (FILE_TEST(filename, /regular) EQ 0) then MESSAGE, "Not a regular File: "+filename
;
; testing whether the format is as expected
;
if ( ~MAGICK_PING(filename, 'JPC') and  ~MAGICK_PING(filename, 'JP2')  and  ~MAGICK_PING(filename, 'JNG') ) then  MESSAGE, "JPEG200 error: Not a JPEG2000 file."
;
READ_ANYGRAPHICSFILEWITHMAGICK, filename, image, colortable, order=order
if n_elements(colortable) gt 0 then begin
  red=colortable[*,0]
  green=colortable[*,1]
  blue=colortable[*,2]
endif
return, image

end



