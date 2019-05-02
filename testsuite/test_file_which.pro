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
if STRLOWCASE(!version.os) eq 'darwin' then begin
    MEssage, /continue, 'This code might be not working on OSX (darwin)'
    message, /continue, 'depending you use a HPFS+ with Case Sensitive or not ...'    
endif
;
nb_pbs=0
;
; the file "Saturn.jpg" must be in the testsuite dir.
;
resu=FILE_WHICH('Saturn.jpg')
if (STRLEN(resu) EQ 0) then ++nb_pbs
;
resu=FILE_WHICH('file_which.pro')
if (STRLEN(resu) EQ 0) then ++nb_pbs
;
; check if we might be in position to do the next checks.
;
filetest='gdl'
;
cantest = file_test('../src/'+filetest)
;
if(cantest) then begin
    ;
    resu2a=FILE_WHICH('../src/', filetest)
    if (STRLEN(resu2a) EQ 0) then ++nb_pbs
    ;
    ; does the auto-add of final "/" working ?
    resu2b=FILE_WHICH('../src', filetest)
    if (STRLEN(resu2b) EQ 0) then ++nb_pbs
    if (STRCMP(resu2b,resu2a) NE 1) then ++nb_pbs
    ;
    endif else $
    message,/continue,' run from a non-production location (no ../src/gdl found)'
;
; final message
;
BANNER_FOR_TESTSUITE, 'TEST_FILE_WHICH', nb_pbs, short=short
;
if (nb_pbs GT 0) AND ~KEYWORD_SET(test) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
