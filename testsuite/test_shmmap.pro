;
; AC 2024-JAN-08, under GNU GPL v2+
;
; Proof of concept of a very priliminary test for SHMMAP
; Need 2 sesssions
;
; No test on the types !
;
; * tested with GDL & IDL & FL on Linux
; * tested with GDL & IDL on OSX
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
;
path_to_file=FILE_WHICH('test_shmmap_session2.pro')
if (path_to_file EQ '') then begin
   MESSAGE, 'input file <<test_shmmap_session2.pro>> not found'
   if ~KEYWORD_SET(no_exit) then EXIT, status=1 else STOP
endif
;
quiet=' -quiet '
if (GDL_IDL_FL() EQ 'FL') then quiet=' '
;
SPAWN, path_to_exe+quiet+path_to_file[0]
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

