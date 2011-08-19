;
; Alain Coulais, August 2011
;
;
pro TEST_FILE_WHICH, test=test
;
nb_pbs=0
;
; the file "Saturn.jpg" must be in the testsuite dir.
;
resu=FILE_WHICH('Saturn.jpg')
if (STRLEN(resu) EQ 0) then nb_pbs=nb_pbs+1
;
resu=FILE_WHICH('file_which.pro')
if (STRLEN(resu) EQ 0) then nb_pbs=nb_pbs+1
;
resu2a=FILE_WHICH('../src/', 'gdl')
if (STRLEN(resu2a) EQ 0) then nb_pbs=nb_pbs+1
;
; does the auto-add of final "/" working ?
resu2b=FILE_WHICH('../src', 'gdl')
if (STRLEN(resu2b) EQ 0) then nb_pbs=nb_pbs+1
if (STRCMP(resu2b,resu2a) NE 1) then nb_pbs=nb_pbs+1
;
if (nb_pbs GT 0) then begin
    if ~KEYWORD_SET(test) then EXIT, status=1
endif else begin
    MESSAGE, /Continue, "No problem found"
endelse
;
if KEYWORD_SET(test) then STOP
;
end
