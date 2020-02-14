pro test_gh00716
  classes = obj_class(count=count)
  foreach str, classes do if str eq '!CPU' then exit, status=1
  if count ne n_elements(classes) then exit, status=1
  class = obj_class("!CPU")
  if class eq "!CPU" then exit, status=1
end
