;
; Lea Noreskal, June 2010
; under GNU GPL 2 or later
;
; initial version by AC, 28 Avril 2009
; very basic tests for FILE_DELETE
;
pro TEST_FILE_DELETE, full_test=full_test, test=test

; Files
files1=['fd_test1a','fd_test1b', '$chops &up str*ings.' ]
file2='fd_test2'
files3=['/etc/passwd', '/etc/passwd1']
specfile='$f_tst*.mq' ; filename with special characters

;
; creation des fichiers temporaires de test
;
if FILE_TEST(files1[0]) EQ 0 then SPAWN, 'touch '+files1[0]
if FILE_TEST(files1[1]) EQ 0 then SPAWN, 'touch '+files1[1]
if FILE_TEST(files1[2]) EQ 0 then SPAWN, 'touch '+escape_special_char(files1[2])
if FILE_TEST(file2) EQ 0 then SPAWN, 'touch '+file2
;

all_files_and_directories=files1 
all_files_and_directories=[all_files_and_directories,file2]

FILE_DELETE, files1, file2, /quiet

for ii=0,N_ELEMENTS(files1)-1 do begin
   if (FILE_TEST(files1[ii]) EQ 1 ) then begin
      MESSAGE, 'files1 has not been deleted', /continue
      del_test_files , all_files_and_directories
      EXIT, status=1
   endif
endfor

if (FILE_TEST(file2) EQ 1) then begin
   MESSAGE, 'file2 has not been deleted', /continue
   del_test_files , all_files_and_directories
   EXIT, status=1
endif

; test with no existing file
FILE_DELETE, specfile
FILE_DELETE, specfile , /quiet
FILE_DELETE, specfile , /allow_nonexistent
FILE_DELETE, specfile , /quiet , /verbose

; Test with directories 
tdir=['test_dir1' , '$&dir2_&-spec$'] ; empty directories 
tdir2=['td2_1' , 'td2_2'] ; Not empty directories


print , 'Empty directories'
if FILE_TEST(tdir[0], /directory) EQ 0 then SPAWN, 'mkdir '+tdir[0]
if FILE_TEST(escape_special_char(tdir[1]), /directory) EQ 0 then SPAWN, 'mkdir '+escape_special_char(tdir[1])

all_files_and_directories=[all_files_and_directories,tdir]

FILE_DELETE, tdir 

for ii=0,N_ELEMENTS(tdir)-1 do begin
   if (FILE_TEST(tdir[ii]) EQ 1 ) then begin
      MESSAGE, 'Empty directory has not been deleted', /continue
      del_test_files , all_files_and_directories
      ;FILE_DELETE, tdir , /recursive , /quiet
     ; EXIT, status=1
   endif
endfor


print , 'Not empty directories'
if FILE_TEST(tdir2[0], /directory) EQ 0 then SPAWN, 'mkdir '+tdir2[0]
if FILE_TEST(tdir2[1], /directory) EQ 0 then SPAWN, 'mkdir '+tdir2[1]
all_files_and_directories=[all_files_and_directories,tdir2]

for ii=0,N_ELEMENTS(tdir2)-1 do begin
   for jj=0, 5 do begin
      fname = tdir2[ii]+'/'+STRCOMPRESS('fi' + string(jj), /remove_all)
      ;fname = tdir2[ii]+'/fi01'
      SPAWN , 'touch '+fname
   endfor
endfor


FILE_DELETE, tdir2[1] 

if (FILE_TEST(tdir2[1]) EQ 0) then begin
   MESSAGE, 'Not empty directory have been deleted..', /continue
   del_test_files , all_files_and_directories
   EXIT, status=1
endif


FILE_DELETE, tdir2 , /recursive

if (FILE_TEST(tdir2[0]) EQ 1) then begin
   MESSAGE, 'Directory has not been deleted with key word recursive', /continue
   del_test_files , all_files_and_directories
   EXIT, status=1
endif

;
if KEYWORD_SET(full_test) then begin
   print, 'Files we cannot remove'
   FILE_DELETE, files3, /quiet
   FILE_DELETE, files3, /allow
   FILE_DELETE, files3, /verbose
end
;
; Noexpand_path test
; titi titit 


file_exp=['titi', 'tititt' , 'ti*']

for ii=0,N_ELEMENTS(file_exp)-1 do begin
   if FILE_TEST(file_exp[ii],/noexpand) EQ 0 then SPAWN, 'touch '+escape_special_char(file_exp[ii])
endfor



SPAWN , 'ls'

all_files_and_directories=[all_files_and_directories,file_exp]

;file_to_supp=FILE_INFO('ti*' , /noexpand)
FILE_DELETE , file_exp[2] , /noexpand

if (FILE_TEST(file_exp[2], /noexpand) EQ 1) then begin
   MESSAGE, 'file_exp[2] has not been deleted', /continue
   del_test_files , all_files_and_directories
   EXIT, status=1
endif

file_to_supp=FILE_INFO('ti*')
FILE_DELETE , 'ti*'
if (FILE_TEST(file_to_supp.name) EQ 1) then begin
   MESSAGE, 'file has not been deleted , error expand', /continue
   del_test_files , all_files_and_directories
   EXIT, status=1
endif
;
file_to_supp=FILE_INFO('ti*')
FILE_DELETE , 'ti*'
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

