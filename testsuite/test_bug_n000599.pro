pro test_bug_n000599
  if 200. mod 12 lt 8 then exit, status=1
  if 200D mod 12 lt 8 then exit, status=1
end
