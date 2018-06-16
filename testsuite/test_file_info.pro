;
; various basic tests on FILE_INFO() based on FILE_TEST()
; written by NATCHKEBIA Ilia, May 2015
; under GNU GPL v2 or any later
;
; -----------------------------------------------
;
pro TEST_FILE_INFO, test=test, no_exit=no_exit, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FILE_INFO, test=test, no_exit=no_exit, help=help'
   return
endif
;
;Errors count is 0 at the beginning
total_errors = 0
;
; Create test directory
tdir='tdir_for_FILE_INFO'
SPAWN, 'mkdir '+tdir
dirInfo=FILE_INFO(tdir)
;
; various tests
;
if (dirInfo.name NE tdir) then ERRORS_ADDS, total_errors, 'bad dir. name'
if (dirInfo.exists NE 1) then ERRORS_ADDS, total_errors, 'Dir. not detected'
if (dirInfo.directory NE 1) then ERRORS_ADDS, total_errors, 'Dir. not considered as Dir'
;Test if it is symlink
if (dirInfo.symlink NE 0) then ERRORS_ADDS, total_errors, 'Dir. is considered as symlink'
;
;Create test folder symlink
tdirsym='testSymlinkDirectory_for_FILE_INFO'
SPAWN, 'ln -s '+tdir+" "+tdirsym
;info
dirsyminfo=FILE_INFO(tdirsym)
;Test if it exists
if (dirsyminfo.exists NE 1) then ERRORS_ADDS, total_errors, 'symlink of Dir. not detected'
;Test if it is symlink of directory
if (dirsyminfo.directory NE 1) then ERRORS_ADDS, total_errors, 'symlink of Dir. not considered as Dir.'
;Test if it is symlink
if (dirsyminfo.symlink NE 1) then ERRORS_ADDS, total_errors, 'symlink is not considered as symlink'
;Remove test directory and symlink
SPAWN, 'rm -r '+tdir
SPAWN, 'rm '+tdirsym
;
;
;Create test file
tfile='tfile_for_FILE_INFO'
SPAWN, 'touch '+tfile
;info
fileinfo=FILE_INFO(tfile)
;Test if it exists
if (fileinfo.exists NE 1) then ERRORS_ADDS, total_errors, 'file not detected'
;Test if it is directory
if (fileinfo.directory NE 0) then ERRORS_ADDS, total_errors, 'file is considered as Dir.'
;Test if it is symlink
if (fileinfo.symlink NE 0) then ERRORS_ADDS, total_errors, 'file is considered as symlink'
;
;Create test file symlink
tfilesym='testSymlinkFile_for_FILE_INFO'
SPAWN, 'ln -s '+tfile+" "+tfilesym
;info
filesyminfo=FILE_INFO(tfilesym)
;Test if it exists
if (filesyminfo.exists NE 1) then ERRORS_ADDS, total_errors, 'symlink of file not detected'
;Test if it is symlink of directory
if (filesyminfo.directory NE 0) then ERRORS_ADDS, total_errors, 'symlink of file is considered as Dir.'
;Test if it is symlink
if (filesyminfo.symlink NE 1) then ERRORS_ADDS, total_errors, 'symlink of file is not considered as symlink'
;Remove test file and symlink
SPAWN, 'rm '+tfile
SPAWN, 'rm '+tfilesym
;
; final message
;
BANNER_FOR_TESTSUITE, 'TEST_FILE_INFO', total_errors
;
if KEYWORD_SET(test) then STOP
;
if (total_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
end
