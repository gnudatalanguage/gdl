;
; Under GNU GPL V3 or later
;
; https://github.com/gnudatalanguage/gdl/issues/1709
; AC 2024-Jan-05
;
; Several bugs related to FILE_READLINK() found,
; https://github.com/gnudatalanguage/gdl/issues/1709
;
; ---------------------------------
;
; Modifications history :
;
; - 2024-01-05 : AC. creation. Ideas welcome since I don't have
;   MSwin and *BSD around ...
;
; - 2026-11-17 : Use temporary symlinks instead of assuming system paths and
;   timezone configuration. Keep the Linux /proc/self regression check.
;
; ---------------------------------
;
pro TEST_UNIX_READLINK, cumul_errors, verbose=verbose
;
nb_errors=0
tdir=''
created=0
lun=-1L
CATCH, error
if error NE 0 then begin
   CATCH, /cancel
   ERRORS_ADD, nb_errors, 'fixture setup/read: '+!ERROR_STATE.MSG
   GOTO, cleanup
endif
;
stamp=STRTRIM(STRING(SYSTIME(1), FORMAT='(F24.6)'), 2)
tdir=FILE_EXPAND_PATH(FILEPATH('gdl-readlink-'+stamp, /tmp))
if FILE_TEST(tdir, /noexpand_path) then MESSAGE, 'Temporary directory already exists: '+tdir
FILE_MKDIR, tdir
created=1
target=FILEPATH('target.txt', ROOT_DIR=tdir)
OPENW, lun, target, /get_lun
PRINTF, lun, 'FILE_READLINK fixture'
FREE_LUN, lun
lun=-1L
FILE_MKDIR, FILEPATH('subdir', ROOT_DIR=tdir)
;
expected=[target, 'target.txt', 'subdir', 'missing target.txt']
links=tdir+PATH_SEP()+['absolute', 'relative', 'directory', 'dangling']
; ln preserves the supplied target text, including relative and missing paths.
; /NOSHELL passes paths as arguments, without shell quoting or expansion.
for i=0,N_ELEMENTS(links)-1 do begin
   SPAWN, ['ln', '-s', expected[i], links[i]], output, errout, $
      /noshell, EXIT_STATUS=status
   if status NE 0 then MESSAGE, 'Cannot create symlink: '+STRJOIN(errout, ' ')
   actual=FILE_READLINK(links[i])
   if KEYWORD_SET(verbose) then PRINT, links[i]+' -> '+actual
   if actual NE expected[i] then ERRORS_ADD, nb_errors, 'wrong target for '+links[i]
endfor
;
if ~ARRAY_EQUAL(FILE_READLINK(links), expected) then $
   ERRORS_ADD, nb_errors, 'wrong targets for array input'
if FILE_READLINK(target, /allow_nonsymlink) NE '' then $
   ERRORS_ADD, nb_errors, '/ALLOW_NONSYMLINK on a regular file'
missing=FILEPATH('missing target.txt', ROOT_DIR=tdir)
if FILE_READLINK(missing, /allow_nonexistent) NE '' then $
   ERRORS_ADD, nb_errors, '/ALLOW_NONEXISTENT on a missing path'
;
cleanup:
CATCH, /cancel
if lun GE 0 then FREE_LUN, lun
if created then begin
   FILE_DELETE, tdir, /recursive, /noexpand_path
   if FILE_TEST(tdir, /noexpand_path) then ERRORS_ADD, nb_errors, 'fixture directory was not removed'
endif
BANNER_FOR_TESTSUITE, 'TEST_UNIX_READLINK', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
end
;
; /proc/self has a dynamically generated target, unlike an ordinary symlink.
pro TEST_LINUX_READLINK, cumul_errors
nb_errors=0
if ~FILE_TEST('/proc/self', /symlink) then return
pid=FILE_READLINK('/proc/self')
if ~STREGEX(pid, '^[0-9]+$', /boolean) then begin
   ERRORS_ADD, nb_errors, 'non-numeric target for /proc/self'
endif else begin
   if ~FILE_TEST('/proc/'+pid, /directory) then $
      ERRORS_ADD, nb_errors, '/proc/self does not name a process directory'
endelse
BANNER_FOR_TESTSUITE, 'TEST_LINUX_READLINK', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
end
;
pro TEST_FILE_READLINK, help=help, test=test, no_exit=no_exit, verbose=verbose
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FILE_READLINK, help=help, test=test, $'
   print, '                        no_exit=no_exit, verbose=verbose'
   return
endif
;
if STRLOWCASE(!version.os_family) NE 'unix' then begin
   MESSAGE, /continue, 'Symbolic-link fixtures require a Unix system'
   if ~KEYWORD_SET(no_exit) then EXIT, status=77
   return
endif
;
cumul_errors=0
TEST_UNIX_READLINK, cumul_errors, verbose=verbose
if STRLOWCASE(!version.os) EQ 'linux' then TEST_LINUX_READLINK, cumul_errors
BANNER_FOR_TESTSUITE, 'TEST_FILE_READLINK', cumul_errors
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
if KEYWORD_SET(test) then STOP
end
