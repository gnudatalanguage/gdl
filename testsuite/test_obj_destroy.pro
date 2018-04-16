pro test_obj_destroy
llist = list(fltarr(4),"hello",2.)
mlist = list(!gdl, "goodbye",findgen(3,4))
pps=ptrarr(2)
pps[0] = ptr_new(llist)
pps[1] = ptr_new(mlist)
rcbeg = heap_refcount(pps)
obj_destroy,llist,mlist
rcend = heap_refcount(pps)
if (obj_valid(llist) or obj_valid(mlist)) then exit, status=1
end
