;
; various basic tests on FILE_TEST()
; written by NATCHKEBIA Ilia, May 2015
; under GNU GPL v2 or any later
;
; -----------------------------------------------
; 
; Modifications history :
;
; - 2018 G. Jung Use FILE_LINK, FILE_MKDIR, and FILE_DELETE instead of SPAWN.
; - 2018-09-24 : AC. 
;     * Splitting code in various topics.
;     * Adding tests on Dangling Symlinks ...
;     * Adding new tests on Unix files (get_mode=, special type ...)
;
; -----------------------------------------------
;
pro TEST_FILE_TEST_DIR_DANGLING, cumul_errors, test=test
;
if (!version.os_family NE 'unix') then begin
   MESSAGE, /continue, 'This code works only on Unix-like OS'
   return
endif
;
nb_errors=0
;
; Testing dangling symlink
;
tdirsym1='test_dangling_Symlink_Dir_for_FILE_TEST_1'
tdirsym2='test_dangling_Symlink_Dir_for_FILE_TEST_2'
;
; cleaning to avoid unwanted message
FILE_DELETE, tdirsym1, tdirsym2, /allow_nonexistent
;
FILE_MKDIR, tdirsym1
FILE_LINK, tdirsym1, tdirsym2
FILE_DELETE, tdirsym1
;
; if "tdirsym1" fully removed ? dir, symlink & dangling_symlink ?
if FILE_TEST(tdirsym1) then ERRORS_ADD, nb_errors, 'dir 1 not removed' 
if FILE_TEST(tdirsym1, /regular) then ERRORS_ADD, nb_errors, 'dir. 1 still regular'
if FILE_TEST(tdirsym1, /symlink) then ERRORS_ADD, nb_errors, 'dir. 1 still symlink' 
if FILE_TEST(tdirsym1, /dangling) then ERRORS_ADD, nb_errors, 'dir. 1 see as dangling'
;
if FILE_TEST(tdirsym2) then ERRORS_ADD, nb_errors, 'dir. 2 not removed' 
if FILE_TEST(tdirsym2, /regular) then ERRORS_ADD, nb_errors, 'dir. 2 still regular'
if FILE_TEST(tdirsym2, /symlink) then ERRORS_ADD, nb_errors, 'dir. 2 still symlink' 
if ~FILE_TEST(tdirsym2, /dangling) then ERRORS_ADD, nb_errors, 'dir. 2 NOT see as dangling'
;
; removing dangling file 2
FILE_DELETE, tdirsym2
;
; we redo all the test because something wrong may happen 
; on dangling symlink ! (not being well done as expected !)
if FILE_TEST(tdirsym2) then ERRORS_ADD, nb_errors, 'dir. 2 not removed (after)' 
if FILE_TEST(tdirsym2, /regular) then ERRORS_ADD, nb_errors, 'dir. 2 still regular (after)'
if FILE_TEST(tdirsym2, /symlink) then ERRORS_ADD, nb_errors, 'dir. 2 still symlink (after)' 
if FILE_TEST(tdirsym2, /dangling) then ERRORS_ADD, nb_errors, 'dir. 2 still see as dangling (after)' 
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_FILE_TEST_DIR_DANGLING', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
pro TEST_FILE_TEST_DIR_BASIC, cumul_errors, test=test
;
nb_errors=0
;
; Create test directory
tdir='tdir_for_file_test'
FILE_MKDIR, tdir
;
; Test if exists
if ~FILE_TEST(tdir) then ERRORS_ADD, nb_errors, 'Dir. not detected'
; Test if it is directory
if ~FILE_TEST(tdir, /dir) then ERRORS_ADD, nb_errors, 'Dir. not considered as Dir'
; Test if it is symlink
if FILE_TEST(tdir, /sym) then ERRORS_ADD, nb_errors, 'Dir. is considered as symlink (but should not !)'
; Test if it is regular
if FILE_TEST(tdir, /regular) then ERRORS_ADD, nb_errors, 'Dir. is considered as a regular file (but should not !)'
;
; cleaning
FILE_DELETE, tdir
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_FILE_TEST_DIR_BASIC', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
pro TEST_FILE_TEST_DIR_SYMLINK, cumul_errors, test=test
;
if (!version.os_family NE 'unix') then begin
   MESSAGE, /continue, 'This code works only on Unix-like OS'
   return
endif
;
nb_errors=0
;
; Create test directory
tdir='tdir_for_file_test'
FILE_MKDIR, tdir
;
;Create test folder symlink
tdirsym='testSymlinkDirectory_for_FILE_TEST'
FILE_LINK, tdir, tdirsym
;
;Test if it exists
if ~FILE_TEST(tdirsym) then ERRORS_ADD, nb_errors, 'symlink of Dir. not detected'
;Test if it is symlink of directory
if ~FILE_TEST(tdirsym, /dir) then ERRORS_ADD, nb_errors, 'symlink of Dir. not considered as Dir.'
;Test if it is symlink
if ~FILE_TEST(tdirsym, /sym) then ERRORS_ADD, nb_errors, 'symlink is not considered as symlink'
;
;Remove test directory and symlink
FILE_DELETE, /recur, tdir
FILE_DELETE, tdirsym
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_FILE_TEST_DIR_SYMLINK', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------

pro TEST_FILE_TEST_FILE_BASIC, cumul_errors, test=test
;
nb_errors=0
;
; Create 2 test files : one without nothing, the other one with content
tfile1='tfile_for_FILE_TEST_void'
tfile2='tfile_for_FILE_TEST_non_void'
;
; cleaning to avoid unwanted message
FILE_DELETE, tfile1, tfile2, /allow_nonexistent
;
; creating a void file
OPENW, flun, tfile1, /get_lun
CLOSE, flun
FREE_LUN, flun
;
OPENW, flun, tfile2, /get_lun
PRINTF, flun, 'this is a non void file !'
CLOSE, flun
FREE_LUN, flun
;
; Test if both files exist
if ~FILE_TEST(tfile1) then ERRORS_ADD, nb_errors, 'file 1 not detected'
if ~FILE_TEST(tfile2) then ERRORS_ADD, nb_errors, 'file 2 not detected'
;
; Test if it is directory
if FILE_TEST(tfile1, /dir) then ERRORS_ADD, nb_errors, 'file 1 is considered as Dir.'
if FILE_TEST(tfile2, /dir) then ERRORS_ADD, nb_errors, 'file 2 is considered as Dir.'
;
; Test if it is symlink
if (!version.os_family EQ 'unix') then begin
   if FILE_TEST(tfile1, /sym) then ERRORS_ADD, nb_errors, 'file 1 is considered as symlink'
   if FILE_TEST(tfile2, /sym) eq 1 then ERRORS_ADD, nb_errors, 'file 2 is considered as symlink'
endif
;
; Test if it is symlink
if ~FILE_TEST(tfile1, /zero_length) then ERRORS_ADD, nb_errors, 'file 1 is NOT considered as zero_length'
if FILE_TEST(tfile2, /zero_length) then ERRORS_ADD, nb_errors, 'file 2 is considered as zero_length'
;
; Test if regular
if ~FILE_TEST(tfile1, /regular) then ERRORS_ADD, nb_errors, 'file 1 is NOT considered as zero_length'
if ~FILE_TEST(tfile2, /regular) then ERRORS_ADD, nb_errors, 'file 2 is considered as zero_length'
;
; final cleaning
FILE_DELETE, tfile1, tfile2, /allow_nonexistent

; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_FILE_TEST_FILE_BASIC', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
pro TEST_FILE_TEST_FILE_SYMLINK, cumul_errors, test=test
;
if (!version.os_family NE 'unix') then begin
   MESSAGE, /continue, 'This code works only on Unix-like OS'
   return
endif
;
nb_errors=0
;
tfile='tfile_for_FILE_TEST'
; cleaning to avoid unwanted message
FILE_DELETE, tfile, /allow_nonexistent
; creating
OPENW, flun, tfile, /get_lun
CLOSE, flun
FREE_LUN, flun
;
; Create test file symlink
tfilesym='testSymlinkFile_for_FILE_TEST'
FILE_LINK, tfile, tfilesym
;
; Test if it exists
if ~FILE_TEST(tfilesym) then ERRORS_ADD, nb_errors, 'symlink of file not detected'
; Test if it is symlink of directory
if FILE_TEST(tfilesym, /dir) then ERRORS_ADD, nb_errors, 'symlink of file is considered as Dir.'
; Test if it is symlink
if ~FILE_TEST(tfilesym, /sym) then ERRORS_ADD, nb_errors, 'symlink of file is not considered as symlink'
;
; Remove test file and symlink
FILE_DELETE, tfile, tfilesym, /allow_nonexistent
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_FILE_TEST_FILE_SYMLINK', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
; testing special files
; https://askubuntu.com/questions/397493/what-does-the-first-character-of-unix-mode-string-indicate
;
pro TEST_FILE_TEST_UNIX, cumul_errors, test=test
;
if (!version.os_family NE 'unix') then begin
   MESSAGE, /continue, 'This code works only on Unix-like OS'
   return
endif
;
nb_errors=0
;
; testing /char file
;
special_file='/dev/urandom'
mess='This special file '+special_file+' '
;
; Test if it exists
if ~FILE_TEST(special_file) then ERRORS_ADD, nb_errors, mess+'should exist !'
; Test if it is symlink of directory
if ~FILE_TEST(special_file, /char) then ERRORS_ADD, nb_errors, mess+'is a Char (crw-rw-rw-) !'
; Test if it is symlink
if FILE_TEST(special_file, /sym) then ERRORS_ADD, nb_errors, mess+'is not a symlink !'
;; Test if it is regular
if FILE_TEST(special_file, /regular) then ERRORS_ADD, nb_errors, mess+'is NOT a regular !'
;
; testing /block file
;
special_file=''
if (!version.os EQ 'linux') then special_file='/dev/sda'
if (!version.os EQ 'darwin') then special_file='/dev/disk0'
if (STRLEN(special_file) EQ 0) then begin
   MESSAGE, /continue, 'We don''t know a special Block file on your OS'
   MESSAGE, /continue, 'Please contribute !! FixMe'
endif else begin
   mess='This special file '+special_file+' '
   if ~FILE_TEST(special_file) then ERRORS_ADD, nb_errors, mess+'should exist !'
   ;; Test if it is symlink of directory
   if FILE_TEST(special_file, /char) then ERRORS_ADD, nb_errors, mess+'is NOT a Char (crw-rw-rw-) !'
   ;; Test if it is symlink
   if FILE_TEST(special_file, /sym) then ERRORS_ADD, nb_errors, mess+'is NOT a symlink !'
   ;; Test if it is block
   if ~FILE_TEST(special_file, /block) then ERRORS_ADD, nb_errors, mess+'must be a block device !'
   ;; Test if it is regular
   if FILE_TEST(special_file, /regular) then ERRORS_ADD, nb_errors, mess+'is NOT a regular !'
endelse

; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_FILE_TEST_UNIX', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
; testing returned mode for some files
;
pro TEST_FILE_TEST_GET_MODE, cumul_errors, test=test
;
if (!version.os_family NE 'unix') then begin
   MESSAGE, /continue, 'This code is ready now only Unix-like OS'
   MESSAGE, /continue, 'Please contribute ! FixMe'
   return
endif
;
nb_errors=0
;
file=FILE_WHICH('Saturn.jpg')
mess='This file '+file+' '
extected_mode=436L
if ~FILE_TEST(file, get_mode=mode) then begin
   ERRORS_ADD, nb_errors, mess+'should exist !'
endif else begin
   if ~ARRAY_EQUAL(extected_mode, mode) then ERRORS_ADD, nb_errors, mess+'has a bad mode !'
endelse
;
; testing /char file
;
file_char='/dev/urandom'
mess='This file '+file_char+' '
extected_mode=438L
if ~FILE_TEST(file_char, get_mode=mode) then begin
   ERRORS_ADD, nb_errors, mess+'should exist !'
endif else begin
   if ~ARRAY_EQUAL(extected_mode, mode) then ERRORS_ADD, nb_errors, mess+'has a bad mode !'
endelse
;
; testing /block file
;
file_block=''
if (!version.os EQ 'linux') then file_block='/dev/sda'
if (!version.os EQ 'darwin') then file_block='/dev/disk0'
if (STRLEN(file_block) EQ 0) then begin
   MESSAGE, /continue, 'We don''t know a special Block file on your OS'
   MESSAGE, /continue, 'Please contribute !! FixMe'
endif else begin
   mess='This file '+file_block+' '
   extected_mode=432L
   if ~FILE_TEST(file_block, get_mode=mode) then begin
      ERRORS_ADD, nb_errors, mess+'should exist !'
   endif else begin
      if ~ARRAY_EQUAL(extected_mode, mode) then ERRORS_ADD, nb_errors, mess+'has a bad mode !'
   endelse
endelse
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_FILE_TEST_GET_MODE', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
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
cumul_errors=0
;
TEST_FILE_TEST_UNIX, cumul_errors, test=test
TEST_FILE_TEST_GET_MODE, cumul_errors, test=test
;
TEST_FILE_TEST_FILE_BASIC, cumul_errors, test=test
TEST_FILE_TEST_FILE_SYMLINK, cumul_errors, test=test
;
TEST_FILE_TEST_DIR_BASIC, cumul_errors, test=test
TEST_FILE_TEST_DIR_SYMLINK, cumul_errors, test=test
TEST_FILE_TEST_DIR_DANGLING, cumul_errors, test=test
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_FILE_TEST', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
