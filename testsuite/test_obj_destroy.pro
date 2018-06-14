;
;
 
PRO ObjTEST::Cleanup, prm1, prm2, prm3
  COMPILE_OPT IDL2 ,HIDDEN
  ; Call our superclass Cleanup method
  ; self->IDL_Object::Cleanup ; (there is no GDL_OBJECT::cleanup)

nprm = N_PARAMS()
print,' ObjTEST::Cleanup, #params=',nprm
;
END
 
PRO ObjTEST__define
  COMPILE_OPT IDL2 ,HIDDEN
  void = {ObjTEST, $
  inherits IDL_Object, $ ; superclass
  center: [0d, 0d], $ ; two-element array
  radius: 0d}  ; scalar value
END
;
;
pro test_obj_destroy, test=test, verbose=verbose
;
llist = list(fltarr(4),"hello",2.)
mlist = list(!gdl, "goodbye",findgen(3,4))
pps=ptrarr(2)
pps[0] = ptr_new(llist)
pps[1] = ptr_new(mlist)
rcbeg = heap_refcount(pps)

obj_destroy,llist
obj_destroy,mlist

rcend = heap_refcount(pps)
if (obj_valid(llist) or obj_valid(mlist)) then exit, status=1

obTEST= obj_new('ObjTEST')
obj_destroy,obTEST,pps,2.
if obj_valid(obTEST) then exit, status=1 else print,'Success!'

end
