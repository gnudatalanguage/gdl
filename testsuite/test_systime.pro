;
; Alain Coulais and Sylwester Arabas, during the GHM 2011 in Paris.
; Copyright 2011
; This code is under GNU GPL v2 or any later.
;
; In some cases, when external libraries are compiled with
; non "C" locales, this can affect internal procedures in GDL ...
; One case was SYSTIME(), broken by external Image Magick with "bad" locale
;
; This test must be call before any use of LOCALE_GET().
; This test is not perfect because internal locale in GDL are changed
; by a call to LOCALE_GET().
;
pro TEST_SYSTIME, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_SYSTIME, help=help, test=test, no_exit=no_exit, verbose=verbose'
    print, ''
    print, 'we test whether we use C locale, as internaly needed in GDL'
    return
endif
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
   if ~KEYWORD_SET(no_exit) then EXIT, status=1
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
   print, 'first two Lines should be equal, except at second diff !' 
   print, 'resu_GDL     : ', resu_GDL
   print, 'resu_LC_ALL  : ', resu_LC_ALL
   print, 'resu (no LC) : ', resu
endif
;
if KEYWORD_SET(test) then STOP
;
end
