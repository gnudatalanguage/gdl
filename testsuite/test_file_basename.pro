;
; Doing a serie of tests for FILE_BASENAME(),
; including mandatory escape of special chars
;
; Lea Noreskal and Alain Coulais
; June 2010
; under GNU GPL 2 or later
;
pro TEST_FILE_BASENAME , test=test
;
;print, FILE_BASENAME('GDLTokenTy*') ; GDLTokenTy*
if (FILE_BASENAME('GDLTokenTy*') NE 'GDLTokenTy*') then begin
    MESSAGE, /continue, 'FILE_BASENAME(GDLTokenTy*) NE GDLTokenTy*'
    EXIT, status=1
endif
;

;print, FILE_BASENAME('GDLTokenTy\*') ; GDLTokenTy\*
if (FILE_BASENAME('GDLTokenTy\*') NE 'GDLTokenTy\*') then begin
    MESSAGE, /continue, 'FILE_BASENAME(GDLTokenTy\*) NE GDLTokenTy\* '
    EXIT, status=1
endif
;
my_file='/usr/share/gnudatalanguage/lib/dist.pro'
;
;print, FILE_BASENAME(my_file, '.pro') ;  dist
my_test='.pro'
if (FILE_BASENAME(my_file, my_test) NE 'dist') then begin
    MESSAGE, /continue, 'FILE_BASENAME('''+my_file+''', '''+my_test+''' ) NE dist.pro'
    EXIT, status=1
endif
;
;print, FILE_BASENAME(my_file, '.pr*') ; dist.pro
my_test='.pr*'
if (FILE_BASENAME(my_file, '.pr*') NE 'dist.pro') then begin
    MESSAGE, /continue, 'FILE_BASENAME('''+my_file+''', '''+my_test+''' ) NE dist.pro'
    EXIT, status=1
endif
;
;print, FILE_BASENAME(my_file, '*') ; dist.pro
my_test='*'
if (FILE_BASENAME(my_file, '*') NE 'dist.pro') then begin
    MESSAGE, /continue, 'FILE_BASENAME('''+my_file+''', '''+my_test+''' ) NE dist.pro'
    EXIT, status=1
endif
;
;print, FILE_BASENAME(my_file, '.') ; dist.pro
my_test='.'
if (FILE_BASENAME(my_file, my_test) NE 'dist.pro') then begin
    MESSAGE, /continue, 'FILE_BASENAME('''+my_file+''', '''+my_test+''' ) NE dist.pro'
    EXIT, status=1
endif
;
MESSAGE, /continue, 'All tests done with success'
;
if KEYWORD_SET(test) then STOP
;
end
;

