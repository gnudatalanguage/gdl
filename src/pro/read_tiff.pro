;
; by: Remi A. Solås <remi (at) edinsights.no>
; ref: https://www.harrisgeospatial.com/docs/READ_TIFF.html
;
function READ_TIFF, filename, r, g, b, channels=channels, $
                    dot_range=dot_range, geotiff=geotiff, $
                    icc_profile=icc_profile, image_index=image_index, $
                    interleave=interleave, orientation=orintation, $
                    photoshop=photoshop, planarconfig=planarconfig, $
                    sub_rect=sub_rect, verbose=verbose

ON_ERROR, 2

if TIFF_EXISTS() eq 0 then begin
    MESSAGE, /continue, "GDL was compiled without libtiff."
    MESSAGE, "libtiff, optionally with libgeotif is required for this function."
endif

if GEOTIFF_EXISTS() eq 0 and keyword_set(GEOTIFF) then begin
    MESSAGE, /continue, "GDL was compiled without libgeotiff."
    MESSAGE, "libgeotiff is required in order to use the GEOTIFF keyword."
endif

return, TIFF_READ(filename, red=r, green=g, blue=b, channels=channels, $
                  dot_range=dot_range, geotiff=geotiff, $
                  icc_profile=icc_profile, image_index=image_index, $
                  interleave=interleave, orientation=orintation, $
                  photoshop=photoshop, planarconfig=planarconfig, $
                  sub_rect=sub_rect, verbose=verbose)
end

