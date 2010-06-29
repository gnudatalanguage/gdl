;
; Doing a serie of tests for FILE_DIRNAME(),
; including mandatory escape of special chars
;
; Lea Noreskal and Alain Coulais
; June 2010
; under GNU GPL 2 or later
;
pro TEST_FILE_DIRNAME, test=test

;print, FILE_DIRNAME('GDLTokenTy\*') ; .
if (FILE_DIRNAME('GDLTokenTy\*') NE '.') then begin
   MESSAGE, /continue, ' FILE_DIRNAME(GDLTokenTy\*) NE .'
   EXIT, status=1
endif
;
;print, FILE_DIRNAME('GDLTokenTy*') ; .
if (FILE_DIRNAME('GDLTokenTy*') NE '.') then begin
   MESSAGE, /continue, 'FILE_DIRNAME : FILE_DIRNAME(GDLTokenTy\*) NE .'
   EXIT, status=1
endif
;
;print, FILE_DIRNAME('GDLTokenTy*/*') ; GDLTokenTy*
if (FILE_DIRNAME('GDLTokenTy*/*') NE 'GDLTokenTy*') then begin
   MESSAGE, /continue, 'FILE_DIRNAME(GDLTokenTy*/*) NE GDLTokenTy*'
   EXIT, status=1
endif
;
;print, FILE_DIRNAME('GDLTokenTy/*') ; GDLTokenTy*
if (FILE_DIRNAME('GDLTokenTy/*') NE 'GDLTokenTy') then begin
   MESSAGE, /continue, 'FILE_DIRNAME(GDLTokenTy/*) NE GDLTokenTy'
   EXIT, status=1
endif
;
;print, FILE_DIRNAME('/home/GDL/test/of/dir/name/*/*') ;
;/home/GDL/test/of/dir/name/*
; 
my_path='/home/GDL/test/of/dir/name/*/*'
if (FILE_DIRNAME(my_path) NE '/home/GDL/test/of/dir/name/*') then begin
   MESSAGE, /continue, 'FILE_DIRNAME(/home/GDL/test/of/dir/name/*/*) NE /home/GDL/test/of/dir/name/*'
   EXIT, status=1
endif
;
MESSAGE, /continue, 'All tests done with success'
;
if KEYWORD_SET(test) then STOP
;
end
;
