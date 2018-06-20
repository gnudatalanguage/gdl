;
; Doing a serie of tests for FILE_DIRNAME(),
; including mandatory escape of special chars
;
; Lea Noreskal and Alain Coulais
; June 2010
; under GNU GPL 2 or later
;
pro TEST_FILE_DIRNAME, test=test

fr = FILE_DIRNAME('GDLTokenTy\*') & expect = '.'
if(!Version.OS_family) eq 'Windows' then expect='GDLTokenTy'
if (fr NE expect) then $
   MESSAGE, /continue, ' FILE_DIRNAME(GDLTokenTy\*) NE '+expect
numerror = 0;
;
fr = FILE_DIRNAME('GDLTokenTy*') & expect = '.'
if (fr NE expect) then $
   MESSAGE, /continue, ' FILE_DIRNAME(GDLTokenTy\*) NE '+expect
if (fr NE expect) then numerror++;

fr =  FILE_DIRNAME('GDLTokenTy*/*') & expect = 'GDLTokenTy*'

if (fr NE expect) then $
   MESSAGE, /continue, 'FILE_DIRNAME(GDLTokenTy*/*) NE '+expect
if (fr NE expect) then numerror++;

;
fr = FILE_DIRNAME('GDLTokenTy/*') & expect = 'GDLTokenTy'

if (fr NE expect) then $
   MESSAGE, /continue, 'FILE_DIRNAME(GDLTokenTy/*) NE '+expect
if (fr NE expect) then numerror++;

; 
fr = FILE_DIRNAME('/home/GDL/test/of/dir/name/*/*')
expect= '/home/GDL/test/of/dir/name/*'

if (fr NE expect) then $
   MESSAGE, /continue, 'FILE_DIRNAME(/home/GDL/test/of/dir/name/*/*) NE '+expect
if (fr NE expect) then numerror++;

;
if(numerror eq 0) then $
	MESSAGE, /continue, 'All tests done with success' $
	else $
	MESSAGE, /continue, 'NOT ALL tests were successful #'+string( numerror)
	
;
if KEYWORD_SET(test) then STOP
;
end
;
print, FILE_DIRNAME('GDLTokenTy\*') ; .
print, FILE_DIRNAME('GDLTokenTy*') ; .
print, FILE_DIRNAME('GDLTokenTy*/*') ; GDLTokenTy*
print, FILE_DIRNAME('GDLTokenTy/*') ; GDLTokenTy*
print, FILE_DIRNAME('GDLTokenTy/*') ; GDLTokenTy*
print, FILE_DIRNAME('GDLTokenTy*/*') ; GDLTokenTy*
print, FILE_DIRNAME('/home/GDL/test/of/dir/name/*/*') ;
;/home/GDL/test/of/dir/name/*
end
