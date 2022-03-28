;
; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
; Check PPM & PGM; James Tappin.

function QUERY_PPM, filename, info, maxval = maxval
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
; TODO: MAXVAL keyword
;
if arg_present(maxval) then message, /continue, $
                                     "MAXVAL not yet implemented."

if MAGICK_PING(filename, 'PPM', info = info) then return, 1
return, magick_ping(filename, 'PGM', info = info)
;
end
