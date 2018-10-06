;
; Testing TIFF support: QUERY_TIFF
; TODO: READ_TIFF and WRITE_TIFF (when implemented using libtiff/libgeotiff)
;
; Licensed under GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
;
; ------------------------------------------
; Modification history:
; 
; 2018-06-20 : Remi A. Solås <remi.solaas (at) edinsights (dot) no>
; - Updated sample image names
; - Added basic READ_TIFF tests for 8-bit grayscale images
;
; 2018-06-19 : Remi A. Solås <remi.solaas (at) edinsights (dot) no>
; - Initial version
; - Basic QUERY_TIFF tests for both tiff and geotiff
;
; 2018-09-19 : AC :
; - Allowing running it on IDL ...
; - Maj for most Pro/Funct
; - This code is not working with FL Fawlty Language 0.79.43.1
;
; ------------------------------------------
;
function INTERNAL_GDL_TIFF
FORWARD_FUNCTION TIFF_EXISTS
return, TIFF_EXISTS()
end
;
function INTERNAL_GDL_GEOTIFF
FORWARD_FUNCTION GEOTIFF_EXISTS
return, GEOTIFF_EXISTS()
end
;
; ------------------------------------------
;
pro TEST_QUERY_TIFF, ntoterr, test=test, verbose=verbose
;
nerr=0

;; Regular TIFF, tiled
file=FILE_SEARCH_for_testsuite('tiff/8bit_gray_tiled.tif')
;
if QUERY_TIFF(file, info) eq 1 then begin
   if info.channels ne 1 then $
      ERRORS_ADD, nerr, 'Unexpected number of CHANNELS in ' + file

   if ~ARRAY_EQUAL(info.dimensions, [1024, 1024]) then $
      ERRORS_ADD, nerr, 'Unexpected value of DIMENSIONS in ' + file

   if info.has_palette ne 0 then $
      ERRORS_ADD, nerr, 'Unexpected value of HAS_PALETTE in ' + file

   if info.num_images ne 1 then $
      ERRORS_ADD, nerr, 'Unexpected value of NUM_IMAGES in ' + file

   if info.image_index ne 0 then $
      ERRORS_ADD, nerr, 'Unexpected value of IMAGE_INDEX in ' + file

   if info.pixel_type ne 1 then $
      ERRORS_ADD, nerr, 'Unexpected value of PIXEL_TYPE in ' + file

   if info.type ne 'TIFF' then $
      ERRORS_ADD, nerr, 'Unexpected value of TYPE in ' + file

   if info.bits_per_sample ne 8 then $
      ERRORS_ADD, nerr, 'Unexpected value of BITS_PER_SAMPLE in ' + file

   if info.orientation ne 1 then $
      ERRORS_ADD, nerr, 'Unexpected value of ORIENTATION in ' + file

   if info.planar_config ne 1 then $
      ERRORS_ADD, nerr, 'Unexpected value of PLANAR_CONFIG in ' + file

   if ~ARRAY_EQUAL(info.resolution, [1, 1]) then $
      ERRORS_ADD, nerr, 'Unexpected value of RESOLUTION in ' + file

   if info.units ne 2 then $
      ERRORS_ADD, nerr, 'Unexpected value of UNITS in ' + file

   if ~ARRAY_EQUAL(info.tile_SIZE, [256, 112]) then $
      ERRORS_ADD, nerr, 'Unexpected value of TILE_SIZE in ' + file

endif else ERRORS_ADD, nerr, 'QUERY_TIFF failed to query ' + file
;
; ------
;
BANNER_FOR_TESTSUITE, 'TEST_QUERY_GEOTIFF', nerr, /status
ERRORS_CUMUL, ntoterr, nerr
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------------------
;
pro TEST_QUERY_GEOTIFF, ntoterr, test=test, verbose=verbose
;
nerr=0
;; GeoTIFF, untiled
file=FILE_SEARCH_for_testsuite('tiff/8bit_gray_geo.tif')
if QUERY_TIFF(file, info, geotiff=geo) eq 1 then begin
   if ~ARRAY_EQUAL(info.tile_SIZE, [info.dimensions[0], 1]) then $
      ERRORS_ADD, nerr, 'Unexpected value of TILE_SIZE in ' + file

   if ~ARRAY_EQUAL(geo.modelPixelScaleTag, [60, 60, 0]) then $
      ERRORS_ADD, nerr, 'Unexpected value of MODELPIXELSCALETAG in ' + file

   if ~ARRAY_EQUAL(geo.modelTiePointTag, [0, 0, 0, 440720, 100000, 0]) then $
      ERRORS_ADD, nerr, 'Unexpected value of MODELTIEPOINTTAG in ' + file

   if geo.gtModelTypeGeoKey ne 1 then $
      ERRORS_ADD, nerr, 'Unexpected value of GTMODELTYPEGEOKEY in ' + file

   if geo.gtRasterTypeGeoKey ne 1 then $
      ERRORS_ADD, nerr, 'Unexpected value of GTRASTERTYPEGEOKEY in ' + file

   if geo.projectedCsTypeGeoKey ne 21892 then $
      ERRORS_ADD, nerr, 'Unexpected value of PROJECTEDCRTYPEGEOKEY in ' + file

endif else ERRORS_ADD, nerr, 'QUERY_TIFF failed to query ' + file
;
; ------
;
BANNER_FOR_TESTSUITE, 'TEST_QUERY_GEOTIFF', nerr, /status
ERRORS_CUMUL, ntoterr, nerr
if KEYWORD_SET(test) then STOP
;
end
; ------------------------------------------

pro TEST_READ_TIFF, ntoterr, test=test, verbose=verbose
;
nerr=0
;
;; 8-bit grayscale with SUB_RECT
file=FILE_SEARCH_FOR_TESTSUITE('tiff/8bit_gray_geo.tif')

; AC 2018-sep-19 : not very happy ! 
;
;image=TIFF_READ(file, sub_rect=[5, 15, 30, 90])
image=READ_TIFF(file, sub_rect=[5, 15, 30, 90])

if ~ARRAY_EQUAL(SIZE(image, /dimensions), [30, 90]) then $
   ERRORS_ADD, nerr, 'Unexpected return value of SUB_RECT of ' + file

if ~ARRAY_EQUAL(image[0:5], [107, 107, 115, 140, 99, 115]) then $
   ERRORS_ADD, nerr, 'Unexpected pixel values in tested range of ' + file

if ((image[0, 0] ne 107) || (image[0, 89] ne 0) || $
    (image[29, 0] ne 107) || (image[29, 89] ne 41)) then $
       ERRORS_ADD, nerr, 'Unexpected pixel values in corners of ' + file
;
; ------
;
BANNER_FOR_TESTSUITE, 'TEST_READ_TIFF', nerr, /status
ERRORS_CUMUL, ntoterr, nerr
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------------------
;
pro TEST_TIFF, help=help, test=test, no_exit=no_exit, verbose=verbose

if KEYWORD_SET(help) then begin
   print, 'pro TEST_TIFF, help=help, test=test, no_exit=no_exit, verbose=verbose'
   return
endif
;
if (GDL_IDL_FL() EQ 'FL') then MESSAGE, /continue, 'This code don''t work on FL now'
;
DEFSYSV, '!gdl', exists=is_it_gdl
;
if (is_it_gdl) then begin 
   status_tiff=INTERNAL_GDL_TIFF()
   if ~status_tiff then begin
      MESSAGE, /continue, 'GDL was compiled without TIFF support'
      EXIT, status=77
   endif
   status_geotiff=INTERNAL_GDL_GEOTIFF()
   if ~status_geotiff then MESSAGE, /continue, 'GDL was compiled without GEO TIFF support'
endif else begin
   status_tiff=1
   status_geotiff=1
endelse
;
if status_tiff then begin
   TEST_QUERY_TIFF, ntoterr, test=test, verbose=verbose
   TEST_READ_TIFF, ntoterr, test=test, verbose=verbose   
endif
if status_geotiff then TEST_QUERY_GEOTIFF, ntoterr, test=test, verbose=verbose
;
; ----------------- final MESSAGE ----------
;
BANNER_FOR_TESTSUITE, 'TEST_TIFF', ntoterr
;
if ntoterr gt 0 && ~KEYWORD_SET(no_exit) then EXIT, status=1
if KEYWORD_SET(test) then stop
;
end

