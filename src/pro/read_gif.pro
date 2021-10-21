;+
;
; NAME: READ_GIF
;
; PURPOSE: Reads a GIF file into memory. This is a very limited
; version, working on a 256 colors image.
;
; CATEGORY: Images (IO)
;
; CALLING SEQUENCE: 
;      READ_GIF, filename, image, Red, Green, Blue
;
; KEYWORD PARAMETERS: 
;
; OUTPUTS: For true color images, data is a three dimensional array
; with interleaving set by TRUE keyword
;
; OPTIONAL OUTPUTS: Red, Green, Blue
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
;         WRITE_GIF, 'Saturn.gif', image
;         READ_GIF, 'Saturn.gif', image2, r, g, b
;         TVLCT, r,g,b
;         TV, image2
;
; MODIFICATION HISTORY:
;    Written by: Alain Coulais : 2011-11-30, as a derivative work from READ_JPEG
;-
; LICENCE:
; Copyright (C) 2011
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-
;
pro READ_GIF, filename, image, Red, Green, Blue, background_color=bgc, close=close, delay_time=dtime,$
              disposal_method=dispm, multiple=mult, repeat_count=rcount, transparent=transp, user_input= user_input

compile_opt hidden, idl2
ON_ERROR, 2

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
if (N_ELEMENTS(filename) GT 1) then MESSAGE, "Only one file at once !"
if (STRLEN(filename) EQ 0) then MESSAGE, "Null filename not allowed."
if ((FILE_INFO(filename)).exists EQ 0) then MESSAGE, "Error opening file. File: "+filename
if (FILE_TEST(filename, /regular) EQ 0) then MESSAGE, "Not a regular File: "+filename
;
if ( ~MAGICK_PING(filename, 'GIF') and ~MAGICK_PING(filename, 'GIF87') )then MESSAGE, "error: Not a GIF file."

READ_ANYGRAPHICSFILEWITHMAGICK, filename, image, colortable, background_color=bgc
if ( n_elements(colortable) eq 0 ) then Message,"GIF file is not a 8 bit GIF"
red=colortable[*,0]
green=colortable[*,1]
blue=colortable[*,2]
; it happens that magick sends back a truecolor image, when IDL would
; just send back a 2D index image. Do as IDL does:
if (size(image))[0] eq 3 then image=reform(image[0,*,*])
end



