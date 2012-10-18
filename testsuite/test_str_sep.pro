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
; conversions of no-string input: when Separator is a simple char,
; we do not convert into STRING before comparisons ...
;
res_single=STR_SEP(2e3, '0')
res_nosingle=STR_SEP(2e3, '00')
res_nosingle_str=STR_SEP(STRING(2e3), '00')
;
; first case
;
if (N_elEments(res_single) NE 1) then begin
    txt='problem 3a: output should be converted'
    MESSAGE, txt, /continue
    nb_errors++
endif
if (N_ELEMENTS(res_single) NE 1) then begin
    txt='problem 3a-bis: output should be converted'
    MESSAGE, txt, /continue
    nb_errors++
endif else begin
    if (STRLEN(res_single) NE 13) then begin
        txt='problem 3a: output should be converted into string'
        MESSAGE, txt, /continue
        nb_errors++
    endif
    if (res_single NE STRING(2e3)) then begin
        txt='problem 3a-bis: output should be converted into string'
        MESSAGE, txt, /continue
        nb_errors++
    endif
endelse
;
; second case
if N_ELEMENTS(res_nosingle) NE 3 then  begin
    txt='problem 3b: bad Elements in output'
    MESSAGE, txt, /continue
    nb_errors++
endif
;
; third one
if (ARRAY_EQUAL(res_nosingle,res_nosingle_str) eq 0) then begin
    txt='problem 3c: result differs from reference'
    MESSAGE, txt, /continue
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
