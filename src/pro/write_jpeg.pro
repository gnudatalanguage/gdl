;$Id: write_jpeg.pro,v 1.1.1.1 2004-12-09 15:10:19 m_schellens Exp $

pro write_jpeg, filename, image,$
                ORDER=ORDER,QUALITY=QUALITY, TRUE=TRUE,UNIT=UNIT,$
                PROGRESSIVE=PROGRESSIVE
;+
;
;
;
; NAME: WRITE_JPEG
;
;
; PURPOSE: write a image from memory to a jpeg
;
;
; CATEGORY: Images (IO)
;
;
; CALLING SEQUENCE: 
;         write_jpeg,filename,image,[ORDER=ORDER,QUALITY=QUALITY,
;                                                  TRUE=TRUE]
;
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
;         Requires ImageMagick
;
;
; PROCEDURE:
;         Use ImageMagick to write the data as requested
;
; EXAMPLE:
;         
;
;
; MODIFICATION HISTORY:
; 	Written by: Christopher Lee 2004-05-17
;
;
;
;-
; LICENCE:
; Copyright (C) 2004,
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;
;-


if(keyword_set(unit)) then message, "UNIT not supported"

if(keyword_set(unit)) then begin
    print, "UNIT not supported"
    return
endif


if(keyword_set(TRUE)) then begin
    if(TRUE eq 1) then t=[0,1,2]
    if(TRUE eq 2) then t=[1,0,2]
    if(TRUE eq 3) then t=[2,0,1]
    image=transpose(image, t)
endif

n=size(image, /n_dimensions)
s=size(image,/dimensions)
q=75

;generic
mid=magick_create(s[1],s[2])
magick_write,mid,image,rgb=rgb

if(keyword_set(progressive)) then $
    magick_interlace, mid, /PLANEINTERLACE

if(keyword_set(order)) then magick_flip,mid
if(keyword_set(quality)) then q=quality

magick_quality, mid,q*1U
magick_writefile,mid,filename,"JPEG"
magick_close,mid

;reverse
if(keyword_set(TRUE)) then begin
    if(TRUE eq 1) then t=[0,1,2]
    if(TRUE eq 2) then t=[1,0,2]
    if(TRUE eq 3) then t=[1,2,0]
    
    image=transpose(image, t)
endif



end



