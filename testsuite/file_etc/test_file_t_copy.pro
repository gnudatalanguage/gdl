;
; Testing FILE_COPY function
;
; Lea Noreskal, June 2010
; under GNU GPL 2 or later
;
;
; -------------------------------------------------------------
; AC 05-Jun-2015 this code is also in copy in "test_file_delete.pro"
; the copy is needed for "make check"
pro DEL_TEST_FILES, to_delete, verbose=verbose
;
; Delete files and directories
;
for ii=0, N_ELEMENTS(to_delete)-1 do begin
   if KEYWORD_SET(verbose) then begin
      print, 'File or Dir. >>'+to_delete[ii]+'<< to be deleted'
      print, 'Interpreted as: >>'+ESCAPE_SPECIAL_CHAR(to_delete[ii])+'<<'
   endif
   if (FILE_TEST(to_delete[ii]) eq 1) then begin
;      SPAWN , 'rm -rf '+ESCAPE_SPECIAL_CHAR(to_delete[ii])

	file_delete,to_delete[ii],/recursive

      ;; we don't check whether the file is deleted or not
   endif else begin
      if KEYWORD_SET(verbose) then begin
         print, 'File or Dir. >>'+to_delete[ii]+'<< not found'
         print, 'Interpreted as: >>'+ESCAPE_SPECIAL_CHAR(to_delete[ii])+'<<'
      endif
   endelse
endfor
;
end
pro create_dummyfile, filename
openw,lun,/get, filename & free_lun,lun
return
end
pro FILE_APPEND,filename, more
openw,lun,/get, filename, /append
printf,lun,more
free_lun,lun
return
end
;
; --------------------------------
;
pro TEST_FILE_T_COPY, test=test, verbose=verbose
;

; Clean up any residue and create test directory
tdir='tdir_test_f_copy_gdl'
file_delete,tdir,/recursive
file_mkdir,tdir

all_files_and_directories=tdir 

;Files
files1=['gdlfd_test1a','gdlfd_test1b']
files2=['gdlfd_test1aCP','gdlfd_test1bCP']

file2='$chops &up str*ings_gdltest.'

all_files_and_directories=[all_files_and_directories,files1]
all_files_and_directories=[all_files_and_directories,files2]
if !version.os_family eq 'unix' then $
   all_files_and_directories=[all_files_and_directories,file2] $
   else $
   message,/continue,' Not unix OS_family and so weird named file2 test not done'

more='more text , more more '

f1=tdir+'/'+files1

if ~FILE_TEST(files1[0]) then CREATE_DUMMYFILE,files1[0]
if ~FILE_TEST(files1[1]) then CREATE_DUMMYFILE,files1[1]
if !version.os_family eq 'unix' then $
   if FILE_TEST(file2) EQ 0 then CREATE_DUMMYFILE,ESCAPE_SPECIAL_CHAR(file2)
;
; Add elements to files
for ii=0, N_ELEMENTS(files1)-1 do file_append, files1[ii], more
;
print , 'COPY N1'
; Copy files1 into tdir
FILE_T_COPY, files1 , tdir, verbose=verbose
;
sce=FILE_INFO(files1)
dest=FILE_INFO(f1) 
;
; if files1 et f1 have same size , cp ok
;

if ~array_equal(sce.size, dest.size) then begin &$
      MESSAGE, 'error cp', /continue &      DEL_TEST_FILES, all_files_and_directories &$
   endif
;
; add elements to files
for ii=0, N_ELEMENTS(files1)-1 do file_append, files1[ii], more
;
print , 'COPY N2'
; Copy files1 into tdir again
FILE_T_COPY, files1 , tdir, verbose=verbose
;
sce=FILE_INFO(files1)
dest=FILE_INFO(f1) 
;
;print , 'Size sce',  sce[0].size
;print , 'Size dest', dest[0].size
;
; if files1 et f1 don't have same size , because overwrite is not allowed
;
if array_equal(sce.size, dest.size) then begin &$
      MESSAGE, 'error cp', /continue &      DEL_TEST_FILES, all_files_and_directories &$
   endif
;
for ii=0, N_ELEMENTS(files1)-1 do file_append, files1[ii], more
;
; Overwrite - 
print , '/OVERWRITE test' 
FILE_T_COPY, files1 , tdir , /overwrite , verbose=verbose
dest=FILE_INFO(f1) 
sce=FILE_INFO(files1)
if ~array_equal(sce.size, dest.size) then begin &$
      MESSAGE, 'error cp', /continue &      DEL_TEST_FILES, all_files_and_directories &$
   endif

;
for ii=0, N_ELEMENTS(files1)-1 do file_append, files1[ii], more
;
; CP files > files
;
print , 'Require_dir (but not a dir)' 

FILE_T_COPY, files1 , files2 , /require_dir, verbose=verbose
; if files2 exist : error
;
;	print,file_test(files2)
if(total(file_test(files2)) ne 0)  then begin &$
      MESSAGE, 'error require_dir', /continue &   DEL_TEST_FILES, all_files_and_directories   &$             
   endif
;
;
print , 'files1 to files2' 
FILE_T_COPY, files1 , files2, verbose=verbose
;
dest=FILE_INFO(files2) 
sce=FILE_INFO(files1)
if ~array_equal(sce.size, dest.size) then begin &$
      MESSAGE, 'error cp', /continue &      DEL_TEST_FILES, all_files_and_directories &$
   endif
;
; if files1 et files2 have same size , cp ok
;
;;;;;;;;;;;;;;;;;;;;;
; How to verify with allow_same?
;
; Without allow_same
print , 'WITHOUT ALLOW_SAME: files1 , files1'
;'
FILE_T_COPY, files1 , files1, verbose=verbose
;
; With Allow_same
print , 'WITH ALLOW_SAME  :files1 , files1'
; 
FILE_T_COPY, files1 , files1 , /allow_same, verbose=verbose
;
;;;;;;;;;;;;;;;;;;;;;
;recursive
;
; Test with directories
 
tdir2=['td2_1' , 'td2_2'] ; 
td2cp = tdir[0]+'/'+tdir2
;
all_files_and_directories=[all_files_and_directories,tdir2]
;
if ~FILE_TEST(tdir2[0], /directory) then file_mkdir,tdir2[0]
if ~FILE_TEST(tdir2[1], /directory) then file_mkdir,tdir2[1]
;
strname = strCOMPRESS('fi' + sindgen(6), /remove_all)
for ii=0,N_ELEMENTS(tdir2)-1 do $
   for jj=0, 5 do $
      CREATE_DUMMYFILE,tdir2[ii]+'/'+strname[jj]
;
print , 'WITHOUT RECURSIVE'
if KEYWORD_SET(test) then STOP
FILE_T_COPY, tdir2 , tdir , verbose=verbose
;
;tdir/tdir2 : copy not allowed
;
if ~array_equal( FILE_TEST(td2cp), 0 ) then begin &$
      MESSAGE, 'error whtout rec', /continue &$
      DEL_TEST_FILES, all_files_and_directories &$
      STOP &$
   endif

;
print , 'WITH RECURSIVE'
FILE_T_COPY, tdir2 , tdir , /recursive, verbose=verbose
;copy ok
;
if ~array_equal(FILE_TEST(td2cp),1) then begin  &$
      MESSAGE, 'error recursive', /continue &$
      DEL_TEST_FILES, all_files_and_directories & stop &$
   endif

;
;delete all
DEL_TEST_FILES, all_files_and_directories
;
print, 'All tests done'
;
if KEYWORD_SET(test) then STOP
;
end

