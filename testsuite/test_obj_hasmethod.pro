;
;
; provide a test-case object besides LIST
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
pro test_obj_hasmethod
llist = list(fltarr(4),"hello",2.)
mlist = list(!gdl, "goodbye",findgen(3,4))
;if ~(obj_valid(llist) and obj_valid(mlist)) then exit, status=1

hasadd = obj_hasmethod(mlist,'ADD')
; MOVE is not yet a list method.
hasmove= obj_hasmethod(mlist,'MOVE')
;	print,' test_obj_hasmethod 1'
hasremove= obj_hasmethod(mlist,'REMOVE')
hastoarray=obj_hasmethod(mlist,'TOARRAY')
;	print,' test_obj_hasmethod 2'
hasall= hasadd + hasmove + hasremove + hastoarray
if(hasall ge 3) then print,'SUCCESS' else exit, status=1

return
end
