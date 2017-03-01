;
; Alain Coulais and Sylwester Arabas, during the GHM 2011 in Paris.
; Copyright 2011. This code is under GNU GPL v2 or any later.
;
; * TEST_SYSTIME_LOCALE
;
; In some cases, when external libraries are compiled with
; non "C" locales, this can affect internal procedures in GDL ...
; One case was SYSTIME(), broken by external Image Magick with "bad" locale
;
; This test must be call before any use of LOCALE_GET().
; This test is not perfect because internal locale in GDL are changed
; by a call to LOCALE_GET().
;
; * TEST_SYSTIME_ZERO
;
; In fact the input in range inside ]-1,1[ is used as 0 ...
;
; -----------------------------------------------
;
pro TEST_SYSTIME_LOCALE, cumul_errors, help=help, test=test, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_SYSTIME_LOCALE, nb_pbs_locale, help=help, test=test, verbose=verbose'
   print, ''
   print, 'we test whether we use C locale, as internaly needed in GDL'
   return
endif
;
nb_pbs_locale=0
;
; same formating in Unix/Linux/OSX "date"
; than IDL/GDL internal SYSTIME
;
command='date +"%a %b %d %T %Y"'
command_C='LC_ALL=C '+command
;
; We cannot make a direct test because we are not sure
; to be in the same sec., then min. then hour. !
; But the true test is on Month !
;
resu_GDL=SYSTIME()
SPAWN, command_C, resu_LC_ALL
SPAWN, command, resu
;
debut_GDL=STRMID(resu_GDL,0,7)
debut_C=STRMID(resu_LC_ALL,0,7)
debut_brut=STRMID(resu,0,7)
;
if (debut_GDL NE debut_C) then begin
   MESSAGE, /continue, 'Problem with LOCALE in GDL'
   nb_pbs_locale=1
   ;;if ~KEYWORD_SET(no_exit) then EXIT, status=1
endif
;
; this test is not pertinent for GDL,
; it is only to inform that you may have problems
; due to your LOCALE
;
;if (debut_brut NE debut_C) then begin
;   MESSAGE, /continue, 'Warning: for your LOCALE outside GDL'
;   MESSAGE, /continue, 'Warning: it is expected C locale for good compilation'
;endif
;
if KEYWORD_SET(verbose) then begin
   print, 'first two Lines should be equal, except may be in the "second" value !' 
   print, 'resu_GDL     : ', resu_GDL
   print, 'resu_LC_ALL  : ', resu_LC_ALL
   print, 'resu (no LC) : ', resu
endif
;
BANNER_FOR_TESTSUITE, "TEST_SYSTIME_LOCALE", nb_pbs_locale, /short, verb=verbose
;
ERRORS_CUMUL, cumul_errors, nb_pbs_locale
;
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
pro TEST_SYSTIME_ZERO, cumul_errors, verbose=verbose, test=test, help=help
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_SYSTIME_LOCALE, nb_pbs_locale, help=help, test=test, verbose=verbose'
    print, ''
    print, 'we test whether we use C locale, as internaly needed in GDL'
    return
 endif
;
; should return something like : Wed May 13 16:19:34 2015
;
nb_pbs_zero=0
;
resu_vide=SYSTIME()
;
; let's say all we be done in less than 5 sec. ...
; do we need a delay ?
while (STRMID(resu_vide,17,2) GT 58) do begin   
   wait, 1
   resu_vide=SYSTIME()
endwhile
;
prefix=STRMID(resu_vide,0,16)
;
; various inputs need to be tested ...
;
resu_zero=SYSTIME(0)
if (prefix NE STRMID(resu_zero,0,16)) then ADD_ERRORS, nb_pbs_zero, 'case 1 i'
resu_zero=SYSTIME(0.0)
if (prefix NE STRMID(resu_zero,0,16)) then ADD_ERRORS, nb_pbs_zero, 'case 1 f'
resu_zero=SYSTIME(0.0d)
if (prefix NE STRMID(resu_zero,0,16)) then ADD_ERRORS, nb_pbs_zero, 'case 1 d'
resu_zero=SYSTIME([0])
if (prefix NE STRMID(resu_zero,0,16)) then ADD_ERRORS, nb_pbs_zero, 'case 1 i a'
resu_zero=SYSTIME([0.0])
if (prefix NE STRMID(resu_zero,0,16)) then ADD_ERRORS, nb_pbs_zero, 'case 1 f a'
resu_zero=SYSTIME(0.9)
if (prefix NE STRMID(resu_zero,0,16)) then ADD_ERRORS, nb_pbs_zero, 'case 1+ f'
resu_zero=SYSTIME(-0.9)
if (prefix NE STRMID(resu_zero,0,16)) then ADD_ERRORS, nb_pbs_zero, 'case 1- f'
resu_zero=SYSTIME([0.9])
if (prefix NE STRMID(resu_zero,0,16)) then ADD_ERRORS, nb_pbs_zero, 'case 1+ a'
resu_zero=SYSTIME([-0.9])
if (prefix NE STRMID(resu_zero,0,16)) then ADD_ERRORS, nb_pbs_zero, 'case 1- a'
;
; testing string conversion
;
resu_zero=SYSTIME('0.0')
if (prefix NE STRMID(resu_zero,0,16)) then ADD_ERRORS, nb_pbs_zero, 'case 2'
resu_zero=SYSTIME('0.9')
if (prefix NE STRMID(resu_zero,0,16)) then ADD_ERRORS, nb_pbs_zero, 'case 2+'
resu_zero=SYSTIME('-0.9')
if (prefix NE STRMID(resu_zero,0,16)) then ADD_ERRORS, nb_pbs_zero, 'case 2-'
resu_zero=SYSTIME(['0.0'])
if (prefix NE STRMID(resu_zero,0,16)) then ADD_ERRORS, nb_pbs_zero, 'case 2 a'
resu_zero=SYSTIME(['0.9'])
if (prefix NE STRMID(resu_zero,0,16)) then ADD_ERRORS, nb_pbs_zero, 'case 2+ a'
resu_zero=SYSTIME(['-0.9'])
if (prefix NE STRMID(resu_zero,0,16)) then ADD_ERRORS, nb_pbs_zero, 'case 2- a'
;
BANNER_FOR_TESTSUITE, "TEST_SYSTIME_ZERO", nb_pbs_zero, /short, verb=verbose
;
ERRORS_CUMUL, cumul_errors, nb_pbs_zero
;
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------------------------------
;
pro TEST_SYSTIME_EPOCH, cumul_errors, verbose=verbose, test=test, help=help
;
nb_pbs_epoch=0
;
; Unix billennium
expected='Sun Sep 09 01:46:40 2001'
value=SYSTIME(0,1000000000,/utc)
if (expected NE value) then ADD_ERRORS, nb_pbs_epoch, 'Unix billennium conversion failed !'
;
; we do have a Float/Double diff here:
; SYSTIME(0,1234567891,/utc) == SYSTIME(0,1234567891.0d,/utc) != SYSTIME(0,1234567891.0,/utc)
;
expected='Fri Feb 13 23:31:30 2009'
value=SYSTIME(0,1234567890,/utc)
if (expected NE value) then ADD_ERRORS, nb_pbs_epoch, 'Decimal Row Keyboard conversion failed !'
;
BANNER_FOR_TESTSUITE, "TEST_SYSTIME_EPOCH", nb_pbs_epoch, short=short, verb=verbose
;
ERRORS_CUMUL, cumul_errors, nb_pbs_epoch
;
if KEYWORD_SET(test) then STOP
;
end
; ----------------------------------------------------
;
pro TEST_SYSTIME_JULIAN, cumul_errors, verbose=verbose, test=test, help=help
;
; not ready now, other pbs found ...
MESSAGE,/continue, 'Not finished'
MESSAGE,/continue, 'Not finished, please contributed !'
MESSAGE,/continue, 'Not finished'
;
nb_pbs_julian=0
;
BANNER_FOR_TESTSUITE, "TEST_SYSTIME_JULIAN", nb_pbs_julian, /short, verb=verbose
;
ERRORS_CUMUL, cumul_errors, nb_pbs_julian
;
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------------------------------
;
pro TEST_SYSTIME, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_SYSTIME, help=help, test=test, no_exit=no_exit, verbose=verbose'
    print, ''
    print, 'few tests related to SYSTIME : locale, values ...'
    return
endif
;
cumul_errors=0
;
TEST_SYSTIME_LOCALE, cumul_errors, verbose=verbose, test=test
;
TEST_SYSTIME_ZERO, cumul_errors, verbose=verbose, test=test
;
TEST_SYSTIME_JULIAN, cumul_errors, verbose=verbose, test=test
;
BANNER_FOR_TESTSUITE, "TEST_SYSTIME", cumul_errors
;
; if /debug OR /test nodes, we don't want to exit
if (cumul_errors GT 0) then begin
    if ~KEYWORD_SET(verbose) then MESSAGE, /continue, 're-run with /verbose for details'
    if ~(KEYWORD_SET(test) or KEYWORD_SET(no_exit)) then EXIT, status=1
endif
;
if KEYWORD_SET(test) then STOP
;
end

