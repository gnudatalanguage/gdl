;
; by: Remi A. Solås <remi (at) edinsights.no>
; ref: https://www.harrisgeospatial.com/docs/READ_TIFF.html
;
function READ_TIFF, filename, r, g, b, channels=channels, $
                    dot_range=dot_range, geotiff=geotiff, $
                    icc_profile=icc_profile, image_index=image_index, $
                    interleave=interleave, orientation=orientation, $
                    photoshop=photoshop, planarconfig=planarconfig, $
                    sub_rect=sub_rect, verbose=verbose

ON_ERROR, 2

if TIFF_EXISTS() eq 0 then begin
    MESSAGE, /continue, "GDL was compiled without libtiff."
    MESSAGE, "libtiff is required for this function."
endif

if GEOTIFF_EXISTS() eq 0 and keyword_set(GEOTIFF) then begin
    MESSAGE, /continue, "GDL was compiled without libgeotiff."
    MESSAGE, "libgeotiff is required in order to use the GEOTIFF keyword."
endif

image = TIFF_READ(filename, red=r, green=g, blue=b, channels=channels, $
                  dot_range=dot_range, geotiff=geotiff, icc_profile=icc_profile, $
                  image_index=image_index, orientation=orientation, photoshop=photoshop, $
                  planarconfig=planarconfig, sub_rect=sub_rect, verbose=verbose)

if keyword_set(interleave) then begin
    if size(image, /n_dimensions) eq 3 then begin
        if interleave eq 0 then image = transpose(image, [0, 1, 2])
        if interleave eq 1 then image = transpose(image, [1, 0, 2])
        if interleave eq 2 then image = transpose(image, [1, 2, 0])
    endif else MESSAGE, /informational, "INTERLEAVE keyword ignored for images where CHANNELS is not 3"
endif

return, image

end

