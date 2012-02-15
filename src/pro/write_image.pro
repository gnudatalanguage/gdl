;+
;
; NAME: WRITE_IMAGE
;
; PURPOSE: write an image file (and colors tables) onto file
;
; CATEGORY: Images (IO)
;
; CALLING SEQUENCE: 
;      WRITE_IMAGE, filename, format, image, [red, green, blue], /append
;
; KEYWORD PARAMETERS: 
;        APPEND: not supported yet
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
;         WRITE_IMAGE, 'Saturn.png', 'png', image
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
pro WRITE_IMAGE, filename, format, image, red, green, blue, $
                 append=append, help=help, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro WRITE_IMAGE, filename, format, image, red, green, blue, $'
   print, '                 append=append, help=help, test=test'
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
if ((N_PARAMS() EQ 0) OR (N_PARAMS() GT 6)) then $
   MESSAGE, "Incorrect number of arguments."
;
if (STRLEN(filename) EQ 0) then MESSAGE, "Null filename not allowed."
;
case STRUPCASE(format) of
   'JPEG' : begin
      if (N_ELEMENTS(red) GT 0) then begin
         colortable=BYTARR(256,3)
         colortable[*,0]=red
         colortable[*,1]=green
         colortable[*,2]=blue
         WRITE_JPEG, filename, image, colortable, colors=256
      endif else begin
         WRITE_JPEG, filename, image
      endelse
   end
   'PNG' : WRITE_PNG, filename, image, red, green, blue
   'GIF' : WRITE_GIF, filename, image, red, green, blue
   else: MESSAGE, 'This format is not managed today, please contribute'
endcase
;
if KEYWORD_SET(test) then STOP
;
end
