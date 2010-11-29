pro test_bug_3091599
  if ~array_equal(size(({b:0})[0]), [1,1,8,1]) then exit, status=1
end
