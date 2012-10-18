;
; by Alain C. (moved here from src/pro/str_sep.pro)
;
; Modification on October, 18, 2012 to include new cases.
; While testing PSM soft (Planck Sky Model: 
; http://www.apc.univ-paris7.fr/~delabrou/PSM/psm.html )
; we discovered a bug: print, STR_SEP('2**10 uK/J', '**')
;
; do not hesitate to submit problematic cases !
;
pro TEST_STR_SEP, help=help, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_STR_SEP, help=help, test=test, no_exit=no_exit'
    return
endif
;
nb_errors=0
;
resu1=STR_SEP('../foo.txt','.')
;
if N_ELEMENTS(resu1) NE 4 then begin
    MESSAGE, 'problem 1 (null string removed ?)', /continue
    nb_errors++
endif
if resu1[2] NE '/foo' then begin
    MESSAGE, 'problem 1bis', /continue
    nb_errors++
endif
if resu1[3] NE 'txt' then begin
    MESSAGE, 'problem 1ter', /continue
    nb_errors++
endif
;
; inpired from PSM tests: will not survive to a call to STRSPLIT
; due to REGEX issues ...
;
units=STR_SEP('arcsec**2','**')
;
if units[0] NE 'arcsec' then begin
    MESSAGE, 'problem 2a', /continue
    nb_errors++
endif
if units[1] NE '2' then begin
    MESSAGE, 'problem 2b', /continue
    nb_errors++
endif
;
; do we triggered errors ?
;
MESSAGE, /continue, 'Final diagnostic :'
if (nb_errors GT 0) then begin
    MESSAGE, /continue, STRING(nb_errors)+' errors found within STR_SEP()'
    if ~KEYWORD_SET(no_exit) then EXIT, status=1
endif else begin
    MESSAGE, /continue, 'All tests within STR_SEP() successful'
endelse
;
if KEYWORD_SET(test) then STOP
;
end
