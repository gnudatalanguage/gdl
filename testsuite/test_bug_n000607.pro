pro test_bug_n000607
  o=obj_new()
  if execute("help, o eq !null") eq 0 then exit, status=1
  if (o EQ !null) ne 1 then exit, status=1
end
