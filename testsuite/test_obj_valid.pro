pro test_obj_valid
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

if ~(obj_valid(llist) and obj_valid(mlist)) then exit, status=1

print,'SUCCESS'

end
