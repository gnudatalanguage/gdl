;+
;
; NAME: WRITE_BMP
;
; PURPOSE: write a image from memory to a bitmap
;
; CATEGORY: Images (IO)
;
; CALLING SEQUENCE: 
;         write_bmp,filename,image,red,green,blue,[RGB=RGB,FOUR_BIT=FOUR_BIT,$ 
;                                                 IHDR=IHDR,$
;                                                 HEADER_DEFINE=HEADER_DEFINE])
;
; KEYWORD PARAMETERS: 
;     RGB        : Set to reverse the color ordering from BGR  
;     FOUR_BIT   : Four bit data file
;     IHDR       : Structure fo head information
;     HEADER_INFO: returns a blank IHDR structure if HEADER_INFO is
;     defined before the call
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
;         See "image_test.pro" in testsuite/
;
; MODIFICATION HISTORY:
;
; Written by: Christopher Lee 2004-05-17
;
; Modifications
; Alain Coulais, 30-AUG-2011:
; - adding check on ImageMagick/GraphicMagick MAGICK_EXISTS()
; Alain Coulais, 10-AUG-2014:
; - FORWARD_FUNCTION
; 
;-
; LICENCE:
; Copyright (C) 2004, 2011, 2014
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-
;
pro WRITE_BMP, filename, image, red, green, blue, $
               rgb=rgb, four_bit=four_bit, $
               ihdr=ihdr,header_define=header_define, $
               test=test, help=help, debug=debug
;
; this line allows to compile also in IDL ...
FORWARD_FUNCTION MAGICK_EXISTS, MAGICK_PING, MAGICK_READ
;
if ~KEYWORD_SET(test) then ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
    print, 'pro WRITE_BMP, filename, image, red, green, blue, $'
    print, '               rgb=rgb, four_bit=four_bit, $'
    print, '               ihdr=ihdr,header_define=header_define, $'
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
if(KEYWORD_SET(header_define)) then begin
    MESSAGE, "header define not yet supported"
endif else begin
    n=SIZE(image, /n_dimensions)
    s=SIZE(image,/dimensions)
    if (n EQ 2) then begin
        ;;pseudocolor
        if (N_PARAMS() lt 5) then TVLCT, red, green, blue, /get
        ;;
        ;;colorvectors provided
        mid=MAGICK_CREATE(s[0],s[1])
        _image=TRANSPOSE([[[blue[image]]],[[green[image]]],[[red[image]]]],[2,0,1])
        ;;
        MAGICK_WRITE, mid,_image,rgb=rgb
        if (N_ELEMENTS(red) eq N_ELEMENTS(green) and $
            N_ELEMENTS(red) eq N_ELEMENTS(blue)) then begin
            MAGICK_WRITEColorTable,mid,red,green,blue
            MAGICK_QUANTIZE,mid,long(N_ELEMENTS(red))
            MAGICK_WRITEFILE, mid, filename, "BMP"
            MAGICK_CLOSE, mid
        endif        
    endif else if(n eq 3) then begin
        mid=MAGICK_CREATE(s[1],s[2])
        MAGICK_WRITE, mid, image, rgb=rgb
        MAGICK_WRITEFILE, mid, filename, "BMP"
        MAGICK_CLOSE, mid
    endif
endelse
;
if KEYWORD_SET(test) OR KEYWORD_SET(debug) then STOP
;   
end



