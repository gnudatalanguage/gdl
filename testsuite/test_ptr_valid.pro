pro test_ptr_valid, test=test,verbose=verbose, noexit=noexit
;
;	First, look for pre-existing pointers.
; if there are any, maybe the user doesn't want to run this.
p = ptr_valid()
if size(p,/type) ne 10 then begin
	message,' ptr_valid() did not return even a pointer type '
	exit, status=1
	endif
if(n_elements(p) ne 1) then $
	message,/con,' ptr_valid() indicates multiple pre-existing pointers'

if ptr_valid(p[0]) then $
	message,/con,' ptr_valid() indicates a pre-existing pointer'


; if(keyword_set(verbose)) then print, ' HEAP_GC called will reset pointer indeces'

; closed bug 708: This didn't work.
ab = ptr_new(fltarr(12))
cmp = {a:ab, b:ab}
err=0
pcmp = ptr_new(cmp)
cmp = 0

if ptr_valid(ptr_valid(10001,/cast)) then err++ $
	else if(keyword_set(verbose)) then message,/con,'NullPointer ok'


	p = (ptr_valid())[0] 
	pval = ptr_valid(p,/get_heap)
	if keyword_set(verbose)  then message,/con,' ptr_valid(p,/get_heap) value=',pval

if ~ptr_valid(p) then err++ $
	else if keyword_set(verbose)  then message,/con,' p =ab ok'
	newptr = ptr_valid(pval,/cast)

if newptr ne p then err++ $
	else if keyword_set(verbose)  then message,/con,' ptr=ptr_valid(lval,/cast) passed'

llist = list() & mlist = list()
pps=ptrarr(2)
pps[0] = ptr_new(llist)
pps[1] = ptr_new(mlist)
if total(ptr_valid(pps)) ne 2  then err++ $
else if(keyword_set(verbose)) then message,/con,'2 created pointers are valid"

; GD: I'm not sure about the pertinence of above tests. Issue #425 showed that PTR_VALID was perfectly invalid in most cases.
; the following is however sure:
; will crash if bug #241 is not cured as ptr_valid(on_a_not_pointer) is always 0 whatever the type.
a={un:1, deux:[0,4], trois:[0.66,68.33,222.16], quatre:'zzzzz'}
; simple tests 
x=ptr_valid(a) & print,x ; before would have crashed on a being a structure
x=ptr_valid(a.(1)) & print,x & if total(x) ne 0 then err++
x=ptr_valid(a.(2)) & print,x & if total(x) ne 0 then err++
x=ptr_valid(a.(3)) & print,x & if total(x) ne 0 then err++
; more complicated: valid and not valid array of pointers:
D=PTRARR(10)& c=dindgen(10) & for i=0,5 do d[i]=ptr_new(c[i])
; x should be a pointer on the double precision value "2.000", of course provided we get the value of the heap slot good for d[2]:
pos=ptr_valid(d[2],/get)
x=PTR_VALID(pos,/cast)
if isa(x,"pointer") ne 1 then err++
if isa((*x),"Double") ne 1 then err++
if *x ne 2 then err++
res=PTR_VALID(D,/GET) & if isa(res,"Ulong") ne 1 then err++
; last 4 values of res must be zero as they are not initialized:
if total(res[6:9]) ne 0 then err++
; same with byte output
res=PTR_VALID(D) & if isa(res,"Byte") ne 1 then err++
; last 4 values of res must be zero as they are not initialized:
if total(res[6:9]) ne 0 then err++
PTR_FREE, D ; clean pointed values ---> NULL
res=PTR_VALID(D) & if total(res) ne 0 then err++
; x points now to <nothing>:
if ptr_valid(x) ne 0 then err++

; following should complain and must be trapped:
;zz=ptr_valid(a,/cast) --> struct expression not allowed in this context: A

if(keyword_set(test)) then stop,' at end of test routine'
;
;
banner_for_testsuite,' TEST_PTR_VALID',err
if (err gt 0) and ~keyword_set(noexit) then exit, status = 1 $
  else if (err eq 0) then print,'Success!'

return
end
