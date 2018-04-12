; test_help: a prototype everything goes collection of compiled routines
; and common blocks, and help calls.  If os_family ne "Windows" then
; we can invoke another set of test routines via spawn and run: that hasn't worked for windows yet.
pro holdacommon, a,b,outhelp=outhelp
common acommon,  acom_a,acom_b
help,acom_a,acom_b,out=outstrb
acom_a=a
acom_b=b
help,acom_a,acom_b,out=outstre
if (n_elements(outstrb) ne 0) then outhelp=[outstrb," ... then (holda) ...",outstre]
return
end
pro holdbcommon, a,b,outhelp=outhelp
common bcommon,  bcom_a,bcom_b
help,bcom_a,bcom_b,out=outstrb
bcom_a=a
bcom_b=b
help,bcom_a,bcom_b,out=outstre
if (n_elements(outstrb) ne 0) then outhelp=[outstrb," ... then (holdb) ...",outstre]
return
end
;
; rudimentary beginnings of a test program
;
pro test_help
holdacommon, findgen(2,3), fltarr(4), outhelp=stra
holdbcommon, findgen(4,5), fltarr(6), outhelp=strb
na = n_elements(stra)
nb = n_elements(strb)
if (na ne nb) or (na eq 0) then exit, status=1
for k=0,na-1 do print,stra[k]
;
;	BANNER_FOR_TESTSUITE, 0
help,/fun,/lib,name='str*',output=strfun
nf=n_elements(strfun)
print,' There are ',nf,' library functions that begin with "STR"'
end


