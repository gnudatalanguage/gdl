;+
;
; NAME: WRITE_JPEG
;
; PURPOSE: write a image from memory to a jpeg
;
; CATEGORY: Images (IO)
;
; CALLING SEQUENCE: 
;    WRITE_JPEG, filename, image, true=true, $
;                order=order, quality=quality, unit=unit, $
;                progressive=progressive, $
;                test=test, help=help, debug=debug
;                     
;
; KEYWORD PARAMETERS: 
;     UNIT        : not supported
;     ORDER       : 0 for bottom to top ,1 for top to bottom
;     PROGRESSIVE : 
;     QUALITY     : default is 75
;     TRUE        ; 1=pixel interleaving, 2=row, 3=column interleaving
;           
;
; OPTIONAL INPUTS: For pseudocolor only
;        red  : the Red colormap vector (for PseudoColor images)
;        green: the Green colormap vector (for PseudoColor images)
;        blue : the Blue colormap vector (for PseudoColor images)
;
;
; RESTRICTIONS:
;         Requires ImageMagick (tested)
;
; PROCEDURE:
;         Use ImageMagick to write the data as requested
;
; EXAMPLE: 
;   READ_JPEG, 'testsuite/Saturn.jpg', image
;   WRITE_JPEG, 'Saturn2.jpg', image
;
;
; MODIFICATION HISTORY:
;  Written by: Christopher Lee 2004-05-17
;  Modification by Alain Coulais 27-Aug-2011, changing q*1U to UINT(q)
;  to be able to do very simple End-To-End test on "Saturn.jpg" as input
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
pro WRITE_JPEG, filename, image, true=true, $
                order=order, quality=quality, unit=unit,$
                progressive=progressive, $
                test=test, help=help, debug=debug
;
if ~KEYWORD_SET(debug) then ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
    print, 'pro WRITE_JPEG, filename, image, true=true, $'
    print, '               order=order, quality=quality, unit=unit,$'
    print, '               progressive=progressive, $'
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
rgb=1
;
if (KEYWORD_SET(unit)) then begin
    print, "UNIT not supported"
    return
endif

if (KEYWORD_SET(TRUE)) then begin
    if (TRUE eq 1) then t=[0,1,2]
    if (TRUE eq 2) then t=[1,0,2]
    if (TRUE eq 3) then t=[2,0,1]
    image=TRANSPOSE(image, t)
endif

n=SIZE(image, /n_dimensions)
s=SIZE(image, /dimensions)
;
if KEYWORD_SET(test) then STOP
q=75
;
;generic
mid=MAGICK_CREATE(s[1],s[2])
MAGICK_WRITE, mid, image,rgb=rgb

if (KEYWORD_SET(progressive)) then $
  MAGICK_INTERLACE, mid, /PLANEINTERLACE

if (KEYWORD_SET(order)) then MAGICK_FLIP,mid
if (KEYWORD_SET(quality)) then q=quality

MAGICK_QUALITY, mid, UINT(q)
MAGICK_WRITEFILE, mid, filename,"JPEG"
MAGICK_CLOSE, mid

;reverse
if(KEYWORD_SET(TRUE)) then begin
    if(TRUE eq 1) then t=[0,1,2]
    if(TRUE eq 2) then t=[1,0,2]
    if(TRUE eq 3) then t=[1,2,0]
    
    image=TRANSPOSE(image, t)
endif

if KEYWORD_SET(test) then STOP


end



