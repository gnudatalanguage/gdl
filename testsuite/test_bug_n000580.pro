pro test_bug_n000580
  if execute("print,!const.pi") ne 1 then exit, status=1  
end
