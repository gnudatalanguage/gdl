; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
; part of GDL - GNU Data Language
pro test_bug_3288652
  arr = routine_names(/s_fun)
  foreach a, arr do if a eq 'WTN' then exit, status=0
  exit, status=1
end
