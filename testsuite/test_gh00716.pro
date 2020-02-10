pro test_gh00716
  classes = obj_class()
  foreach str, classes do if str eq '!CPU' then exit, status=1
end
