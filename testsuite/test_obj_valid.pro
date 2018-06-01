pro test_obj_valid, verbose=verbose, quiet=quiet, noexit=noexit
;
 llist = list(fltarr(4),"hello",2.)
;
; when N_Elements() hack is fixed this won't happen anymore:
; GDL> help,[llist]
;GDLArray line 210 ix=1, sz = 1
;GDLArray line 210 ix=2, sz = 1
;<Expression>    OBJREF    = Array[1]
;
err = 0
llist = list(fltarr(4),"hello",2.)
mlist = list(!gdl, "goodbye",findgen(3,4))
if ~(obj_valid(llist) and obj_valid(mlist)) then exit, status=1

pval = lonarr(2)
pval[0] = obj_valid(llist,/get_heap_identifier)
pval[1] = obj_valid(mlist,/get_heap_identifier)
twolist = [llist, mlist]
if ~array_equal(obj_valid(twolist,/get_heap_identifier), pval) then err++ $
	else if(keyword_set(verbose)) then message,/con,'/GET_HEAP ok'
againtwo = obj_valid(pval,/cast)
if ~array_equal(twolist, againtwo) then err++ $
	else if(keyword_set(verbose)) then message,/con,'/CAST ok'

if ~keyword_set(quiet) then $
	banner_for_testsuite,' TEST_OBJ_VALID',err $
	else return
	
if (err gt 0) and ~keyword_set(noexit) then exit, status = 1 $
  else if (err eq 0) then print,'Success!'
  
end
