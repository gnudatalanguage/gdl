;
; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl> + G. Duvert
;
function QUERY_IMAGE, filename, info, _ref_extra=ex
;
  compile_opt idl2, hidden

  ON_ERROR, 2
 
   CATCH, Error_status
   IF Error_status NE 0 THEN BEGIN
      CATCH, /CANCEL
      if (n_elements(lun) gt 0 ) then begin
         close,lun
         free_lun,lun
      endif
      RETURN,0
   ENDIF

;
; GDL Should have ImageMagick NOW. Let this procedure crash if it not
; the case.
; Do we have access to ImageMagick functionnalities ??
;
;; if (MAGICK_EXISTS() EQ 0) then begin
;;     MESSAGE, /continue, "GDL was compiled without ImageMagick support."
;;     MESSAGE, "You must have ImageMagick support to use this functionaly."
;;  endif

; GD: protect against non-IDL supported formats by fast checking the
; magic number: query_image is only interested in the few
; idl-compliant image formats, not all magick-redable formats.
; and magick's 'ping' is waaaay tooo slow.

openr,lun,filename,/get_lun
what=bytarr(12)
READU, lun, what ; will exit through catch if size is too small
close,lun
free_lun,lun
; if total (what[0:9] eq ['67'x, '69'x, '6d'x, '70'x, '20'x, '78'x, '63'x, '66'x, '20'x, '76'x]) eq 10 then return, MAGICK_PING(filename, info=info, _strict_extra=ex) ; 'XCF'
; if total (what[0:8] eq ['2f'x, '2a'x, '20'x, '58'x, '50'x, '4d'x, '20'x, '2a'x, '2f'x]) eq 9 then return, MAGICK_PING(filename, info=info, _strict_extra=ex) ; 'XPM'
if (string(what[0:6]) eq '#define') then return, MAGICK_PING(filename, info=info, _strict_extra=ex) ; 'XBM'
if total (what[0:7] eq ['00'x, '00'x, '00'x, '0C'x, '6A'x, '50'x, '20'x, '20'x]) eq 8 then return, MAGICK_PING(filename, info=info, _strict_extra=ex) ; 'JPEG2000'
if total (what[0:5] eq ['50'x, '49'x, '43'x, '54'x, '00'x, '08'x]) eq 6 then return, MAGICK_PING(filename, info=info, _strict_extra=ex) ; 'PICT'
if total (what[0:3] eq ['47'x, '49'x, '46'x, '38'x]) eq 4 then  return, MAGICK_PING(filename, info=info, _strict_extra=ex) ; 'GIF8'
if total (what[0:3] eq ['89'x, '50'x, '4e'x, '47'x]) eq 4 then  return, MAGICK_PING(filename, info=info, _strict_extra=ex) ; 'PNG'
if total (what[0:3] eq ['4d'x, '4d'x, '00'x, '2a'x]) eq 4 then  return, MAGICK_PING(filename, info=info, _strict_extra=ex) ; 'TIFF' (big endian)
if total (what[0:3] eq ['49'x, '49'x, '2a'x, '00'x]) eq 4 then  return, MAGICK_PING(filename, info=info, _strict_extra=ex) ; 'TIFF'
;if total (what[0:3] eq ['00'x, '00'x, '01'x, '00'x]) eq 4 then return, MAGICK_PING(filename, info=info, _strict_extra=ex) ; 'ICO' UNSOUPPORTED
if total (what[0:3] eq ['59'x, 'a6'x, '6a'x, '95'x]) eq 4 then  return, MAGICK_PING(filename, info=info, _strict_extra=ex) ; 'SRF'
if total (what[0:3] eq ['44'x, '49'x, '43'x, '4D'x]) eq 4 then  return, MAGICK_PING(filename, info=info, _strict_extra=ex) ; 'DICOM'
if total (what[0:2] eq ['50'x, '35'x, '0A'x]) eq 3 then  return, MAGICK_PING(filename, info=info, _strict_extra=ex) ; 'PGM'


if total (what[0:1] eq ['42'x, '4D'x]) eq 2 then  return, MAGICK_PING(filename, info=info, _strict_extra=ex) ; 'BMP'
if total (what[0:1] eq ['FF'x, 'D8'x]) eq 2 then  return, MAGICK_PING(filename, info=info, _strict_extra=ex) ; 'JPG'
if total (what[0:1] eq ['0D'x, '0A'x]) eq 2 then  return, MAGICK_PING(filename, info=info, _strict_extra=ex) ; 'JPG2000 (variant)'
if total (what[1:3] eq ['00'x, '00'x]) eq 2 then begin
  if total (what[4:6] eq ['07'x, '00'x, '00'x]) eq 3 then return, MAGICK_PING(filename, info=info, _strict_extra=ex) ; 'XWD'
  if total (what[5:7] eq ['00'x, '00'x, '07'x]) eq 3 then return, MAGICK_PING(filename, info=info, _strict_extra=ex) ; 'XWD'
endif

if total (what[0:1] eq ['FF'x, 'FB'x]) eq 2 then  return, MAGICK_PING(filename, info=info, _strict_extra=ex) ; 'MPEG1'
if total (what[0:1] eq ['FF'x, 'F3'x]) eq 2 then  return, MAGICK_PING(filename, info=info, _strict_extra=ex) ; 'MPEG1'
if total (what[0:1] eq ['FF'x, 'F2'x]) eq 2 then  return, MAGICK_PING(filename, info=info, _strict_extra=ex) ; 'MPEG1'

return,0
;

;
end
