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
pro READ_4B_FILE, file, compress=compress

x = BYTARR(1)

print,'reading 4 times the 1st character of '+file+', compress option set to '+strtrim(compress,2)+'...', format='($,a)'
openr, lun, file, /get_lun, compress=compress
err=0
for i=0,3 do begin 
    point_lun, lun, 0L  ; <<< rewind
    readu,lun,x & if (string(x) ne 'a') then err++
 endfor
if (err) then print,"read error: "+string(x) else print,"OK"
print,"reading 13 elements at position 7...", format='($,a)'
y=bytarr(7)
point_lun, lun, 0L  ; <<< rewind
point_lun, lun, 7L  ; <<< goto 7
readu,lun,y & if (string(y) ne 'hijklmn') then print,"read error:"+string(y) else print,"OK"
print,"position status as per fstat() function:"
lunstat=fstat(lun)
help,lunstat,/struct
if (lunstat.cur_ptr ne 14) then print,"Error: wrong CUR_PTR returned by fstat()."

free_lun, lun

return
end

pro TEST_POINT_LUN

; file creation
file = '/tmp/file.txt'
filegz = file+'.gz'
openw,lun,file,/get_lun
printf,lun,'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'
free_lun,lun

; read raw file
read_4b_file, file, compress=0

; read raw file
read_4b_file, file, compress=1

; compress file
spawn,'gzip -f '+file,/sh

; read compressed file
read_4b_file, filegz, compress=1

return
end

