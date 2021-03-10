;
; Alain C., derived from snippet by D Schnitzeler on 2017-04-21
;
; under GNU GPL v2 or later
;
; https://sourceforge.net/p/gnudatalanguage/bugs/720/
;
; NB: this code was in fact very simple to correct using "WordExp()"
; BUT it was realize at the same time that "WordExp()" was a mess !
;
pro TEST_BUG_N000720, help=help, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_BUG_N000720, help=help, test=test, no_exit=no_exit'
   return
end
;
errors=0
;
; recovering the Epoch tag (to avoid to erase othe file ...)
;
SPAWN, 'date +%s', epoch, exit_status=status
if (status GT 0) then epoch='12345fake'
;
; To trigger this bug, it is mandatory to use "~"
;
file='~/test_'+GDL_IDL_FL()+'_openu_'+epoch+'.txt'
print, 'file to be created : ', file
;
; removing the file if already existed
if FILE_TEST(file) then FILE_DELETE, file
;
OPENW, unit, file, /get_lun
PRINTF, unit, 'hello'
CLOSE, unit
FREE_LUN, unit
;
OPENU, unit, file, /get_lun, /append
PRINTF, unit, 'goodbye'
CLOSE, unit
FREE_LUN, unit
;
if FILE_TEST(file) then print, 'the file was written'
;
; Reading back the file
;
OPENR, unit, file, /get_lun
texte=''
line=''
while ~EOF(unit) do begin
   READF, unit, line
   texte=[texte, line]
endwhile
CLOSE, unit
FREE_LUN, unit
;
if N_ELEMENTS(texte) GT 1 then read_back=texte[1:*] else read_back=!null
expected=['hello','goodbye']
;
if ~ARRAY_EQUAL(read_back, expected) then begin
   nb1=N_ELEMENTS(read_back)
   nb2=N_ELEMENTS(expected)
   if (nb1 EQ 0) then ERRORS_ADD, errors,'when read back file is void'
   if (nb1 LT nb2) then ERRORS_ADD, errors,'missing lines in read back file'
   if (nb1 GT nb2) then ERRORS_ADD, errors, 'too much lines in read back file'
endif
cumul_errors=errors
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_BUG_N000720', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
if FILE_TEST(file) then FILE_DELETE, file
;
end
