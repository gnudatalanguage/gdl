pro test_issues1474
if !version.os_family eq 'Windows' then return
close,1, 2
openw,1,'basefile'
line1='01234567890123456789'
for i=0,10 do writeu,1,line1
flush,1 ; very necessary as IDL's i/o are buffered

openr,2,'basefile'
line2=bytarr(20)
for i=0,10 do readu,2,line2
print,"before adding several bytes at end:"
f1=fstat(1) & print, 'unit 1 (write) :', f1.unit, f1.cur_ptr, f1.size
before_w=f1.cur_ptr
f2=fstat(2) & print, 'unit 2 (read)  :', f2.unit, f2.cur_ptr, f2.size
before_r=f2.cur_ptr

; append to file separately:
openu,3,'basefile'
point_lun,3,before_r
writeu,3,"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ"
flush,3
close,3

print,"after adding several bytes at end:"
f1=fstat(1) & print, 'unit 1 (write) :', f1.unit, f1.cur_ptr, f1.size
after_w=f1.cur_ptr
f2=fstat(2) & print, 'unit 2 (read)  :', f2.unit, f2.cur_ptr, f2.size
after_r=f2.cur_ptr
if (before_r ne after_r and before_w ne after_w ) then exit,status=1

print,"writing something at current offset "+strtrim(after_w)
line3=bytarr(12)+65b ; a serie of 'A'
writeu,1,line3
flush,1                         ; necessary...
print,"reading at current offset "+strtrim(after_r)
a=0b
readu,2,a
if (a ne 65) then message,/info,"Known problem with sync() in defaut_io.cpp, FIXME"
end
