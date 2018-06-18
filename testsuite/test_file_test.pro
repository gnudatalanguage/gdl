;
; various basic tests on FILE_TEST()
; written by NATCHKEBIA Ilia, May 2015
; under GNU GPL v2 or any later
;
; -----------------------------------------------
;
pro TEST_FILE_TEST, test=test, no_exit=no_exit, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FILE_TEST, test=test, no_exit=no_exit, help=help'
   return
endif
;
;Errors count is 0 at the beginning
total_errors = 0
;
; Create test directory
tdir='tdir_for_file_test'
SPAWN, 'mkdir '+tdir
;Test if exists
if FILE_TEST(tdir) eq 0 then ERRORS_ADDS, total_errors, 'Dir. not detected'
;Test if it is directory
if FILE_TEST(tdir,/dir) eq 0 then ERRORS_ADDS, total_errors, 'Dir. not considered as Dir'
;Test if it is symlink
if FILE_TEST(tdir,/sym) eq 1 then ERRORS_ADDS, total_errors, 'Dir. is considered as symlink'
;
;Create test folder symlink
tdirsym='testSymlinkDirectory_for_FILE_TEST'
SPAWN, 'ln -s '+tdir+" "+tdirsym
;Test if it exists
if FILE_TEST(tdirsym) eq 0 then ERRORS_ADDS, total_errors, 'symlink of Dir. not detected'
;Test if it is symlink of directory
if FILE_TEST(tdirsym,/dir) eq 0 then ERRORS_ADDS, total_errors, 'symlink of Dir. not considered as Dir.'
;Test if it is symlink
if FILE_TEST(tdirsym,/sym) eq 0 then ERRORS_ADDS, total_errors, 'symlink is not considered as symlink'
;Remove test directory and symlink
SPAWN, 'rm -r '+tdir
SPAWN, 'rm '+tdirsym
;
;
;Create test file
tfile='tfile_for_FILE_TEST'
SPAWN, 'touch '+tfile
;Test if it exists
if FILE_TEST(tfile) eq 0 then ERRORS_ADDS, total_errors, 'file not detected'
;Test if it is directory
if FILE_TEST(tfile,/dir) eq 1 then ERRORS_ADDS, total_errors, 'file is considered as Dir.'
;Test if it is symlink
if FILE_TEST(tfile,/sym) eq 1 then ERRORS_ADDS, total_errors, 'file is considered as symlink'
;
;Create test file symlink
tfilesym='testSymlinkFile_for_FILE_TEST'
SPAWN, 'ln -s '+tfile+" "+tfilesym
;Test if it exists
if FILE_TEST(tfilesym) eq 0 then ERRORS_ADDS, total_errors, 'symlink of file not detected'
;Test if it is symlink of directory
if FILE_TEST(tfilesym,/dir) eq 1 then ERRORS_ADDS, total_errors, 'symlink of file is considered as Dir.'
;Test if it is symlink
if FILE_TEST(tfilesym,/sym) eq 0 then ERRORS_ADDS, total_errors, 'symlink of file is not considered as symlink'
;Remove test file and symlink
SPAWN, 'rm '+tfile
SPAWN, 'rm '+tfilesym
;
; final message
;
BANNER_FOR_TESTSUITE, 'TEST_FILE_TEST', total_errors
;
if KEYWORD_SET(test) then STOP
;
if (total_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
end
