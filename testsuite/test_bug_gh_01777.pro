; https://github.com/gnudatalanguage/gdl/issues/1777
pro test_bug_gh_01777
  a = [1] & a /= 2.
  if a ne .5 then exit, status=1  

  a = [1.] & a /= 2.
  if a ne .5 then exit, status=1  

  a = [10] & a /= 2
  if a ne 5 then exit, status=1  

  a = [10.] & a /= 2
  if a ne 5 then exit, status=1  
end
