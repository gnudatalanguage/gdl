;
; Under GNU GPL V3 or later
;
; https://github.com/gnudatalanguage/gdl/issues/1709
; AC 2024-Jan-05
;
; Several bugs related to FILE_READLINK() found,
; https://github.com/gnudatalanguage/gdl/issues/1709
;
; ---------------------------------
; 
; Modifications history :
;
; - 2024-01-05 : AC. creation. Ideas welcome since I don't have
;   MSwin and *BSD around ...
;
; ---------------------------------
;
pro TEST_BSD_READLINK, cumul_errors, test=test
;
nb_errors=0
;
MESSAGE, /continue, "No idea for tests under *BSD. Please contribute"
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_BSD_READLINK', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ---------------------------------
;
pro TEST_MSWIN_READLINK, cumul_errors, test=test
;
nb_errors=0
;
MESSAGE, /continue, "No idea for tests under MSwin. Please contribute"
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_MSWIN_READLINK', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ---------------------------------
;
pro TEST_OSX_READLINK, cumul_errors, test=test
;
nb_errors=0
;
expected='private/etc'
res=FILE_READLINK('/etc')
mess= "error for /etc vs private/etc"
if (expected NE res) then ERRORS_ADD, nb_errors, mess
;
expected='/var/db/timezone/zoneinfo/'
res=FILE_READLINK('/etc/localtime')
res=STRMID(res,0,26)
mess="error for localtime vs timezone"
if (expected NE res) then ERRORS_ADD, nb_errors, mess
;
expected='/System/Volumes/Data/home'
res=FILE_READLINK('/home')
mess="error for /home vs Volume ..."
if (expected NE res) then ERRORS_ADD, nb_errors, mess
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_OSX_READLINK', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
;  ---------------------------------
;
pro TEST_LINUX_READLINK, cumul_errors, test=test
;
nb_errors=0
;
; see discussion in issue #1706
if FILE_TEST('/proc/sys/kernel/pid_max') then begin
   GET_LUN, nlun
   OPENR, nlun, '/proc/sys/kernel/pid_max'
   pid_max=0L
   ;; temporary using a format ... see #1712 :(
   READF, nlun, pid_max, format='(i)'
   CLOSE, nlun
   FREE_LUN, nlun
endif else begin
   print, "assuming old Linux system ..."
   pid_max=32767
endelse
pid_min=2                       ; (0 scheduler, 1 init)
;
pid=FILE_READLINK('/proc/self')
; when the conversion cannot be done, result is "0"
pid=LONG(pid)
mess= "error for /proc/self"
if (pid LT pid_min) or (pid GT pid_max) then ERRORS_ADD, nb_errors, mess
;
expected='/usr/share/zoneinfo/'
res=FILE_READLINK('/etc/localtime')
res=STRMID(res,0,20)
mess="error for localtime vs timezone"
if (expected NE res) then ERRORS_ADD, nb_errors, mess
;
expected='share/man'
res=FILE_READLINK('/usr/local/man')
mess="error for man pages ... not sure it is portable" 
if (expected NE res) then ERRORS_ADD, nb_errors, mess
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_LINUX_READLINK', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ---------------------------------
;
pro TEST_FILE_READLINK, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FILE_READLINK, help=help, test=test, $'
   print, '                        no_exit=no_exit, verbose=verbose'
   return
endif
;
cumul_errors=0
;
; specific tests for OSX
;
if (!version.os EQ STRLOWCASE('darwin')) then begin
   TEST_OSX_READLINK, cumul_errors, test=test
endif
;
; specific tests for Linux
;
if (!version.os EQ STRLOWCASE('linux')) then begin
   TEST_LINUX_READLINK, cumul_errors, test=test
endif
;
; specific tests for MSwin
;
if (!version.os EQ STRLOWCASE('window')) then begin
   TEST_MSWIN_READLINK, cumul_errors, test=test
endif
;
; specific tests for *BSD systems
;
if (STRPOS(STRlowCASE(!version.os), 'bsd') GE 0) then begin
   TEST_BSD_READLINK, cumul_errors, test=test
endif
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_FILE_READLINK', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
