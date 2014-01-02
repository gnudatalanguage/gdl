pro test_bug_2892631
  if execute("a = replicate(ptr_new(0), 1) & print, *a") eq 1 then exit, status=1
end
