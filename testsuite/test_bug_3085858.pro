pro test_bug_3085858
  a=[0] & a[-1:-1] = 1
  if a[0] ne 1 then exit, status=1
end
