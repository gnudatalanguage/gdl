;$Id: write_pict.pro,v 1.2 2006-03-29 07:48:21 m_schellens Exp $

pro write_pict, filename, image,red, green, blue
;+
;
;
;
; NAME: WRITE_PICT
;
;
; PURPOSE: write a image from memory to a PICT
;
;
; CATEGORY: Images (IO)
;
;
; CALLING SEQUENCE: 
;         write_pict,filename,image,red,green,blue
;
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
; 	Written by: Christopher Lee 2004-05-28
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

rgb=1
n=size(image, /n_dimensions)
s=size(image,/dimensions)

if(n eq 2) then begin
                                ;pseudocolor
    if(n_params() lt 5) then $
    tvlct, red, green, blue, /get
    
                                ;colorvectors provided
    mid=magick_create(s[0],s[1])
    _image=transpose([[[blue[image]]],[[green[image]]],[[red[image]]]],[2,0,1])
    
    magick_write,mid,_image,rgb=rgb
    magick_flip,mid
    if(n_elements(red) eq n_elements(green) and $
       n_elements(red) eq n_elements(blue)) then begin
        magick_quantize,mid,long(n_elements(red))
        
;                magick_writeIndexes,mid,image
;                magick_writeColorTable,mid,red,green,blue
        
        magick_writefile,mid,filename,"PICT"
        magick_close,mid
    endif

    
endif else if(n eq 3) then begin
    mid=magick_create(s[1],s[2])
    magick_write,mid,image,rgb=rgb
    magick_writefile,mid,filename,"PICT"
    magick_close,mid
endif




end



