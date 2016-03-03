;
; Alain Coulais, August 2011
;
; PB 1: this code might be not working on MSwin OS 
; (path sep. are hard coded with "/")
;
; PB 2: this code might be not working on OSX
; if the HPFS+ File System is not sensitive to Case (min/MAJ
; ("not sensitive" by default !!)
;
pro TEST_FILE_WHICH, test=test
;
if STRLOWCASE(!version.os_name) eq 'windows' then begin
    MEssage, /continue, 'This code might be not working on MSwin OS'
    message, /continue, 'Please help us and report improvment !'
endif
if STRLOWCASE(!version.os_name) eq 'darwin' then begin
    MEssage, /continue, 'This code might be not working on OSX (darwin)'
    message, /continue, 'depending you use a HPFS+ with Case Sensitive or not ...'    
endif
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
; final message
;
BANNER_FOR_TESTSUITE, 'TEST_FILE_WHICH', nb_pbs, short=short
;
if (nb_pbs GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
