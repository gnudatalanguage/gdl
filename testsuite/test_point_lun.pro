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

print,'reading 4 times the 1st character of '+file
print,'compress ',compress
openr, lun, file, /get_lun, compress=compress
for i=0,3 do begin 
    point_lun, lun, 0L  ; <<< rewind
    readu,lun,x & print,string(x)
endfor
free_lun, lun

return
end

pro TEST_POINT_LUN

; file creation
file = '/tmp/file.txt'
filegz = file+'.gz'
openw,lun,file,/get_lun
printf,lun,'abcd'
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

