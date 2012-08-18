pro test_bug_3559291
  if float('1d-1') ne 0.1 then exit, status=1
  if float('1D-1') ne 0.1 then exit, status=1
  if double('1D-1') ne 0.1d then exit, status=1
  if double('1d-1') ne 0.1d then exit, status=1
end
