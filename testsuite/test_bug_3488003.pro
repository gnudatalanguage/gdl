; test code by Gilles Duvert
pro test_bug_3488003
  A = REPLICATE({VALUE1:DINDGEN(15)},20)
  help,a
  help,a,/Struct
  help,a.VALUE1
  mask=[59,70,89,103,115,119,130,145,179,193]
  a.value1[mask]=1
end
