;
; Sylwester Arabas and Alain Coulais, August 2011
;
; Can we read a JPEG file ?
;
pro test_bug_3376577
;
if (MAGICK_EXISTS() EQ 0) then begin
    MESSAGE,/continue, "GDL was compiled without ImageMaick support"
    EXIT, status=77
endif
;
fichier=FILE_WHICH('Saturn.jpg')
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
