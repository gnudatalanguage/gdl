pro test_ptr_valid, test=test,verbose=verbose, noexit=noexit
;
;	First, look for pre-existing pointers.
; if there are any, maybe the user doesn't want to run this.
p = ptr_valid()
if size(p,/type) ne 10 then begin
	message,' ptr_valid() did not return even a pointer type '
	exit, status=1
	endif
if(n_elements(p) ne 1) then begin
	message,' ptr_valid() indicates multiple pre-existing pointers'
	exit, status=1
	endif
if ptr_valid(p[0]) then begin
	message,' ptr_valid() indicates a pre-existing pointer'
	exit, status=1
	endif
;
; clear the heaps and reset its index so that we can predictably create
; a new pointer in ptr_valid(/cast).
heap_free,ptr_valid()
heap_free,OBJ_valid()
HEAP_GC
;	
 if(keyword_set(verbose)) then print, ' HEAP_GC called will reset pointer indeces'
; closed bug 708: This didn't work.
ab = ptr_new(fltarr(12))
cmp = {a:ab, b:ab}
err=0

cmp = 0
if ptr_valid(ptr_valid(10001,/cast)) then err++ $
else if(keyword_set(verbose)) then message,/con,'NullPointer ok'

 p = ptr_valid(1,/cast)	; scalar
if ~ptr_valid(p) then err++ $
else if keyword_set(verbose)  then message,/con,' p =ab ok'

llist = list(fltarr(4),"hello",2.)
mlist = list(!gdl, "goodbye",findgen(3,4))
if ~(obj_valid(llist) and obj_valid(mlist)) then err++ $
else if(keyword_set(verbose)) then message,/con,'2 created list objects are valid"
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
  else print,'Success!'

return
end
