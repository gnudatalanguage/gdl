;
; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
; libtiff version by Remi A. Solås <remi (at) edinsights.no>
;
function QUERY_TIFF, filename, info, geotiff=geotiff, image_index=image_index
ON_ERROR, 2

if TIFF_EXISTS() eq 0 then begin
    MESSAGE, /informational, "GDL was compiled without libtiff."

    if MAGICK_EXISTS() eq 0 then begin
        MESSAGE, /continue, "GDL was compiled without ImageMagick."
        MESSAGE, "Either libtiff or ImageMagick is required to use QUERY_TIFF."
    endif

    MESSAGE, /informational, "ImageMagick enabled as fallback with limited TIFF support."
    return, MAGICK_PING(filename, 'TIFF', info=info, image_index=image_index)
endif

return, TIFF_QUERY(filename, info=info, geotiff=geotiff, image_index=image_index)

end

