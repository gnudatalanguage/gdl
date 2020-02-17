pro test_obj_valid, noexit=noexit
;
 llist = list(fltarr(4),"hello",2.)
;
; when N_Elements() hack is fixed this won't happen anymore:
; GDL> help,[llist]
;GDLArray line 210 ix=1, sz = 1
;GDLArray line 210 ix=2, sz = 1
;<Expression>    OBJREF    = Array[1]
;
mlist = list("!gdl", "goodbye",findgen(3,4))
err=0
if ~(obj_valid(llist) and obj_valid(mlist)) then err++

; particular case for ptr_valid and comparison with !NULL that should be the inverse
 good=[1b,0b] & p=obj_new('idl_container') & res=[obj_valid(p),p eq !NULL] & if total(res eq good) ne 2 then err++
 good=[0b,1b] & obj_destroy,p & res=[obj_valid(p),!NULL eq p] & if total(res eq good) ne 2 then err++

banner_for_testsuite,' TEST_OBJ_VALID',err
if (err gt 0) and ~keyword_set(noexit) then exit, status = 1 

end
