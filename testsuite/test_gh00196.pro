pro test_gh00196 
  spawn, "../src/gdl -quiet -e 'a=routine_info(/so)'", stdout, stderr
  if n_elements(stderr) > 0 then exit, status=1
end
