;
; Testing TIFF support: QUERY_TIFF
; TODO: READ_TIFF and WRITE_TIFF (when implemented using libtiff/libgeotiff)
;
; Licensed under GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
;
; ---------------------
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
; ---------------------
;

pro test_query_tiff, ntoterr, test=test, verbose=verbose
  nerr=0

  ; Regular TIFF, tiled
  file=file_search_for_testsuite('tiff/8bit_gray_tiled.tif')
  if query_tiff(file, info) eq 1 then begin
    if info.channels ne 1 then $
      errors_add, nerr, 'Unexpected number of CHANNELS in ' + file

    if ~array_equal(info.dimensions, [1024, 1024]) then $
      errors_add, nerr, 'Unexpected value of DIMENSIONS in ' + file

    if info.has_palette ne 0 then $
      errors_add, nerr, 'Unexpected value of HAS_PALETTE in ' + file

    if info.num_images ne 1 then $
      errors_add, nerr, 'Unexpected value of NUM_IMAGES in ' + file

    if info.image_index ne 0 then $
      errors_add, nerr, 'Unexpected value of IMAGE_INDEX in ' + file

    if info.pixel_type ne 1 then $
      errors_add, nerr, 'Unexpected value of PIXEL_TYPE in ' + file

    if info.type ne 'TIFF' then $
      errors_add, nerr, 'Unexpected value of TYPE in ' + file

    if info.bits_per_sample ne 8 then $
      errors_add, nerr, 'Unexpected value of BITS_PER_SAMPLE in ' + file

    if info.orientation ne 1 then $
      errors_add, nerr, 'Unexpected value of ORIENTATION in ' + file

    if info.planar_config ne 1 then $
      errors_add, nerr, 'Unexpected value of PLANAR_CONFIG in ' + file

    if ~array_equal(info.resolution, [1, 1]) then $
      errors_add, nerr, 'Unexpected value of RESOLUTION in ' + file

    if info.units ne 2 then $
      errors_add, nerr, 'Unexpected value of UNITS in ' + file

    if ~array_equal(info.tile_size, [256, 112]) then $
      errors_add, nerr, 'Unexpected value of TILE_SIZE in ' + file

  endif else errors_add, nerr, 'QUERY_TIFF failed to query ' + file

  if geotiff_exists() then begin
    ; GeoTIFF, untiled
    file=file_search_for_testsuite('tiff/8bit_gray_geo.tif')
    if query_tiff(file, info, geotiff=geo) eq 1 then begin
      if ~array_equal(info.tile_size, [info.dimensions[0], 1]) then $
        errors_add, nerr, 'Unexpected value of TILE_SIZE in ' + file

      if ~array_equal(geo.modelPixelScaleTag, [60, 60, 0]) then $
        errors_add, nerr, 'Unexpected value of MODELPIXELSCALETAG in ' + file

      if ~array_equal(geo.modelTiePointTag, [0, 0, 0, 440720, 100000, 0]) then $
        errors_add, nerr, 'Unexpected value of MODELTIEPOINTTAG in ' + file

      if geo.gtModelTypeGeoKey ne 1 then $
        errors_add, nerr, 'Unexpected value of GTMODELTYPEGEOKEY in ' + file

      if geo.gtRasterTypeGeoKey ne 1 then $
        errors_add, nerr, 'Unexpected value of GTRASTERTYPEGEOKEY in ' + file

      if geo.projectedCsTypeGeoKey ne 21892 then $
        errors_add, nerr, 'Unexpected value of PROJECTEDCRTYPEGEOKEY in ' + file

    endif else errors_add, nerr, 'QUERY_TIFF failed to query ' + file
  endif

  banner_for_testsuite, 'test_query_tiff', nerr, /status
  errors_cumul, ntoterr, nerr
  if keyword_set(test) then STOP
end

pro test_read_tiff, ntoterr, test=test, verbose=verbose
  nerr=0

  ; 8-bit grayscale with SUB_RECT
  file=file_search_for_testsuite('tiff/8bit_gray_geo.tif')
  image=tiff_read(file, sub_rect=[5, 15, 30, 90])

  if ~array_equal(size(image, /dimensions), [30, 90]) then $
    errors_add, nerr, 'Unexpected return value of SUB_RECT of ' + file

  if ~array_equal(image[0:5], [107, 107, 115, 140, 99, 115]) then $
    errors_add, nerr, 'Unexpected pixel values in tested range of ' + file

  if ((image[0, 0] ne 107) || (image[0, 89] ne 0) || (image[29, 0] ne 107) || (image[29, 89] ne 41)) then $
    errors_add, nerr, 'Unexpected pixel values in corners of ' + file

  banner_for_testsuite, 'test_read_tiff', nerr, /status
  errors_cumul, ntoterr, nerr
  if keyword_set(test) then STOP
end

pro test_tiff, help=help, test=test, no_exit=no_exit, verbose=verbose
  if keyword_set(help) then begin
    print, 'pro test_tiff, help=help, test=test, no_exit=no_exit, verbose=verbose'
    return
  endif

  if tiff_exists() then begin
    test_query_tiff, ntoterr, test=test, verbose=verbose
    test_read_tiff, ntoterr, test=test, verbose=verbose
  endif

  banner_for_testsuite, 'test_tiff', ntoterr
  if ntoterr gt 0 && ~keyword_set(no_exit) then exit, status=1
  if keyword_set(test) then stop
end

