; just exercising the idl_idlbridge object and library, not a real test
pro test_gdl2gdl
  a=obj_new("idl_idlbridge")
  a->Setvar,"A",indgen(10)
  a->execute,"A-=indgen(10)"
  a->execute,"A+=1"
  b=a->getvar("A")
  if (total(b) ne 10) then print,"error"
end
