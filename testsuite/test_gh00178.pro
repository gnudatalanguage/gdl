pro test_gh00178
  cmd = "pm, [1,2], format='(I2)', title='PM'" 
  spawn, '../src/gdl -quiet -e "' + cmd + '"',res, count=cnt 
  if strmatch(res[0], '*error*') eq 1 then exit, status=1 
end
