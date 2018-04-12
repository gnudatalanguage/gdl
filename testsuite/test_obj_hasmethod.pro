pro test_obj_hasmethod
llist = list(fltarr(4),"hello",2.)
mlist = list(!gdl, "goodbye",findgen(3,4))
if ~(obj_valid(llist) and obj_valid(mlist)) then exit, status=1
pps=ptrarr(2)
pps[0] = ptr_new(llist)
pps[1] = ptr_new(mlist)
if total(ptr_valid(pps)) ne 2  then exit, status=1
hasadd = obj_hasmethod(mlist,'ADD')
; MOVE is not yet a list method.
hasmove= obj_hasmethod(mlist,'MOVE')
hasremove= obj_hasmethod(mlist,'REMOVE')
hastoarray=obj_hasmethod(mlist,'TOARRAY')
hasall= hasadd + hasmove + hasremove + hastoarray
if(hasall ge 3) then print,'SUCCESS' else exit, status=1
;rcbeg = heap_refcount(pps)
;obj_destroy,llist,mlist
;rcend = heap_refcount(pps)
return
end
