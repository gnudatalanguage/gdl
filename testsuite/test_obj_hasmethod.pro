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
pro test_obj_hasmethod, verbose=verbose, noexit=noexit, quiet=quiet

compile_opt idl2, hidden

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
if(hasall ne 4) then exit, status=1 $
	else if(keyword_set(verbose)) then message,/con,'BEGIN:'

err=0
if ~obj_hasmethod(mlist,['cleanup','ADD','move','remove','toarray']) then err++ $
	else if(keyword_set(verbose)) then message,/con,' LIST object has cleanup + more'

ot = obj_new('objtest')
if ~obj_hasmethod(ot,['cleanup']) then err++ $
	else if(keyword_set(verbose)) then message,/con,' easy object has cleanup'

obj_destroy,ot,prm1,prm2,prm3

tests = ['cleanup','move','add','remove','reverse','swap','count']
if ~obj_hasmethod(llist,tests) then err++ $
	else if(keyword_set(verbose)) then message,/con,' LIST object has much more'

hh=hash('one',1)
if ~obj_hasmethod(hh,'cleanup') then $
  if(keyword_set(verbose)) then message,/con," HASH does not have ::cleanup ... shouldn't it?" $
  else $
  if(keyword_set(verbose)) then message,/con," HASH DOES have ::cleanup ... does it clean well?"

  inhash = $
['REMOVE','COUNT','HASKEY','ISEMPTY','ISFOLDCASE','ISORDERED','KEYS','REMOVE','TOSTRUCT','VALUES','WHERE']
if ~obj_hasmethod(hh,inhash) then err++ $
	else if(keyword_set(verbose)) then message,/con,' HASH has:'+strjoin(inhash+' ')
if ~array_equal(obj_hasmethod([hh,llist,mlist],['count','remove']) , 1+lonarr(3)) then err++ $
	else if(keyword_set(verbose)) then message,/con,' HASH and LIST both have count, remove'

if ~keyword_set(quiet) then $
	banner_for_testsuite,' TEST_OBJ_HASMETHOD',err $
	else $
	if (err eq 0) then return

if (err gt 0) and ~keyword_set(noexit) then exit, status = 1 $
  else if (err eq 0) then print,'Success!'

end
test_obj_hasmethod,/quiet
;help,/heap
ptrleft = ptr_valid()
objleft = obj_valid()
objname = obj_class(objleft)
NOBJ = N_ELEMENTS(OBJname) 
if objname[0] eq '' then print,' No leftover objects' $
	else begin
		print,' leftover objects!!'
		FOR K=0,NOBJ-1 DO print,obj_class(objleft[k]),' ID=',obj_valid( objleft[k],/get_heap)
	endelse
end
