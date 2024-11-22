;+
; WRITE_PPM
;	Write a PPM or PGM image file.
;
; Usage:
;	write_ppm, file, image
;
; Arguments:
;	file	string	The file to write
;	image	byte	The image to write, must be n×m (PGM) or 3×n×m
;			(PPM)
;
; Notes:
;	The ASCII keyword is not (yet) implemented.
;
; History:
;	Original (using write_jpeg as a template); 28/9/21; SJT
;-

pro write_ppm, file, image,  help = help
  
;
; this line allows to compile also in IDL ...
  forward_function magick_exists, magick_ping, magick_read
;
  on_error, 2

  if keyword_set(help) then begin
     print, 'pro write_ppm, filename, image'
     return
  endif

; Do we have access to ImageMagick functionnalities ??

  if ~magick_exists() then begin
     message, /continue, "GDL was compiled without ImageMagick support."
     message, "You must have ImageMagick support to use this functionaly."
  endif

  sz = size(image)
  if sz[0] eq 2 then begin      ; PGM
     mid = magick_create(sz[1], sz[2])
     cmap = indgen(256)
     magick_write, mid, reverse(image, 2)
     magick_writecolortable, mid, cmap, cmap, cmap
     magick_writefile, mid, file, 'PGM'
  endif else if sz[0] eq 3 && sz[1] eq 3 then begin ; PPM
     mid = magick_create(sz[2], sz[3])
     magick_write, mid, reverse(image, 3), rgb = 1s
     magick_writefile, mid, file, 'PPM'
  endif else message, "IMAGE must be  n×m (PGM) or 3×n×m (PPM)"

  magick_close, mid
  
end
