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
; --------------------------------
;
pro TEST_FILE_COPY, test=test
;
; Create test directory
tdir='tdir_test_f_copy_gdl'
if FILE_TEST(tdir) EQ 0 then SPAWN, 'mkdir '+tdir
;if (FILE_TEST(tdir,/directory) EQ 1 and FILE_TEST(rep) EQ 0) then SPAWN, 'mkdir '+rep

all_files_and_directories=tdir 

;Files
files1=['gdlfd_test1a','gdlfd_test1b']
files2=['gdlfd_test1aCP','gdlfd_test1bCP']

file2='$chops &up str*ings_gdltest.'

all_files_and_directories=[all_files_and_directories,files1]
all_files_and_directories=[all_files_and_directories,files2]
all_files_and_directories=[all_files_and_directories,file2]

more='more text , more more '

f1=[tdir+'/gdlfd_test1a',tdir+'/gdlfd_test1b']

if FILE_TEST(files1[0]) EQ 0 then SPAWN, 'touch '+files1[0]
if FILE_TEST(files1[1]) EQ 0 then SPAWN, 'touch '+files1[1]
if FILE_TEST(file2) EQ 0 then SPAWN, 'touch '+ESCAPE_SPECIAL_CHAR(file2)
;
;
sce=MAKE_ARRAY(N_ELEMENTS(files1))
dest=MAKE_ARRAY(N_ELEMENTS(files1))
;
; Add elements to files
for ii=0, N_ELEMENTS(files1)-1 do begin
   SPAWN , 'echo '+more+' >> '+files1[ii]
endfor
;
print , 'COPY N1'
; Copy files1 into tdir
FILE_COPY, files1 , tdir
;
sce=FILE_INFO(files1)
dest=FILE_INFO(f1) 
;
;print, 'Size sce',  sce[0].size
;print, 'Size dest', dest[0].size
;
; if files1 et f1 have same size , cp ok
;
for ii=0, N_ELEMENTS(sce)-1 do begin
   if(sce[ii].size ne dest[ii].size ) then begin
      MESSAGE, 'error cp', /continue
      DEL_TEST_FILES, all_files_and_directories
      EXIT, status=1
   endif
endfor
;
; add elements to files
for ii=0, N_ELEMENTS(files1)-1 do begin
   SPAWN, 'echo '+more+' >> '+files1[ii]
endfor
;
print , 'COPY N2'
; Copy files1 into tdir again
FILE_COPY, files1 , tdir
;
sce=FILE_INFO(files1)
dest=FILE_INFO(f1) 
;
;print , 'Size sce',  sce[0].size
;print , 'Size dest', dest[0].size
;
; if files1 et f1 don't have same size , because overwrite is not allowed
;
for ii=0, N_ELEMENTS(sce)-1 do begin
   if(sce[ii].size eq dest[ii].size ) then begin
      MESSAGE, 'error cp', /continue
      DEL_TEST_FILES, all_files_and_directories
      EXIT, status=1
   endif
endfor
;
for ii=0, N_ELEMENTS(files1)-1 do begin
   SPAWN, 'echo '+more+' >> '+files1[ii]
endfor
;
; Overwrite -  maj :::: t0 < t1
print , 'OVERWRITE' 
FILE_COPY, files1 , tdir , /overwrite 
dest=FILE_INFO(f1) 
sce=FILE_INFO(files1)
;print, 'Size sce',  sce[0].size
;print, 'Size dest', dest[0].size
;
; if files1 et f1 have same size , cp ok
for ii=0, N_ELEMENTS(sce)-1 do begin
   if(sce[ii].size ne dest[ii].size ) then begin
      MESSAGE, 'error cp', /continue
      DEL_TEST_FILES, all_files_and_directories
      EXIT, status=1
   endif
endfor
;
for ii=0, N_ELEMENTS(files1)-1 do begin
   SPAWN, 'echo '+more+' >> '+files1[ii]
endfor
;
; CP files > files
;
print , 'Require_dir ' 
FILE_COPY, files1 , files2 , /require_dir
; if files2 exist : error
;
for ii=0, N_ELEMENTS(files2)-1 do begin
   if(FILE_TEST(files2[ii]) eq 1 ) then begin
      MESSAGE, 'error require_dir', /continue
      DEL_TEST_FILES, all_files_and_directories                
      EXIT, status=1
   endif
endfor
;
;print , 'Sce=tab of files Dest=file ' 
FILE_COPY, files1 , file2 , /require_dir
;
print , 'files to files' 
FILE_COPY, files1 , files2
;
sce=FILE_INFO(files2)
dest=FILE_INFO(files1) 
;
;print, 'Size sce', sce[0].size
;print, 'Size dest',  dest[0].size
; if files1 et files2 have same size , cp ok
;
for ii=0, N_ELEMENTS(sce)-1 do begin
   if(sce[ii].size ne dest[ii].size ) then begin
      MESSAGE, 'error cp', /continue
      DEL_TEST_FILES, all_files_and_directories
      EXIT, status=1
   endif
endfor
;
;;;;;;;;;;;;;;;;;;;;;
; How to verify with allow_same?
;
; Without allow_same
print , 'WITHOUT ALLOW_SAME' 
FILE_COPY, files1 , files1
;
; With Allow_same
print , 'WITH ALLOW_SAME' 
FILE_COPY, files1 , files1 , /allow_same
;
;;;;;;;;;;;;;;;;;;;;;
;recursive
;
; Test with directories
tdir3=['test_dir1' , '$&dir2_&-spec$'] ; 
tdir2=['td2_1' , 'td2_2'] ; 
td2cp=[tdir+'/td2_1' , tdir+'/td2_2'] ; 
;
all_files_and_directories=[all_files_and_directories,tdir2]
;
if FILE_TEST(tdir2[0], /directory) EQ 0 then SPAWN, 'mkdir '+tdir2[0]
if FILE_TEST(tdir2[1], /directory) EQ 0 then SPAWN, 'mkdir '+tdir2[1]
;
for ii=0,N_ELEMENTS(tdir2)-1 do begin
   for jj=0, 5 do begin
      fname = tdir2[ii]+'/'+STRCOMPRESS('fi' + string(jj), /remove_all)
      SPAWN , 'touch '+fname
   endfor
endfor
;
print , 'WITHOUT RECURSIVE'
FILE_COPY, tdir2 , tdir
;tdir/tdir2 : copy not allowed
;
for ii=0, N_ELEMENTS(td2cp)-1 do begin
   if(FILE_TEST(td2cp[ii]) eq 1 ) then begin
      MESSAGE, 'error whtout rec', /continue
      DEL_TEST_FILES, all_files_and_directories
      EXIT, status=1
   endif
endfor
;
print , 'WITH RECURSIVE'
FILE_COPY, tdir2 , tdir , /recursive
;copy ok
sce=FILE_INFO(tdir2)
dest=FILE_INFO(td2cp) 
;
for ii=0, N_ELEMENTS(td2cp)-1 do begin
   if(FILE_TEST(td2cp[ii]) eq 0 ) then begin
      MESSAGE, 'error recursive', /continue
      DEL_TEST_FILES, all_files_and_directories
      EXIT, status=1
   endif
endfor
;
;delete all
DEL_TEST_FILES, all_files_and_directories
;
print, 'All tests done'
;
if KEYWORD_SET(test) then STOP
;
end

