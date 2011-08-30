;+
;
; NAME: WRITE_PICT
;
; PURPOSE: write a image from memory to a PICT
;
; CATEGORY: Images (IO)
;
; CALLING SEQUENCE: 
;         write_pict,filename,image,red,green,blue
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
; PROCEDURE:
;         Use ImageMagick to write the data as requested
;
; EXAMPLE:
;         See "image_test.pro" in testsuite/
;
; MODIFICATION HISTORY:
;  Written by: Christopher Lee 2004-05-28
;
;  Modification by Alain Coulais, 30-AUG-2011:
;    adding check on ImageMagick, MAJ
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
pro WRITE_PICT, filename, image, red, green, blue, $
                test=test, help=help, debug=debug
;
if ~KEYWORD_SET(test) then ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
    print, 'pro WRITE_BMP, filename, image, red, green, blue, $'
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
n=SIZE(image, /n_dimensions)
s=SIZE(image,/dimensions)

if (n eq 2) then begin
    ;;pseudocolor
    if (N_PARAMS() lt 5) then TVLCT, red, green, blue, /get
    ;;
    ;; colorvectors provided
    ;;
    mid=MAGICK_CREATE(s[0],s[1])
    _image=TRANSPOSE([[[blue[image]]],[[green[image]]],[[red[image]]]],[2,0,1])
    ;;
    MAGICK_WRITE,mid,_image,rgb=rgb
    MAGICK_FLIP,mid
    if(N_ELEMENTS(red) eq N_ELEMENTS(green) and $
       N_ELEMENTS(red) eq N_ELEMENTS(blue)) then begin
        MAGICK_QUANTIZE,mid,long(N_ELEMENTS(red))
        ;;
        ;;                MAGICK_WRITEIndexes,mid,image
        ;;                MAGICK_WRITEColorTable,mid,red,green,blue
        ;;
        MAGICK_WRITEfile,mid,filename,"PICT"
        MAGICK_CLOSE,mid
    endif
endif else begin
    if(n eq 3) then begin
        mid=MAGICK_CREATE(s[1],s[2])
        MAGICK_WRITE,mid,image,rgb=rgb
        MAGICK_WRITEfile,mid,filename,"PICT"
        MAGICK_CLOSE,mid
    endif
endelse
;
if KEYWORD_SET(test) OR KEYWORD_SET(debug) then STOP
;
end



