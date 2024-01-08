;
; AC 2024-JAN-08, under GNU GPL v2+
;
; Proof of concept of a very priliminary test for SHMMAP
; Need 2 sesssions
; tested with GDL & IDL
;
pro TEST_SHMMAP, test=test, no_exit=no_exit, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_SHMMAP, test=test, no_exit=no_exit, help=help'
   return
endif
;
; SESSION1
;
SHMMAP,'anynamehere', /long, 5
tmp=SHMVAR('anynamehere')
print, 'session 1 before cleaning : ', tmp
;
; affect a content ...
local_var=INDGEN(5)
tmp(0)=local_var
print, 'session 1 after cleaning : ', tmp
;
; SESSION 2
;
path_to_exe=GET_PATH_TO_EXE()
SPAWN, path_to_exe+' -quiet test_shmmap_session2.pro'
;
print, 'session 1 after : ', tmp
;
nb_errors=0
expected=REVERSE(local_var)
if ~ARRAY_EQUAL(expected, tmp) then ERRORS_ADD, nb_errors, 'bad result'
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_SHMMAP', nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

