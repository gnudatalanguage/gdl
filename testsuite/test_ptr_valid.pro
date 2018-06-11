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

;
if(keyword_set(test)) then stop,' at end of test routine'
;
;
banner_for_testsuite,' TEST_PTR_VALID',err
if (err gt 0) and ~keyword_set(noexit) then exit, status = 1 $
  else if (err eq 0) then print,'Success!'

return
end
