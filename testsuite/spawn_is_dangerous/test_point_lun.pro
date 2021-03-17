;
; Eriv Hivon, April 2010
; under GNU GPL 2 or later
;
; Related to contribution of Maxime Lenoir in June 2010,
; see also "test_zip.pro" in testsuite/
;
; Simple tests on POINT_LUN
;
; Works in IDL and in GDL (CVS version since 29 june 2010)
;
; Surprisingly, as noted in bug report 712
; https://sourceforge.net/p/gnudatalanguage/bugs/712/
; this code is not working for POINT_LUN
; and this code is not in the Makefile.am :(
;
; Modifications history :
;
;* AC 2017-10-01 : 
; - add this file in the list of tests (Makefile.am)
; - small change to keep the files (compress or not),
;   more easy to manage if problems
; - formated as a true test case !
;
pro READ_4B_FILE, file, errors, compress=compress, test=test
;
err=0
x = BYTARR(1)
;
mess='reading 4 times the 1st character of '+file
mess=mess+' compress option set to '+strtrim(compress,2)+'...'
;
print, mess, format='($,a)'
OPENR, lun, file, /get_lun, compress=compress
;
for i=0,3 do begin 
   POINT_LUN, lun, 0L           ; <<< rewind
   READU, lun, x 
   if (STRING(x) NE 'a') then err++
endfor
;
if (err) then print, "read error: "+STRING(x) else print, "OK"
;
print,"reading 13 elements at position 7...", format='($,a)'
y=BYTARR(7)
POINT_LUN, lun, 0L  ; <<< rewind
POINT_LUN, lun, 7L  ; <<< goto 7
READU,lun,y 
;
if (STRING(y) ne 'hijklmn') then begin
   err++
   print,"read error:"+STRING(y)
endif else print,"OK (POINT_LUN 0 then 7)"
;
print,"position status as per fstat() function:"
lunstat=FSTAT(lun)
;help,lunstat,/struct
if (lunstat.cur_ptr ne 14) then begin
   err++
   print, lunstat.cur_ptr
   print,"Error: wrong CUR_PTR returned by fstat()."
endif else begin
   print,"OK: good CUR_PTR returned by fstat()."
endelse
;
FREE_LUN, lun
;
ERRORS_CUMUL, errors, err
BANNER_FOR_TESTSUITE, "READ_4B_FILE", errors, /SHORT
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------------------
;
pro TEST_POINT_LUN, test=test, no_exit=no_exit, verbose=verbose, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_POINT_LUN, no_exit=no_exit, help=help, test=test, verbose=verbose, quiet=quiet'
   return
end
;
errors=0
;
tmpdir=GETENV('IDL_TMPDIR')
if STRLEN(tmpdir) GT 0 then begin
   tmpdir=tmpdir+PATH_SEP()
   MESSAGE, /continue, 'working in IDL_TMPDIR'
endif else begin
   MESSAGE, /continue, 'IDL_TMPDIR undefined, working in current directory'
endelse
;
; file creation
file1=tmpdir+'file1.txt'
file2=tmpdir+'file2.txt'
file2gz=tmpdir+'file2.txt.gz'
;
data='abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'
;
OPENW, lun, file1, /get_lun
PRINTF, lun, data
FREE_LUN, lun
;
; recopy "file1" into "file2"
FILE_COPY, file1, file2
; compress "file2"
; maybe not OK on MSwin
SPAWN, 'gzip -f '+file2, /sh
;
; read raw file
READ_4B_FILE, file1, errors, compress=0
;
; read raw file as a compressed one
; This is working for IDL, GDL & FL
;
READ_4B_FILE, file1, compress=1
;
; read compressed file
READ_4B_FILE, file2gz, errors, compress=1
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_POINT_LUN', errors
;
if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
