pro test_bug_3313522
  i = 2
  t = size(i, /type)
  s = string(i, '(I1)')
  if size(i, /type) ne t then exit, status=1
end
