; test_help: a prototype everything goes collection of compiled routines
; and common blocks, and help calls.  If os_family ne "Windows" then
; we can invoke another set of test routines via spawn and run: that hasn't worked for windows yet.
pro holdacommon, a,b,outhelp=outhelp, test=test
common acommon,  acom_a,acom_b
help,acom_a,acom_b,out=outstrb
acom_a=a
acom_b=b
help,acom_a,acom_b,out=outstre
if keyword_set(test) then stop,' test KW: holdacommon'
if (n_elements(outstrb) ne 0) then outhelp=[outstrb," ... then (holda) ...",outstre]
return
end
pro holdbcommon, a,b,outhelp=outhelp, test=test
common bcommon,  bcom_a,bcom_b
help,bcom_a,bcom_b,out=outstrb
bcom_a=a
bcom_b=b
help,bcom_a,bcom_b,out=outstre
if keyword_set(test) then stop,' test KW: holdbcommon'
if (n_elements(outstrb) ne 0) then outhelp=[outstrb," ... then (holdb) ...",outstre]
return
end
;
; rudimentary beginnings of a test program
;
pro test_help, test=test, noexit=noexit, verbose=verbose
holdacommon, findgen(2,3), fltarr(4), outhelp=stra, test=test
holdbcommon, findgen(4,5), fltarr(6), outhelp=strb, test=test
na = n_elements(stra)
nb = n_elements(strb)
err=0
if keyword_set(test) then stop,' test KW: $MAIN check na, nb'
if (na ne nb) or (na eq 0) then begin & err++ &$
		if ~keyword_set(noexit) then exit, status=1 &$
		stop,' na, nb check fails: ',na, nb &$
	endif
if keyword_set(verbose) then for k=0,na-1 do print,stra[k]
;
;	BANNER_FOR_TESTSUITE, 0
help,/fun,/lib,name='str*',output=strfun
nf=n_elements(strfun)
print,' There are ',nf-3,' library functions that begin with "STR"'
common acommon,  acom_a,acom_b
common bcommon,  bcom_a,bcom_b
help,names='*com_*',out=comstr & ncs=n_elements(comstr)
;
; if output lines are modified, the #14 will also need to be
if ncs ne 15 then begin & err++ &$
		stop,' ncs check fails: ',ncs  &$
		if ~keyword_set(noexit) then exit, status=1  &$
		endif
		
if keyword_set(verbose) then for k=0,ncs-1 do print,comstr[k]
if err eq 0 then $
	banner_for_testsuite,' test_help',' new features! no errors!' $
	else $
	banner_for_testsuite,' test_help',err
	
if keyword_set(test) then stop,' test_help complete'
	
end


