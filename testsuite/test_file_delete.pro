; Sebastien BOUCHET, January 2026

pro DEL_TEST_FILES, to_delete, verbose=verbose
  
; Delete files and directories
  
if N_ELEMENTS(to_delete) EQ 0 then return

; for ii = 0, N_ELEMENTS(to_delete)-1 do begin

foreach path, to_delete do begin
   if FILE_TEST(path) NE 0 then begin
      if KEYWORD_SET(verbose) then print, 'Deleting: ', path
      FILE_DELETE, path, /recursive, /verbose
   endif else begin
      if KEYWORD_SET(verbose) then print, 'Not found: ', path
   endelse
endforeach
   ;
end

; --------------------------------

pro TEST_FILE_DELETE, cumul_errors, test=test, help=help, verbose=verbose

errors = 0

; ----------
; File tests
; ----------

files_simple = ['fd_test1a', 'fd_test1b', 'fd_test2']

; NOTE: IDL truncates filenames containing '$' in verbose output
file_special = '$chops &up st.rings'
allfiles = [files_simple, file_special]

; Creation of test files

foreach f, allfiles do begin
   if FILE_TEST(f) EQ 0 then begin
      openw, lun, f, /get
      free_lun, lun
   endif
endforeach

; Verify creation

err = where(FILE_TEST(allfiles) EQ 0, nerr)
if nerr GT 0 then begin
   ERRORS_ADD, errors, 'Setup failure, following files were not created: ' + STRJOIN(allfiles[err], ', ')
endif 

;
; Try Deletion, then Verify Deletion

FILE_DELETE, allfiles, verbose=verbose

foreach f, allfiles do begin
   if FILE_TEST(f) NE 0 then begin
      ERRORS_ADD, errors, 'File not deleted: ' + f
   endif
endforeach


; ---------------
; Directory Tests
; ---------------

tdir_empty = ['test_dir1', '$&dir2_&-spec$'] ; what abt a * or space
tdir_nonempty = ['td2_1', 'td2_2']

;
; --- Create and Delete empty directories

foreach d, tdir_empty do begin
   if FILE_TEST(d) EQ 0 then file_mkdir, d
endforeach

FILE_DELETE, tdir_empty, verbose=verbose

; --- Verify deletion

err = where(FILE_TEST(tdir_empty) NE 0, nerr)
if nerr GT 0 then begin
   ERRORS_ADD, errors, 'Empty directories not deleted: ' + STRJOIN(tdir_empty[err], ', ')
endif

;
; --- Create non-empty directories

foreach d, tdir_nonempty do begin
   if FILE_TEST(d) EQ 0 then file_mkdir, d
   foreach i, INDGEN(6) do begin
      openw, lun, d + '/file' + STRING(i, FORMAT='(I0)'), /get
      free_lun, lun
   endforeach
endforeach
      
; --- Attempt delete WITHOUT /recursive (should fail = not delete)

res = EXECUTE("FILE_DELETE, tdir_nonempty[0], verbose=verbose")
if KEYWORD_SET(verbose) then PRINT, 'EXECUTE result = ', res

if res NE 0 then begin
   ERRORS_ADD, errors, 'Non-empty directory did NOT error on non-/RECURSIVE delete'
endif 

; --- Verify directory still exists (should be the case)

if FILE_TEST(tdir_nonempty[0]) EQ 0 then begin
   ERRORS_ADD, errors, 'ERROR: Non-empty directory deleted without /RECURSIVE'
endif else if KEYWORD_SET(verbose) then begin
   print, 'OK: error successfully caught' ; also TO DELETE if verified
endif

;
; --- Delete and verify deletion WITH /recursive

FILE_DELETE, tdir_nonempty, verbose=verbose, /recursive

err = where(FILE_TEST(tdir_nonempty) NE 0, nerr)
if nerr GT 0 then begin
   ERRORS_ADD, errors, 'Recursive delete failed for: ' + STRJOIN(tdir_nonempty[err], ', ')
endif

;
; ---------------
; /NOEXPAND tests
; ---------------

; File config and creation

files_exp = ['exp_1.txt', 'exp_2.txt', 'exp_ABC.txt']
file_literal = 'exp_tmp.txt'

foreach f, [files_exp, file_literal] do begin
   if FILE_TEST(f) EQ 0 then begin
      openw, lun, f, /get
      free_lun, lun
   endif
endforeach

; Rename tmp into wildcard file, making sure it doesn't exist before

FILE_MOVE, 'exp_tmp.txt', 'exp_*.txt', /noexpand_path

; Sanity check

err = where(FILE_TEST(files_exp, /noexpand_path) EQ 0, nerr)
if nerr GT 0 then begin
   ERRORS_ADD, errors, 'Setup failure in noexpand test: ' + STRJOIN(files_exp[err], ', ')
endif

;
; Test WITH /NOEXPAND_PATH (literal deletion only)

FILE_DELETE, 'exp_*.txt', /noexpand_path, verbose=verbose

; Literal file must be gone
if FILE_TEST('exp_*.txt', /noexpand_path) NE 0 then begin
   ERRORS_ADD, errors, 'ERROR: literal wildcard file not deleted with /NOEXPAND_PATH'
endif

; Test of /noexpand success (other files must still exist)
if MIN(FILE_TEST(['exp_1.txt', 'exp_2.txt', 'exp_ABC.txt'])) EQ 0 then begin
   ERRORS_ADD, errors, 'ERROR: /NOEXPAND_PATH deleted expanded files'
endif

;
; Test WITHOUT /noexpand (wildcard expansion deletion)

FILE_DELETE, 'exp_*.txt', verbose=verbose ; switch to quiet if good

remaining = TOTAL(FILE_TEST(files_exp, /noexpand_path) EQ 1)

if remaining EQ N_ELEMENTS(files_exp) then begin
   ERRORS_ADD, errors, 'ERROR: Wildcard deletion deleted no files'
endif else if remaining NE (N_ELEMENTS(files_exp) - 1) then begin
   ERRORS_ADD, errors, 'ERROR: Wildcard deletion deleted wrong number of files'
endif


; ----------------------
; ALLOW_NONEXISTENT test
; ----------------------

; Clean-up of remaining test files, mixed with already deleted files
if KEYWORD_SET(verbose) then PRINT, 'Deleting all remaining files: '
FILE_DELETE, files_exp, verbose=verbose, /noexpand, /allow_nonexistent

err = where(FILE_TEST(files_exp, /noexpand_path) NE 0, nerr)
if nerr GT 0 then begin
   ERRORS_ADD, errors, 'ERROR: /allow_nonexistent test failed, not all files are deleted: ' + STRJOIN(files_exp[err], ', ')
endif


; ------------------------
; Wildcard with no matches
; ------------------------

no_match = 'file_does_not_exist_*'

; Make sure there is no matching file
if FILE_TEST(no_match) NE 0 then begin
   ERRORS_ADD, errors, 'Setup failure: wildcard unexpectedly matches existing files'
endif

; --- Test with /ALLOW_NONEXISTENT (should error)
res = EXECUTE("FILE_DELETE, no_match, verbose=verbose, /allow_nonexistent")
if KEYWORD_SET(verbose) then PRINT, 'EXECUTE result = ', res

if res NE 0 then begin
   ERRORS_ADD, errors, 'Wildcard without match did NOT error despite /ALLOW_NONEXISTENT'
endif 


; --- Test without QUIET (should error)

res = EXECUTE("FILE_DELETE, no_match, verbose=verbose")
if KEYWORD_SET(verbose) then PRINT, 'EXECUTE result = ', res
if res NE 0 then begin
   ERRORS_ADD, errors, 'Wildcard without match did NOT error without /QUIET'
endif

; --- Test with QUIET (shouldn't error)

res = EXECUTE("FILE_DELETE, no_match, /quiet")
if KEYWORD_SET(verbose) then PRINT, 'EXECUTE result = ', res
if res EQ 0 then begin
   ERRORS_ADD, errors, '/QUIET did not suppress wildcard error'
endif else if KEYWORD_SET(verbose) then begin
   PRINT, 'OK: /QUIET suppressed wildcard error (as expected)'
endif


; -----
; Final
; -----

BANNER_FOR_TESTSUITE, 'TEST_FILE_DELETE', errors ;, /short

if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1

if KEYWORD_SET(test) then STOP

end
