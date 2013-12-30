pro test_bug_3572473
  if string([1,2],form='(10(:,"A",i0,"b"))') ne 'A1bA2b' then exit, status=1
end
