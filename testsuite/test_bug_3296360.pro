pro test_bug_3296360
  s=replicate(create_struct('t',[0.]),69000.0) & s.t[0]=findgen(69000.0) 
  if max(s.t) ne 68999.0 then exit, status=1
end
