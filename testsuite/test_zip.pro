;
; Testing the /compress keyword !
;
; Maxime Lenoir, June 2010
; under GNU GPL 2 or later
; 
pro TEST_ZIP

file='test_zip.gz'
spawn,'echo -e "GDL - GNU Data Language\nA free IDL (Interactive Data Language) compatible incremental\ncompiler (i. e. runs IDL programs)." | gzip > '+file

openr,fd,file,/get,/compress,/delete

;; Size test (FSTAT)
if strlowcase(!version.os_name) eq 'linux' then spawn,'stat --printf=%s '+file,s else spawn,'stat -f%z '+file,s
if (fstat(fd)).size ne s then begin
	message,/continue,'Bad size returned for zipped file'
	exit,status=1
endif

;; READ TESTS

; Readf test
str=''
readf,fd,str
spawn,'cat '+file+' | gunzip - | head -1',head1
if str ne head1 then begin
	message,/continue,'Readf doesn''t work on zipped file'
	exit,status=1
endif

readf,fd,str
spawn,'cat '+file+' | gunzip - | head -2 | tail -1',head2
if str ne head2 then begin
	message,/continue,'Readf doesn''t work twice on zipped file'
	exit,status=1
endif

point_lun,fd,0
readf,fd,str ; At this point, readf works well, so we can test point_lun
if str ne head1 then begin
	message,/continue,'Point_lun doesn''t work on zipped file (read mode)'
	exit,status=1
endif

; Assoc test
point_lun,fd,0
a=assoc(fd,bytarr(5))
spawn,'cat '+file+' | gunzip - | head -1 | cut -b 1-5',cut15
if string(a[0]) ne cut15 then begin
	message,/continue,'Assoc doesn''t work on zipped file'
	exit,status=1
endif

point_lun,fd,5

if string(a[0]) ne cut15 then begin
	message,/continue,'Assoc doesn''t work twice on zipped file'
	exit,status=1
endif

spawn,'cat '+file+' | gunzip - | head -1 | cut -b 6-10',cut15
if string(a[1]) ne ' GNU ' then begin
	message,/continue,'Assoc doesn''t work with differents indexes on zipped file'
	exit,status=1
endif

; Readu test
point_lun,fd,0
str='123'
readu,fd,str
if str ne 'GDL' then begin
	message,/continue,'Readu doesn''t work on zipped file'
	exit,status=1
endif

readu,fd,str
if str ne ' - ' then begin
	message,/continue,'Readu doesn''t work twice on zipped file'
	exit,status=1
endif

close,fd
free_lun,fd

;; WRITE TESTS

file='test_zipw.gz'
openw,fd,file,/get,/compress
printf,fd,'GNU Data Language'
close,fd
free_lun,fd

openr,fd,file,/get,/compress
readf,fd,str
if str ne 'GNU Data Language' then begin
	message,/continue,'Printf doesn''t work on zipped file'
	exit,status=1
endif
close,fd
free_lun,fd

openw,fd,file,/get,/compress
printf,fd,'GNU Data Language'
point_lun,fd,10
; point_lun used in write mode is pretty useless cause you can only move forward and it cause premature EOF.
; Also, you can't open a file in write mode, with compress and append keywords.
; So, if you don't specify append, you truncate the file, and point_lun become useless.
; To summarize, you can't write data and go backward to overwrite it. (Just don't use it)

printf,fd,'rocks' ; GNU Data Language\nrocks\n
close,fd
free_lun,fd

openr,fd,file,/get,/compress,/delete
a=assoc(fd,bytarr(10))

if string(a[0]) ne 'GNU Data L' or strmid(string(a[1]),0,7) ne 'anguage' then begin
	message,/continue,'Point_lun doesn''t work on zipped file (write mode)'
	exit,status=1
endif

close,fd
free_lun,fd

end
