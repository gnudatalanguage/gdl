; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
; part of GDL - GNU Data Language
function test_scope_carfetch_sub
  return, scope_varfetch('a', level=-1)
end
pro test_scope_varfetch
  a = 1234
  if a ne test_scope_carfetch_sub() then exit, status=1
end
