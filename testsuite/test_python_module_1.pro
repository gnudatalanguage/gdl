;
; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
;
; uses GDL.so symlink created in test_python_module_0 and deleted test_python_module_2
;
; ---------------------------------
;
; 2018-SEP-18 : AC : various small fix 
; 
; ---------------------------------
; 
pro TEST_PYTHON_MODULE_1, no_exit=no_exit, $
                          help=help, test=test, verbose=verbose
;
if (STRlowCase(!version.os_family) EQ 'windows') then begin
   MESSAGE, /continue, 'Not ready for MSwin, please contribute (!fixme!)'
   exit, status=77
endif
;
; link between dylib & so done in module_0 ;)
if ~FILE_TEST('GDL.so') then begin
   MESSAGE, /continue,  'GDL.so not found'
   EXIT, status=77
endif
;
if STRlowCase(!version.os) eq 'darwin' then begin
   SPAWN, 'otool -L GDL.so | grep Python | cut -d'' '' -f1', python
   python = FILE_DIRNAME(STRTRIM(python, 2)) + '/bin/python'
endif else begin
   SPAWN, 'ldd GDL.so | grep python | cut -d'' '' -f3', python
   python = FILE_DIRNAME(STRTRIM(python, 2)) + '/../bin/python' 
   ;; tODO python2.X
endelse
;
if ~FILE_TEST(python) then begin
   MESSAGE, /continue, python + ' not found'
   EXIT, status=77
endif
;
; testing if importing works
;
SPAWN, 'echo "import GDL" | ' + python, exit_status=ex
if (ex ne 0) then ERRORS_ADD, cumul_errors, 'import GDL'
;
; testing GDL.pro
;
command='echo "import GDL; GDL.pro(\"print\", \"Hello world!\")" | '
SPAWN, command + python, out, exit_status=ex
;
if (ex ne 0) then ERRORS_ADD, cumul_errors, 'testing  GDL.pro'
if (STRPOS(out[0], 'Hello world!') eq -1) then $
   ERRORS_ADD, cumul_errors, 'Hello world != Hello world (GDL.pro failed?)'
;
; testing GDL.function
;
command='echo "import GDL; print GDL.function(\"sin\", 1)" | '
SPAWN, command + python, out, exit_status=ex
;
if (ex ne 0) then ERRORS_ADD, cumul_errors, 'testing  GDL.function'
if (STRPOS(out[0], '0.841') eq -1) then $
   ERRORS_ADD, cumul_errors, 'sin(1) != 0.841... (GDL.function failed?)'
;
; testing Numpy
;
command='echo "import GDL; print GDL.function(\"findgen\", 100)" | '
SPAWN, command + python, out, exit_status=ex
;
if (ex ne 0) then ERRORS_ADD, cumul_errors, 'testing Numpy'
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_PYTHON_MODULE_1', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

