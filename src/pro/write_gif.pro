;+
;
; NAME: WRITE_GIF
;
; PURPOSE: write a image from memory to a GIF
;
; CATEGORY: Images (IO)
;
; CALLING SEQUENCE: 
;    WRITE_GIF, filename, image, Red, Green, Blue, $
;               background_color=background_color, close=close, $
;               delay_time=delay_time, disposal_method=disposal_method, $
;               multiple=multiple, repeat_count=repeat_count, $
;               transparent=transparent, user_input=user_input,                
;               test=test, help=help, debug=debug
;
; KEYWORD PARAMETERS:
;      Except very basic output, nothing is supported now, please contribute !
;
; OPTIONAL INPUTS: For pseudocolor only
;        red  : the Red colormap vector (for PseudoColor images)
;        green: the Green colormap vector (for PseudoColor images)
;        blue : the Blue colormap vector (for PseudoColor images)
;
; RESTRICTIONS:
;         Requires ImageMagick (tested)
;         Most Keywords are not operational now.
;
; PROCEDURE:
;         Use ImageMagick to write the data as requested
;
; EXAMPLE:
;   READ_JPEG, 'testsuite/Saturn.jpg', image
;   WRITE_GIF, 'Saturn2.gif', image
;
; MODIFICATION HISTORY:
;  Written by: Alain Coulais 2011-11-30
;  Derived work from WRITE_JPEG by: Christopher Lee 2004-05-17
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
pro GIF_MESSAGE, mot_clef
MESSAGE, /Continue, 'This Keyword '+STRUPCASE(mot_clef)+' is not operational'
MESSAGE, /Continue, 'Please contribute !!'
end
;
pro WRITE_GIF, filename, image, Red, Green, Blue, $
               background_color=background_color, close=close, $
               delay_time=delay_time, disposal_method=disposal_method, $
               multiple=multiple, repeat_count=repeat_count, $
               transparent=transparent, user_input=user_input, $
               test=test, help=help, debug=debug
;
; this line allows to compile also in IDL ...
FORWARD_FUNCTION MAGICK_EXISTS, MAGICK_PING, MAGICK_READ
;
if ~KEYWORD_SET(debug) then ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
   print, 'pro WRITE_GIF, filename, image, Red, Green, Blue, $'
   print, '               background_color=background_color, close=close, $'
   print, '               delay_time=delay_time, disposal_method=disposal_method, $'
   print, '               multiple=multiple, repeat_count=repeat_count, $'
   print, '               transparent=transparent, user_input=user_input, $'
   print, '               help=help, test=test, debug=debug'
   return
endif
nr=n_elements(red)
ng=n_elements(green)
nb=n_elements(blue)
if (nr ne 0) then do_rgb=1 else do_rgb=0
if (do_rgb && ng eq 0) then Message,'usage: WRITE_GIF, file, image, [red, green, blue]' 
if (do_rgb && nb eq 0) then Message,'usage: WRITE_GIF, file, image, [red, green, blue]' 
if (do_rgb && ng ne nr) then Message,'Red, Green, & Blue must all have the same length.'
if (do_rgb && nb ne nr) then Message,'Red, Green, & Blue must all have the same length.'
; Do we have access to ImageMagick functionnalities ??
;
if (MAGICK_EXISTS() EQ 0) then begin
   MESSAGE, /continue, "GDL was compiled without ImageMagick support."
   MESSAGE, "You must have ImageMagick support to use this functionaly."
endif
;
if KEYWORD_SET(background_color) then GIF_MESSAGE, 'background_color'
if KEYWORD_SET(close) then GIF_MESSAGE, 'close'
if KEYWORD_SET(delay_time) then GIF_MESSAGE, 'delay_time'
if KEYWORD_SET(disposal_method) then GIF_MESSAGE, 'disposal_method'
if KEYWORD_SET(multiple) then GIF_MESSAGE, 'multiple'
if KEYWORD_SET(user_input) then GIF_MESSAGE, 'user_input'

n=SIZE(image, /n_dimensions)
s=SIZE(image, /dimensions)
;
if KEYWORD_SET(test) then STOP
;
if (n NE 2) then begin
   MESSAGE, 'Image must be a byte matrix'
endif
;
mid=MAGICK_CREATE(s[0],s[1])
; add colortable
if (do_rgb) then MAGICK_WRITECOLORTABLE,mid,red,green,blue else MAGICK_WRITECOLORTABLE,mid
if ( N_ELEMENTS(transparent) eq 1 && n eq 2 ) then MAGICK_MATTECOLOR, mid, uint(transparent)
MAGICK_WRITE, mid, image
MAGICK_WRITEFILE, mid, filename,"GIF"
MAGICK_CLOSE, mid
;
if KEYWORD_SET(test) then STOP
;
end




