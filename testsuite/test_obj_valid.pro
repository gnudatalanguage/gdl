pro test_obj_valid
llist = list(fltarr(4),"hello",2.)
;	print,' test_obj_valid 0'
mlist = list("!gdl", "goodbye",findgen(3,4))
;	brackets put because a strict scalar is not always welcome.
;
if ~(obj_valid([llist]) and obj_valid([mlist])) then exit, status=1
pps=ptrarr(2)
;	print,' test_obj_valid 2'
pps[0] = ptr_new(llist)
pps[1] = ptr_new(mlist)
if total(ptr_valid(pps)) ne 2  then exit, status=1
print,'SUCCESS'

end
