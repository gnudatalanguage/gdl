;+
;
; NAME: WRITE_TIFF
;
; PURPOSE: write a tiff image from memory to a bitmap
;
; CATEGORY: Images (IO)
;
; CALLING SEQUENCE: 
;    WRITE_TIFF, filename, image, bits_per_sample, red, green, blue, $
;                compression=[0|1|2|3], description=description,document_name=docname,geotiff=structure,$
;                double=double, verbose=verbose, help=help, test=test, $
;                debug=debug
;
; KEYWORD PARAMETERS: 
;     ORDER      : 1 = top-bottom, 0 = bottom-top
;     VERBOSE    : Not Used
;     TRANSPARENT: 
;           
;
; OPTIONAL INPUTS:
;        bits_per_sample: bits per sample (not done)
;        compression: compression method (not done)
;        For pseudocolor only
;        red  : the Red colormap vector (for PseudoColor images)
;        green: the Green colormap vector (for PseudoColor images)
;        blue : the Blue colormap vector (for PseudoColor images)
;
; RESTRICTIONS:
;         Requires ImageMagick or GraphicsMagick.
;
; PROCEDURE:
;         Uses ImageMagick to write the data as requested
;
; EXAMPLE: 
;         Currently no example is available
;
; MODIFICATION HISTORY:
;  Written by James Tappin, based on the extant routine attempt to
;  discern what is meant to happen.
;-
; LICENCE:
; Copyright (C) 2021: SJT
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   

pro WRITE_TIFF, filename, image, bits_per_sample, $
   red = red, green =  green, blue = blue, $
   compression = compression, description=description, document_name=docname, $
   geotiff=geotiff,$
   double=double, $
   verbose = verbose, $
   help = help, test = test, $
   debug = debug, order = order

; this line allows to compile also in IDL ...
  forward_function magick_exists, magick_ping, magick_read

  
  if keyword_set(help) then begin
     print, ' WRITE_TIFF, filename, image, bits_per_sample, red=red, green=green, blue=blue, $'
     print, '            compression=compression, compression=[0|1|2|3], $'
     print, '            description=string,document_name=string,$'
     print, '            geotiff=structure, verbose=verbose, help=help, test=test, $'
     print, '            debug=debug'
     return
  endif
;
; Do we have access to ImageMagick functionalities ??
;
  if (MAGICK_EXISTS() EQ 0) then begin
     MESSAGE, /continue, $
              "GDL was compiled without ImageMagick/GraphicsMagick support."
     MESSAGE, "You must have one of these to use this functionality."
  endif

  sz = size(image)

  case sz[0] of
     2: begin
        mid = magick_create(sz[1], sz[2])
        psflag = 1b
     end
     3: begin
        md = min(sz[1:3], imd)
        psflag = md eq 2        ; Pseudo colour with transparency
        if md ne 3 then begin
           message, /continue, $
                    "Images with transparency aren't properly " + $
                    "supported yet"
           message, /continue, $
                    "the result will be corrupted."
        endif
        case imd of
           0: begin
              seq = [0, 1, 2]
              mid = magick_create(sz[2], sz[3])
           end
           1: begin
              seq = [1, 0, 2]
              mid = magick_create(sz[1], sz[3])
           end
           2: begin
              seq = [2, 0, 1]
              mid = magick_create(sz[1], sz[2])
           end              
        endcase
     end
     else: message, "Image array must be of dimension (n,m) or (k,n,m)."
  endcase
  
  if psflag then begin
     if ~keyword_set(red) then red = bindgen(256)
     if ~keyword_set(green) then green = bindgen(256)
     if ~keyword_set(blue) then blue = bindgen(256)

     if sz[0] eq 2 then begin
        timage = transpose([[[red  [image]]], $
                            [[green[image]]], $
                            [[blue [image]]]], $
                           [2, 0, 1])
     endif else begin
        case imd of
           0: begin
              fci = reform(image[0, *, *])
              tri = reform(image[1, *, *])
           end
           1: begin
              fci = reform(image[*, 0, *])
              tri = reform(image[*, 1, *])
           end
           2: begin
              fci = reform(image[*, *, 0])
              tri = reform(image[*, *, 1])
           end
        endcase
        timage = transpose([[[red  [fci]]], $
                            [[green[fci]]], $
                            [[blue [fci]]], $
                            [[tri]]], $
                           [2, 0, 1])
     endelse
  endif else begin
     if imd eq 0 then timage = image $
     else timage = transpose(image, seq)
  endelse

  if keyword_set(order) then timage = reverse(timage, 3)
  
  magick_write, mid, timage, rgb = 1s
  magick_writefile, mid, filename, 'TIFF'
  magick_close, mid
  if keyword_set(geotiff) then updategeotagsinimage,filename,geotiff
end
