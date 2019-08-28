;
; TEST_FILE_MKDIR Greg Jung July 2009
;
pro TEST_FILE_MKDIR, help=help, test=test, $
            verbose=verbose,  no_exit=no_exit
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_FILE_SEARCH, help=help, test=test, verbose=verbose, $'
    print, '                      no_exit=no_exit'
    return
endif
;
cumul_errors=0
;
sandbox='TMPDIR_FILE_MKDIR'
if file_test(sandbox) eq 0 then FILE_MKDIR, sandbox $
    else begin
      message,' a test directory already exists! ./'+sandbox,/continue
      if keyword_set(no_exit) then return else EXIT, status=1
    endelse
;
; make a file that we won't be able to create a directory over:
openw,/get_lun,lu,sandbox+'/aaafile' & free_lun,lu
CD, sandbox, cur=cur
;
afile = 'aaafile'
if file_test(afile) eq 0 then begin
    message,/continue,' test file not found!'+sandbox+'/aaafile'
    CD, cur
    if keyword_set(no_exit) then return else EXIT, status=1
    endif
if KEYWORD_SET(verbose) then message,/continue,$
        ' attempting to create directory over a file'
++cumul_errors;
catch, mkdir_err
if mkdir_err ne 0 then begin &$
  catch,/cancel & --cumul_errors & message,/continue,' expected error caught' & endif $
  else $
file_mkdir,afile
adir = 'adir'

file_mkdir,adir
if KEYWORD_SET(verbose) then message,/continue,$
        ' verify created directory "adir"'
if ~file_test(adir,/directory) then begin
    message,/continue,' test directory fail: '+sandbox+'/adir'+' not found!'
    CD, cur
    if keyword_set(no_exit) then return else EXIT, status=1
    endif
if KEYWORD_SET(verbose) then message,/continue,$
        ' create and verify directories'
tdirs=['adir/b/c/d','bdir']
file_mkdir,tdirs,'cdir'
file_mkdir,'b/c/d/e','./b/c'
chkdirs = [tdirs,'cdir','b/c/d/e','./b/c']
if KEYWORD_SET(verbose) then print,$
        ' : ' + chkdirs
foreach c, chkdirs do $
  if ~file_test(c,/directory) then begin &$
  message,/continue,' test directory fail'+sandbox+c &$
  cumul_errors++ & endif
  
if KEYWORD_SET(verbose) then message,/continue,$
        ' another attempt to create directory over a file'
CD,cur
direrr = sandbox+'/aaafile/a/b/c'
++cumul_errors;
catch, mkdir_err
if mkdir_err ne 0 then begin &$
  catch,/cancel & --cumul_errors  &  message,/continue,' expected error caught' & endif $
  else $
file_mkdir,direrr
FILE_DELETE, sandbox, /recursive
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, "TEST_FILE_MKDIR", cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

