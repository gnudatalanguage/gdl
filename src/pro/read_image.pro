;+
;
; NAME: READ_IMAGE
;
; PURPOSE: Reads a image file (and colors tables) into memory
;
; CATEGORY: Images (IO)
;
; CALLING SEQUENCE: 
;      READ_IMAGE, filename, [red, green, blue], image_index=image_index, $
;                 help=help, test=test
;
; OUTPUTS: [n,m], [3,n,m] with optional r,g,b if the image format is
; indexed color (has a palette)
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
;  2022-Jul-11, GD
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
function READ_IMAGE, filename, red, green, blue, image_index=image_index

compile_opt hidden, idl2

ON_ERROR, 2

; if !MAGIC_EXIST, these procedures should not be present in the !PATH
; at all (not installed)
; Do we have access to ImageMagick functionnalities ??
;
if (MAGICK_EXISTS() EQ 0) then begin
    MESSAGE, /continue, "GDL was compiled without ImageMagick support."
    MESSAGE, "You must have ImageMagick support to use this functionality."
 endif
;
if ((N_PARAMS() EQ 0) OR (N_PARAMS() GT 4)) then $
   MESSAGE, "Incorrect number of arguments."
;
if (N_ELEMENTS(filename) GT 1) then MESSAGE, "Only one file at once !"
;
; First, we have to test whether the file is here
;
status=QUERY_IMAGE(filename, info)
;
if (status EQ 0) then begin
   MESSAGE, /INFO, 'Not a valid image file: '+filename
   return, -1
endif
; if query_image said it's OK, just use read_anything:
READ_ANYGRAPHICSFILEWITHMAGICK, filename, image, colortable
if n_elements(colortable) gt 0 then begin
  red=colortable[*,0]
  green=colortable[*,1]
  blue=colortable[*,2]
endif
;
return, image
;
end
