pro test_bug_3147733 
  spawn, 'echo "' $
  + '.compile test_bug_3147733.pro ' + string(10b) $
  + 'test_bug_3147733_helper' + string(10b) $
  + '.cont' + string(10b) $
  + '" | ../src/gdl -quiet &>/dev/stdout', out
  foreach i, out do if strpos(i, 'Internal error. ProgNode::Run() called.') ne -1 then exit, status=1
end

pro test_bug_3147733_helper
  print, b + ''
end
