pro test_bug_3147181 
  if string(1, format="('<',i0,'>')") ne '<1>' then exit, status=1
end
