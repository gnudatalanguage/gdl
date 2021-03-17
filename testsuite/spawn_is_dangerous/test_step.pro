pro test_step
  spawn, 'echo "' $
  + '.compile test_step.pro ' + string(10b) $
  + 'test_step_helper, a' + string(10b) $
  + '.step' + string(10b) $
  + 'print, a' + string(10b) $
  + '" | ../src/gdl -quiet', out
  if fix(out[n_elements(out) - 1]) ne 2 then exit, status=1
end

pro test_step_helper, a
  a = 1
  stop
  a = 2
  a = 3
  a = 4
end
