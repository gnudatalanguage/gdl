;
; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
;
function QUERY_IMAGE, filename, info, _ref_extra=ex
;
ON_ERROR, 2
;
; Do we have access to ImageMagick functionnalities ??
;
if (MAGICK_EXISTS() EQ 0) then begin
    MESSAGE, /continue, "GDL was compiled without ImageMagick support."
    MESSAGE, "You must have ImageMagick support to use this functionaly."
endif
;
return, MAGICK_PING(filename, info=info, _strict_extra=ex)
;
end
