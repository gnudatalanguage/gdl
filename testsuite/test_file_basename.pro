;
; Doing a serie of tests for FILE_BASENAME(),
; including mandatory escape of special chars
;
; Lea Noreskal and Alain Coulais
; June 2010
; under GNU GPL 2 or later
;
; ---------------------------------
; 
; Modifications history :
;
; - 2025-04-15 : AC. using cumulative error count !
;
; ---------------------------------
;
pro TEST_FILE_BASENAME, help=help, no_exit=no_exit, test=test
; 
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FILE_BASENAME, help=help, no_exit=no_exit, test=test'
   return
endif
;
errors=0
;
;print, FILE_BASENAME('GDLTokenTy*') ; GDLTokenTy*
if (FILE_BASENAME('GDLTokenTy*') NE 'GDLTokenTy*') then begin
   ERRORS_ADD, errors, 'FILE_BASENAME(GDLTokenTy*) NE GDLTokenTy*'
endif
;
;print, FILE_BASENAME('GDLTokenTy\*') ; GDLTokenTy\*
Win32 = !version.OS_family eq 'Windows'
if (FILE_BASENAME('GDLTokenTy\*') NE 'GDLTokenTy\*' and ~Win32) then begin
    ERRORS_ADD, errors, 'FILE_BASENAME(GDLTokenTy\*) NE GDLTokenTy\* '
endif
;
my_file='/usr/share/gnudatalanguage/lib/dist.pro'
;
;print, FILE_BASENAME(my_file, '.pro') ;  dist
my_test='.pro'
if (FILE_BASENAME(my_file, my_test) NE 'dist') then begin
    ERRORS_ADD, errors, 'FILE_BASENAME('''+my_file+''', '''+my_test+''' ) NE dist.pro'
endif
;
;print, FILE_BASENAME(my_file, '.pr*') ; dist.pro
my_test='.pr*'
if (FILE_BASENAME(my_file, '.pr*') NE 'dist.pro') then begin
    ERRORS_ADD, errors, 'FILE_BASENAME('''+my_file+''', '''+my_test+''' ) NE dist.pro'
endif
;
;print, FILE_BASENAME(my_file, '*') ; dist.pro
my_test='*'
if (FILE_BASENAME(my_file, '*') NE 'dist.pro') then begin
    ERRORS_ADD, errors, 'FILE_BASENAME('''+my_file+''', '''+my_test+''' ) NE dist.pro'
endif
;
;print, FILE_BASENAME(my_file, '.') ; dist.pro
my_test='.'
if (FILE_BASENAME(my_file, my_test) NE 'dist.pro') then begin
    ERRORS_ADD, errors, 'FILE_BASENAME('''+my_file+''', '''+my_test+''' ) NE dist.pro'
endif
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_FILE_BASENAME', errors
;
if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
;

