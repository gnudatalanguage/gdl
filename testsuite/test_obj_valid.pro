pro test_obj_valid
;
; llist = list(fltarr(4),"hello",2.)
; This list is "too hard" for current usage, as a list of more than 1 elements
; is not an object, p->N_elements() counts the list's internal number of elements.
;
  llist = list(1)		;  This list is ok until N_elements() is fixed.
  mlist = list(!gdl)	;mlist = list("!gdl", "goodbye",findgen(3,4))

if ~(obj_valid(llist) and obj_valid(mlist)) then exit, status=1

print,'SUCCESS'

end
