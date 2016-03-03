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
; - Written by: Christopher Lee 2004-05-17
; - Modification by Alain Coulais 27-Aug-2011, changing q*1U to UINT(q)
;   to be able to do very simple End-To-End test on "Saturn.jpg" as input
; - Alain C., 11-Aug-2012: being able to write 2D Gray JPEG images.
;   Unfortunately, I was not able to find a nice way around IM/GM,
;   then I need to create fake [n,m,3] image and write it.
;   Help wanted to clean this.
;
;-
; LICENCE:
; Copyright (C) 2004, 2011, 2012; CL and AC
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
; this line allows to compile also in IDL ...
FORWARD_FUNCTION MAGICK_EXISTS, MAGICK_PING, MAGICK_READ
;
if ~KEYWORD_SET(debug) then ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
    print, 'pro WRITE_JPEG, filename, image, true=true, $'
    print, '               order=order, quality=quality, unit=unit,$'
    print, '               progressive=progressive, $'
    print, '               help=help, test=test, debug=debug'
    print, ''
    print, 'true unset or =0 implies image dims are: (n,m) pure 2D'
    print, 'true=1 implies image dims are: (3,n,m)'
    print, 'true=2 implies image dims are: (n,3,m)'
    print, 'true=3 implies image dims are: (n,m,3)'
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
;
if KEYWORD_SET(true) then begin
   if true LT 0 OR (true GT 3) then begin
      MESSAGE, "True Keyword outside valid range [0,1,2,3]"
   endif
endif
;
nb_dims=SIZE(image, /n_dimensions)
;
if (nb_dims LT 2) OR (nb_dims GT 3) then begin
   MESSAGE, "Image array must be (n,m) or (3,n,m), (n,3,m), (n,m,3)."
endif
;
if (nb_dims EQ 2) and KEYWORD_SET(true) then begin
   MESSAGE, /cont, "The Keyword_SET(true) is set but the Image array is 2D."
   MESSAGE, "If Keyword_SET(true), Image array must be (3,n,m), (n,3,m), (n,m,3)."
endif
;
; AC August 2012: I found no way to write well scaled grayscale JPEG :(
; I change the code is such a way it should not be to much a pain
; to write pure 2D Gray JPEG images in the future !
;
bad_trick=0
if (nb_dims EQ 2) then begin
   ;; temporary copy, please remove also the line at the end
   ref_img=image
   ;; this is a [n,m,3]
   image=[[[image]],[[image]],[[image]]]
   true=3
   nb_dims=SIZE(image, /n_dimensions)
   bad_trick=1
endif
;
if (nb_dims EQ 3) and ~KEYWORD_SET(true) then begin
   MESSAGE, /cont, "The Keyword /TRUE is not set but the Image array is 3D."
   MESSAGE, /cont, "You must provide a adequate value for True Keyword"
   MESSAGE, "if image is (3,n,m): true=1; (n,3,m): true=2; (n,m,3): true=3."
endif
;
im_size=SIZE(image,/dimensions)
; are the dims OK related to "True" keyword value ?
if (nb_dims EQ 3) then begin
   if ((true EQ 1) and (im_size[0] NE 3)) then MESSAGE, "expected True=1"
   if ((true EQ 2) and (im_size[1] NE 3)) then MESSAGE, "expected True=2"
   if ((true EQ 3) and (im_size[2] NE 3)) then MESSAGE, "expected True=3"
endif
;
if (KEYWORD_SET(TRUE)) then begin
    if (TRUE eq 1) then t=[0,1,2]
    if (TRUE eq 2) then t=[1,0,2]
    if (TRUE eq 3) then t=[2,0,1]
    image=TRANSPOSE(image, t)
endif
;
if KEYWORD_SET(test) then STOP
;
if (nb_dims EQ 2) then mid=MAGICK_CREATE(im_size[0],im_size[1])
if (nb_dims EQ 3) then mid=MAGICK_CREATE(im_size[1],im_size[2])
;
;generic
;
;if (nb_dims EQ 2) then MAGICK_QUANTIZE, mid, 256
;
;
rgb=1
MAGICK_WRITE, mid, image, rgb=rgb
;
if (KEYWORD_SET(progressive)) then $
  MAGICK_INTERLACE, mid, /PLANEINTERLACE
;
if (KEYWORD_SET(order)) then MAGICK_FLIP,mid
;
q=75
if (KEYWORD_SET(quality)) then q=quality
;
MAGICK_QUALITY, mid, UINT(q)
MAGICK_WRITEFILE, mid, filename, "JPEG"
MAGICK_CLOSE, mid
;
;reverse for returning initial order image !
if(KEYWORD_SET(TRUE)) then begin
   if(TRUE eq 1) then t=[0,1,2]
   if(TRUE eq 2) then t=[1,0,2]
   if(TRUE eq 3) then t=[1,2,0]
   ;;
   image=TRANSPOSE(image, t)
endif
;
; to be removed when pure 2D Gray images will be available
if (bad_trick EQ 1) then image=ref_img
;
if KEYWORD_SET(test) OR KEYWORD_SET(debug) then STOP
;
end
