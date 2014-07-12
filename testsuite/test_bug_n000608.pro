pro test_bug_n000608
  p = ptr_new()
  help, p, out=a
  if a ne "P               POINTER   = <NullPointer>" then exit, status=1
end
