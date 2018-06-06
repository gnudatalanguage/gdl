;
; Alain C., 2018-JAN-25, under GNU GPL v3 or later
;
; 4 bugs were discovered this day in RESOLVE_ROUTINE !
; (one major : is the path contains Maj, they are converted into Min)
; -- the PATH should not be used !!!
;
; an opportunity to test also that no bugs or typo introduced
; in the PRO files ! (two discovered in testsuite files when tested !)
;
pro TEST_ALL_TEST_ROUTINES, cumul_errors, filter=filter, test=test
;
errors=0
;
if ~KEYWORD_SET(filter) then filter='test_*pro'
;
files=FILE_SEARCH(filter)
;
; we need to remove "TEST_RESOLVE_ROUTINE" in the list
;
index=STRPOS(files, 'test_resolve_routine')
ok=WHERE(index LT 0, nbp_ok)
if (nbp_ok GT 0) then files=files[ok]
;
if ((N_ELEMENTS(files) EQ 1) AND (STRLEN(files[0]) EQ 0)) then begin
   txt='No files <<test_*pro>> in current directory ('
   txt=txt+GETENV('PWD')+')'
   MESSAGE, /continue, txt
   ERRORS_CUMUL, cumul_errors, 1
   if KEYWORD_SET(test) then STOP
   return
endif
;
; now we need to remove the suffix : ".pro"
;
files=FILE_BASENAME(files, '.pro')
;
quote="'"
pbs=''
;
for ii=0, N_ELEMENTS(files)-1 do begin
   res=EXECUTE('RESOLVE_ROUTINE, '+quote+files[ii]+quote+",/EITHER")
   if (res NE 1) then begin
      errors++
      pbs=[pbs,files[ii]]
   endif
endfor
;
if (N_ELEMENTS(pbs) GT 1) then begin
   pbs=pbs[1:*]
   for jj=0, N_ELEMENTS(pbs)-1 do print, 'Problem in : ', pbs[jj]
   print, 'Due to problem, will skip next test.'
endif else begin
   print, 'No problem encoutered, starting again but all at once'
   resolve_routine,files,/either
endelse
;
BANNER_FOR_TESTSUITE, 'TEST_ALL_TEST_ROUTINES', errors, /short
;
ERRORS_CUMUL, cumul_errors, errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------------------------
;
pro TEST_RESOLVE_ROUTINE, help=help, test=test, no_exit=no_exit
;
TEST_ALL_TEST_ROUTINES, errors
;
; AC 2018 : no sense !!! should not work !!
;TEST_ALL_TEST_ROUTINES, errors, path=path=getenv('PWD')
;
; --------------------------
;
BANNER_FOR_TESTSUITE, 'TEST_RESOLVE_ROUTINE', errors, short=short
;
if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
