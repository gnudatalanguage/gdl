;
; Greg Jung October 2017
;	 make compatible with windows, use openw and file_mkdir.
;
; Lea Noreskal, June 2010
; under GNU GPL 2 or later
;
; initial version by AC, 28 Avril 2009
; very basic tests for FILE_DELETE
;
; -------------------------------------------------------------
; AC 05-Jun-2015 this code is also in copy in "test_file_delete.pro"
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
      SPAWN , 'rm -rf '+ESCAPE_SPECIAL_CHAR(to_delete[ii])
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
;
; -------------------------------------------------------------
;
pro TEST_FILE_DELETE, full_test=full_test, test=test, help=help
;
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FILE_DELETE, full_test=full_test, test=test, help=help'
   return
endif
;
;
; Files Names
files1=['fd_test1a','fd_test1b']
files1_sc = '$chops &up str*ings.' 
file2='fd_test2'
;
; creation des fichiers temporaires de test
;
if(!version.OS_FAMILY ne "Windows") then $
	if FILE_TEST(files1_sc) EQ 0 then SPAWN, 'touch '+ESCAPE_SPECIAL_CHAR(files1_sc)
;
allfiles = [files1[0:1], file2] 

for k=0,2 do begin & openw,lu,allfiles[k],/get & free_lun, lu & endfor

j = where( FILE_TEST(allfiles) EQ 0 , count_ne)
if(count_ne ne 0) then begin &$
     message,' one of the easy files was not created! returning ' & return &$
     endif
;
FILE_T_DELETE, files1, file2, files1_sc, /quiet
;

for ii=0,N_ELEMENTS(allfiles)-1 do begin
   if (FILE_TEST(allfiles[ii]) EQ 1 ) then begin
      MESSAGE, 'allfiles >>'+allfiles[ii]+'<<has not been deleted', /continue

;      EXIT, status=1
   endif
endfor
;
FILE_T_DELETE,allfiles,/quiet
;
; Test with no existing file
;
specfile='$f_tst*.mq' ; filename with special characters
FILE_T_DELETE, specfile
FILE_T_DELETE, specfile , /quiet
FILE_T_DELETE, specfile , /allow_nonexistent
FILE_T_DELETE, specfile , /quiet , /verbose
;
; Test with directories 
;
tdir = ['test_dir1' , '$&dir2_&-spec$'] ; empty directories 
tdir2= ['td2_1' , 'td2_2'] ; Not empty directories
;
print , 'Empty directories'
if FILE_TEST(tdir[0]) EQ 0 then file_mkdir,tdir[0]
; invalid for windows
if(!version.OS_FAMILY ne "Windows") then $
		if FILE_TEST(ESCAPE_SPECIAL_CHAR(tdir[1]), /directory) EQ 0 then $
		SPAWN, 'mkdir '+ESCAPE_SPECIAL_CHAR(tdir[1])
;
all_files_and_directories=[allfiles,tdir]

FILE_T_DELETE, tdir 
j = where(file_test(tdir) eq 1, count_ed)
if(count_ed ne 0) then begin &$
      MESSAGE, 'Empty directory has not been deleted', /continue &$
   endif

print , 'Not empty directories'
for k=0,1 do begin & if file_test(tdir2[k]) eq 0 then file_mkdir,tdir2[k] & endfor

all_files_and_directories=[allfiles,tdir2]
fnames = '/file'+strcompress(sindgen(6),/remove_all)
for ii=0,N_ELEMENTS(tdir2)-1 do begin &$
   for jj=0, 5 do begin & openw,lu,tdir2[ii]+fnames[jj],/get & free_lun, lu & endfor &$
  endfor

message,$
 ' attempting to delete non-empty directories (without /RECURSIVE)',/continue

; /RECURSIVE keyword needed to remove non-empty directory
FILE_T_DELETE, tdir2[1] 
if (FILE_TEST(tdir2[1]) EQ 0) then begin
   MESSAGE, 'Not empty directory have been deleted..', /continue
   EXIT, status=1
endif


FILE_T_DELETE, tdir2 , /recursive

j = where( file_test(tdir2), count_dirs)
if( count_dirs ne 0) then begin 
   MESSAGE, 'Directories have not been deleted with key word recursive', /continue
   EXIT, status=1
endif

if(!version.OS_FAMILY eq "Windows") then begin &$
	print, 'Windows-restricted tests done (SUCCESS)'  &$
	return & endif

;
; Noexpand_path test
; titi titit 


file_exp=['titi', 'tititt' , 'ti*']

for ii=0,N_ELEMENTS(file_exp)-1 do begin
   if FILE_TEST(file_exp[ii],/noexpand) EQ 0 then SPAWN, 'touch '+escape_special_char(file_exp[ii])
endfor



;	SPAWN , 'ls'

all_files_and_directories=[all_files_and_directories,file_exp]

;file_to_supp=FILE_INFO('ti*' , /noexpand)
FILE_T_DELETE , file_exp[2] , /noexpand

if (FILE_TEST(file_exp[2], /noexpand) EQ 1) then begin
   MESSAGE, 'file_exp[2] has not been deleted', /continue
   del_test_files , all_files_and_directories
   EXIT, status=1
endif

file_to_supp=FILE_INFO('ti*')
FILE_T_DELETE , 'ti*'
if (FILE_TEST(file_to_supp.name) EQ 1) then begin
   MESSAGE, 'file has not been deleted , error expand', /continue
   del_test_files , all_files_and_directories
   EXIT, status=1
endif
;
file_to_supp=FILE_INFO('ti*')
FILE_T_DELETE , 'ti*'
if (FILE_TEST(file_to_supp.name) EQ 1) then begin
   MESSAGE, 'file has not been deleted , error expand', /continue
   del_test_files , all_files_and_directories
   EXIT, status=1
endif


print, 'All tests done'
;
if KEYWORD_SET(test) then STOP
;
end

