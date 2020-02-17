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
  dotrue=keyword_set(true)
;
  if (KEYWORD_SET(unit)) then begin
     print, "UNIT not supported"
     return
  endif
;
  if dotrue then begin
     if true LT 0 OR (true GT 3) then begin
        MESSAGE, "Value of TRUE keyword is out of allowed range."
     endif
  endif
;
  nb_dims=SIZE(image, /n_dimensions)
;
  if (nb_dims LT 2) OR (nb_dims GT 3) then begin
     MESSAGE, "Image array must be (n,m) or (3,n,m), (n,3,m), (n,m,3)."
  endif
;
  if (nb_dims EQ 2) and dotrue then begin
     MESSAGE, "Array must have 3 dimensions: "+scope_varname(image,lev=-1)
  endif
  if (nb_dims EQ 3) and ~dotrue then begin
     MESSAGE, "Array must have 2 dimensions: "+scope_varname(image,lev=-1)
  endif
;
  im_size=SIZE(image,/dimensions)
; are the dims OK related to "True" keyword value ?
  variants=['(3, m, n)','(m, 3, n)','(m, n, 3)']
  transporder=[[0,1,2],[1,0,2],[2,0,1]]
  if ( nb_dims EQ 3 && dotrue && im_size[true-1] NE 3 ) then MESSAGE, "Array must have dimensions of "+variants[true-1]+" : "+scope_varname(image)
;
  if dotrue then begin
     image2=TRANSPOSE(image, transporder[*,true-1])
     mid=MAGICK_CREATE(im_size[1],im_size[2]) ; only 3 dimensions allowed here.
     MAGICK_WRITE, mid, temporary(image2), rgb=1
     if (KEYWORD_SET(progressive)) then MAGICK_INTERLACE, mid, /PLANEINTERLACE
     if (KEYWORD_SET(order)) then MAGICK_FLIP,mid
     q=75
     if (KEYWORD_SET(quality)) then q=quality
     MAGICK_QUALITY, mid, UINT(q)
     MAGICK_WRITEFILE, mid, filename, "JPEG"
     MAGICK_CLOSE, mid
  endif else begin
     mid=MAGICK_CREATE(im_size[0],im_size[1]) ; only 2 dimensions allowed here
                                ; create a greyscale colormap
     cmap=indgen(256)
     MAGICK_WRITECOLORTABLE, mid,cmap,cmap,cmap
     MAGICK_WRITE, mid, image
     if (KEYWORD_SET(progressive)) then MAGICK_INTERLACE, mid, /LINEINTERLACE
     if (KEYWORD_SET(order)) then MAGICK_FLIP,mid
     q=75
     if (KEYWORD_SET(quality)) then q=quality
     MAGICK_QUALITY, mid, UINT(q)
     MAGICK_WRITEFILE, mid, filename, "JPEG"
     MAGICK_CLOSE, mid
  endelse

if KEYWORD_SET(test) OR KEYWORD_SET(debug) then STOP
;
end
