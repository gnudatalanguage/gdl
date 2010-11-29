pro test_bug_3104326
  spawn, 'echo "' $
  + '.compile test_bug_3104326.pro ' + string(10b) $
  + 'test_bug_3104326_helper' + string(10b) $
  + '" | ../src/gdl -quiet 2>/dev/stdout', out
  if strpos(out[n_elements(out) - 2], '10 test_bug_3104326.pro') eq -1 then exit, status=1  
end

pro test_bug_3104326_helper
  test_bug_3104326_helper_sub
end

pro test_bug_3104326_helper_sub
  nonexistent
end
