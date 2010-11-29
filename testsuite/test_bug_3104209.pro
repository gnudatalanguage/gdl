pro test_bug_3104209
  spawn, 'echo ".compile test_bug_3104209.pro ' + string(10b) + 'continue" | ../src/gdl'
end

pro test_bug_3104209_helper
  stop
end
