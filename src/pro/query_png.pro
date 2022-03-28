;
; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
;
function QUERY_PNG, filename, info, image_index=image_index
;

compile_opt idl2, hidden

ON_ERROR, 2
;
; Do we have access to ImageMagick functionnalities ??
;
if (MAGICK_EXISTS() EQ 0) then begin
    MESSAGE, /continue, "GDL was compiled without ImageMagick support."
    MESSAGE, "You must have ImageMagick support to use this functionaly."
endif
;
return, MAGICK_PING(filename, 'PNG', info=info)
;
end
