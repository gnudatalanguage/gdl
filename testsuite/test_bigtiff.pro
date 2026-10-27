;
; Testing WRITE_TIFF with BigTIFF output (/BIGTIFF), against the classic TIFF output.
;
; Licensed under GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
;
; ------------------------------------------
; Modification history:
;
; 2026-10-28 : ngo-binh0108
; - Initial version: round trips for each pixel type, RGB, /APPEND and /GEOTIFF,
;   classic versus BigTIFF. Files over 4 GB are not written here (too large for CI).
;
; ------------------------------------------
;
; TIFF version number in the header: 42 classic, 43 BigTIFF
function BIGTIFF_VERSION, file
openr, u, file, /get_lun
head=bytarr(4)
readu, u, head
free_lun, u
return, fix(head[2]) + 256*fix(head[3])
end
;
; Write the same image as classic TIFF and as BigTIFF; both must read back identically.
pro BIGTIFF_ROUNDTRIP, nerr, image, name, dir, _extra=extra
classic=dir+path_sep()+name+'_classic.tif'
big=dir+path_sep()+name+'_bigtiff.tif'
WRITE_TIFF, classic, image, _extra=extra
WRITE_TIFF, big, image, /bigtiff, _extra=extra
if BIGTIFF_VERSION(classic) ne 42 then ERRORS_ADD, nerr, name+': classic file does not have version 42'
if BIGTIFF_VERSION(big) ne 43 then ERRORS_ADD, nerr, name+': /BIGTIFF file does not have version 43'
a=READ_TIFF(classic)
b=READ_TIFF(big)
if ~ARRAY_EQUAL(size(a), size(b)) then ERRORS_ADD, nerr, name+': classic and BigTIFF differ in type or dimensions' $
else if ~ARRAY_EQUAL(a, b) then ERRORS_ADD, nerr, name+': classic and BigTIFF pixel values differ'
if ~ARRAY_EQUAL(size(b,/dim), size(image,/dim)) then ERRORS_ADD, nerr, name+': BigTIFF dimensions differ from the image written'
end
;
pro TEST_BIGTIFF_TYPES, ntoterr, dir, test=test, verbose=verbose
nerr=0
img=bindgen(37,23)
BIGTIFF_ROUNDTRIP, nerr, img, 'byte', dir
BIGTIFF_ROUNDTRIP, nerr, uint(img)*251us, 'uint', dir, /short
BIGTIFF_ROUNDTRIP, nerr, fix(img)-100s, 'int', dir, /short, /signed
BIGTIFF_ROUNDTRIP, nerr, ulong(img)*100003UL, 'ulong', dir, /long
BIGTIFF_ROUNDTRIP, nerr, long64(img)*10000000003LL, 'l64', dir, /l64, /signed
BIGTIFF_ROUNDTRIP, nerr, float(img)/7., 'float', dir, /float
BIGTIFF_ROUNDTRIP, nerr, double(img)/7d, 'double', dir, /double
; 3 samples: BitsPerSample and SampleFormat take 6 bytes, which sit inside the entry in BigTIFF
BIGTIFF_ROUNDTRIP, nerr, bytscl(randomu(1, 3, 40, 30)), 'rgb', dir
; 4 samples: exactly 8 bytes
BIGTIFF_ROUNDTRIP, nerr, bytscl(randomu(2, 4, 40, 30)), 'rgba', dir
; a document name of up to 8 bytes is also stored inside the entry
BIGTIFF_ROUNDTRIP, nerr, img, 'shortname', dir, document_name='ab'
BANNER_FOR_TESTSUITE, 'TEST_BIGTIFF_TYPES', nerr, /short, verb=verbose
ERRORS_CUMUL, ntoterr, nerr
if KEYWORD_SET(test) then STOP
end
;
pro TEST_BIGTIFF_APPEND, ntoterr, dir, test=test, verbose=verbose
nerr=0
first=bindgen(31,17)
second=reverse(first,2)
for big=0,1 do begin
   file=dir+path_sep()+(big ? 'append_bigtiff.tif' : 'append_classic.tif')
   WRITE_TIFF, file, first, bigtiff=big
   WRITE_TIFF, file, second, /append
   if BIGTIFF_VERSION(file) ne (big ? 43 : 42) then ERRORS_ADD, nerr, file+': /APPEND changed the file format'
   if QUERY_TIFF(file, info) ne 1 then ERRORS_ADD, nerr, file+': not readable after /APPEND' $
   else if info.num_images ne 2 then ERRORS_ADD, nerr, file+': expected 2 images after /APPEND'
   a0=READ_TIFF(file, image_index=0)
   a1=READ_TIFF(file, image_index=1)
   if ARRAY_EQUAL(a0, a1) then ERRORS_ADD, nerr, file+': the two images read back the same'
   if ~ARRAY_EQUAL(a1, reverse(a0,2)) then ERRORS_ADD, nerr, file+': second image does not match what was appended'
endfor
BANNER_FOR_TESTSUITE, 'TEST_BIGTIFF_APPEND', nerr, /short, verb=verbose
ERRORS_CUMUL, ntoterr, nerr
if KEYWORD_SET(test) then STOP
end
;
pro TEST_BIGTIFF_GEOTIFF, ntoterr, dir, test=test, verbose=verbose
nerr=0
; larger than 32 kB, so the GeoKeys sit beyond a 16-bit file offset
img=bytscl(dist(300,200))
; one double key and a short citation: in BigTIFF both values are 8 bytes or less and sit inside the entry
geoshort = { ModelPixelScaleTag: [10d, 10d, 0d], $
          ModelTiepointTag: [0d, 0d, 0d, 500000d, 4000000d, 0d], $
          GTModelTypeGeoKey: 1, GTRasterTypeGeoKey: 1, ProjectedCSTypeGeoKey: 32652, $
          ProjNatOriginLatGeoKey: 37.5d, $
          GTCitationGeoKey: 'UTM52' }
; several double keys and a long citation: written after the IFD in both formats.
; Each double key must point at its own value (the 2nd and 3rd used to read back as the 1st).
geolong  = { ModelPixelScaleTag: [10d, 10d, 0d], $
          ModelTiepointTag: [0d, 0d, 0d, 500000d, 4000000d, 0d], $
          GTModelTypeGeoKey: 1, GTRasterTypeGeoKey: 1, ProjectedCSTypeGeoKey: 32652, $
          ProjNatOriginLatGeoKey: 37.5d, ProjFalseEastingGeoKey: 123456d, ProjFalseNorthingGeoKey: 7890d, $
          GTCitationGeoKey: 'WGS 84 / UTM zone 52N' }
foreach g, list(geoshort, geolong), k do begin
   for big=0,1 do begin
      file=dir+path_sep()+'geo'+strtrim(k,2)+(big ? '_bigtiff.tif' : '_classic.tif')
      WRITE_TIFF, file, img, geotiff=g, bigtiff=big
      if BIGTIFF_VERSION(file) ne (big ? 43 : 42) then ERRORS_ADD, nerr, file+': wrong TIFF version'
      back=READ_TIFF(file, geotiff=tags)
      if ~ARRAY_EQUAL(back, img) && ~ARRAY_EQUAL(back, reverse(img,2)) then ERRORS_ADD, nerr, file+': pixel values differ'
      if size(tags,/type) ne 8 then begin
         ERRORS_ADD, nerr, file+': no GeoTIFF tags read back'
         continue
      endif
      if ~ARRAY_EQUAL(tags.MODELPIXELSCALETAG, g.ModelPixelScaleTag) then ERRORS_ADD, nerr, file+': ModelPixelScaleTag differs'
      if ~ARRAY_EQUAL(tags.MODELTIEPOINTTAG[*], g.ModelTiepointTag) then ERRORS_ADD, nerr, file+': ModelTiepointTag differs'
      if tags.PROJECTEDCSTYPEGEOKEY ne 32652 then ERRORS_ADD, nerr, file+': ProjectedCSTypeGeoKey differs'
      if abs(tags.PROJNATORIGINLATGEOKEY - 37.5d) gt 1d-12 then ERRORS_ADD, nerr, file+': ProjNatOriginLatGeoKey differs'
      if strtrim(tags.GTCITATIONGEOKEY,2) ne g.GTCitationGeoKey then ERRORS_ADD, nerr, file+': GTCitationGeoKey differs'
      if k eq 1 then begin
         if abs(tags.PROJFALSEEASTINGGEOKEY - 123456d) gt 1d-9 then ERRORS_ADD, nerr, file+': ProjFalseEastingGeoKey differs'
         if abs(tags.PROJFALSENORTHINGGEOKEY - 7890d) gt 1d-9 then ERRORS_ADD, nerr, file+': ProjFalseNorthingGeoKey differs'
      endif
   endfor
endforeach
BANNER_FOR_TESTSUITE, 'TEST_BIGTIFF_GEOTIFF', nerr, /short, verb=verbose
ERRORS_CUMUL, ntoterr, nerr
if KEYWORD_SET(test) then STOP
end
;
; ------------------------------------------
;
pro TEST_BIGTIFF, help=help, test=test, no_exit=no_exit, verbose=verbose, keep=keep
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_BIGTIFF, help=help, test=test, no_exit=no_exit, verbose=verbose, keep=keep'
   print, '   keep: a folder in which to keep the files written (default: a temporary folder, deleted)'
   return
endif
;
DEFSYSV, '!gdl', exists=is_it_gdl
if is_it_gdl then begin
   if ~TIFF_EXISTS() then begin
      MESSAGE, /continue, 'GDL was compiled without TIFF support'
      EXIT, status=77
   endif
   has_geotiff=GEOTIFF_EXISTS()
endif else has_geotiff=1
;
dir = KEYWORD_SET(keep) ? keep : FILEPATH('gdl_test_bigtiff', /tmp)
FILE_MKDIR, dir
ntoterr=0
TEST_BIGTIFF_TYPES, ntoterr, dir, test=test, verbose=verbose
TEST_BIGTIFF_APPEND, ntoterr, dir, test=test, verbose=verbose
if has_geotiff then TEST_BIGTIFF_GEOTIFF, ntoterr, dir, test=test, verbose=verbose $
else MESSAGE, /continue, 'GDL was compiled without GeoTIFF support: /GEOTIFF tests skipped'
if ~KEYWORD_SET(keep) then FILE_DELETE, dir, /recursive, /quiet
;
; ----------------- final MESSAGE ----------
;
BANNER_FOR_TESTSUITE, 'TEST_BIGTIFF', ntoterr
;
if ntoterr gt 0 && ~KEYWORD_SET(no_exit) then EXIT, status=1
if KEYWORD_SET(test) then stop
;
end
