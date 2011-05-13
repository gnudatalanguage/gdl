pro b__define
  struct = {b}
end
pro a__define
  struct = {a, inherits b}
end
pro test_bug_3189072
  if execute("help, obj_new('a')") ne 0 then exit, status=1
end
