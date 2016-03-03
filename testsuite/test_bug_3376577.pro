;
; Sylwester Arabas and Alain Coulais, August 2011
;
; Can we read a JPEG file ?
;
pro TEST_BUG_3376577
;
;  for IDL compatibility
FORWARD_FUNCTION MAGICK_EXISTS
;
DEFSYSV, '!gdl', exists=is_it_gdl
;
if (is_it_gdl EQ 1) then begin
    ;; Do we have access to ImageMagick functionnalities ??
    ;;
    if (MAGICK_EXISTS() EQ 0) then begin
        MESSAGE,/continue, "GDL was compiled without ImageMaick support"
        EXIT, status=77
    endif
endif
;
fichier=FILE_SEARCH_FOR_TESTSUITE('Saturn.jpg', /quiet)
;
if (STRLEN(fichier) EQ 0) then begin
    MESSAGE, "File <<Saturn.jpg>> not found", /continue
    MESSAGE, "but this file must be in the GDL_PATH", /continue
    EXIT, status=1
endif
;
READ_JPEG, fichier, image
;
end
