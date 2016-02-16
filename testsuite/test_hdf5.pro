;
; Alain C., 16 Fev. 2016
; Very preliminary tests for HDF5 formats.
; Extensions (data (small volume) + code) very welcome
; (never forget than adding now test cases will avoid future silently
; broken functionnalities !!)
;
; Some users are already using the HDF5 functionnalities
; but it was reported that the STRING field is not well managed.
;
function FILE_IN_TESTSUITE, filename, add_currrent_dir=add_currrent_dir
;
list_of_dirs=STRSPLIT(!PATH, PATH_SEP(/SEARCH_PATH), /EXTRACT)
;
; adding the current directory if needed
;
if KEYWORD_SET(add_currrent_dir) then begin
    CD, current=current
    index=STRPOS(list_of_dirs, current)
    already=WHERE(index EQ 0, nb_already)
    ;; adding at the end to avoid conflict with other choices
    if nb_already EQ 0 then begin
        !PATH=!PATH+PATH_SEP(/SEARCH_PATH)+current
        list_of_dirs=STRSPLIT(!PATH, PATH_SEP(/SEARCH_PATH), /EXTRACT)
    endif
endif
;
fullfile=FILE_SEARCH(list_of_dirs+PATH_SEP()+filename)
;
if (N_ELEMENTS(fullfile) GT 1) then fullfile=fullfile[0]
;
if ~FILE_TEST(fullfile) then begin
    MESSAGE, 'file <<'+file+'>> not found in the !PATH', /continue
;    if KEYWORD_SET(no_exit) OR KEYWORD_SET(test) then STOP
    EXIT, status=1
endif
;
return, fullfile
;
end
;
; -----------------------------------------------
;
pro TEST_HDF5_BYTE_ARRAY, cumul_errors, test=test
;
errors=0
;
file='byte_array.h5'
fullfile=FILE_IN_TESTSUITE(file, /add_cur)
;
file_id = H5F_OPEN(fullfile)
data_id = H5D_OPEN(file_id, 'g1/d1')
;
vector=0
ok=EXECUTE('vector = H5D_READ(data_id)')
;
expected=BYTE([127,1, 23])
;
; we have to test the values but also the TYPE
if ~ARRAY_EQUAL(vector, expected) then begin
    MESSAGE, /continue, 'Bad values ...'
    errors++
endif
if (TYPENAME(vector) NE 'BYTE') then begin
    MESSAGE, /continue, 'Bad TYPE ...'
    errors++
endif
;
BANNER_FOR_TESTSUITE, 'TEST_HDF5_BYTE_ARRAY', errors, /short
;
if KEYWORD_SET(test) then STOP
;
if ~ISA(cumul_errors) then cumul_errors=0
cumul_errors=cumul_errors+errors
;
end
;
; -----------------------------------------------
;
pro TEST_HDF5_STRING, cumul_errors, test=test
;
; Test provided by R. Parker (Univ. Leicester)
;
errors=0
;
file='string.h5'
fullfile=FILE_IN_TESTSUITE(file, /add_cur)
;
file_id = H5F_OPEN(fullfile)
data_id = H5D_OPEN(file_id, 'mystring')
;
mystring=''
ok=EXECUTE('mystring=H5D_READ(data_id)')
;
expected='Peter is great.'
;
if ~ARRAY_EQUAL(mystring, expected) then errors=1
;
BANNER_FOR_TESTSUITE, 'TEST_HDF5_STRING', errors, /short
;
if KEYWORD_SET(test) then STOP
;
if ~ISA(cumul_errors) then cumul_errors=0
cumul_errors=cumul_errors+errors
;
end
;
; -----------------------------------------------
;
pro TEST_HDF5, help=help, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_HDF5, help=help, test=test, no_exit=no_exit'
   return
endif
;
; Do we have access to HDF5 functionnalities ??
;
DEFSYSV, '!gdl', exists=is_it_gdl
;
if (is_it_gdl EQ 1) then begin
    ok=EXECUTE('resu=HDF5_EXISTS()')
    if (resu EQ 0) then begin
        MESSAGE, /continue, "GDL was compiled without HDF5 support."
        MESSAGE, /con, "You must have HDF5 support to use this functionaly."
        EXIT, status=77
    endif
endif
;
cumul_errors=0
;
TEST_HDF5_BYTE_ARRAY, cumul_errors
;
TEST_HDF5_STRING, cumul_errors
;
BANNER_FOR_TESTSUITE, 'TEST_HDF5', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

