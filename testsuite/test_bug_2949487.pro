pro test_bug_2949487
  a=1
  if execute("print, form='', a") ne 1 then exit, status=1
end
