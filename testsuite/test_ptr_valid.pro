pro test_ptr_valid, test=test,verbose=verbose, noexit=noexit, quiet=quiet
;
;	First, look for pre-existing pointers.
; if there are any, maybe the user doesn't want to run this.
compile_opt idl2,hidden
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
;
; collect all three pointers. Just for grins.  Track the refcounts.
;
	hrp = heap_refcount(p)
	hrpcmp = heap_refcount(pcmp)
	hrab = heap_refcount(ab)
	ptrall = [p,pcmp,ab]
	hr00 = [hrp, hrpcmp, hrab]
; since this is not a heap_refcount test, just report the results if interested.
if(keyword_set(verbose)) then $
	message,/con,' heaprefcount increment after attachment = ', $
						heap_refcount(ptrall) - hr00
; we should only have 3 valid pointers, so pointer #10001 is not valid.

pvnull = ptr_valid(100001,/cast) ;	this is a null pointer
if ptr_valid(pvnull) or ptr_valid(100001) then err++ $
	else if(keyword_set(verbose)) then message,/con,'NullPointer ok'

p = (ptr_valid())[0]
if ~ptr_valid(p) then err++ $
	else if keyword_set(verbose)  then message,/con,' p =ab ok'

; from all of the valid pointers, get the first one available, put in p:
	p = (ptr_valid())[0] 
	pval = ptr_valid(p,/get_heap)	
if ~array_equal(size(pval), [0, 3, 1]) then err++ $
	else if keyword_set(verbose)  then message,/con,' ptr_valid(p,/get_heap) value=',pval
if ptr_valid(pval,/cast) ne p then err++ $
	else if keyword_set(verbose)  then message,/con,' ptr_valid(pval,/cast) = p '

; Get some pointers to objects and push them into arrays and pass them to routines
llist = list() & mlist = list()
pps=ptrarr(2)
pps[0] = ptr_new(llist)
pps[1] = ptr_new(mlist)
if total(ptr_valid(pps)) ne 2  then err++ $
	else if(keyword_set(verbose)) then message,/con,'2 created pointers are valid"
if ~array_equal([llist,mlist],[*pps[0],*pps[1]]) then err++ $
	else if(keyword_set(verbose)) then message,/con,'array_equal([llist,mlist] , [*pps[0],*pps[1]])'
;
if(keyword_set(test)) then stop,' at end of test routine'
;
;
if ~keyword_set(quiet) then $
	banner_for_testsuite,' TEST_PTR_VALID',err $
	else return

if (err gt 0) and ~keyword_set(noexit) then exit, status = 1 $
  else if (err eq 0) then print,'Success!'

return
end

heap_free,ptr_valid()
heap_free,obj_valid()
heap_GC

message,/con,' TEST_PTR_VALID: Running it 100 times!'
for k=0,99 do test_ptr_valid,/quiet
test_ptr_valid
end
