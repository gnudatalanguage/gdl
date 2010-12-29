pro test_bug_3091610
  a = {b:0, c:findgen(10)}
  b = {c:fltarr(10)}
  struct_assign, a, b
end
