; https://github.com/gnudatalanguage/gdl/issues/1445
pro test_array_indices_star
  a=cindgen(32)
  res=EXECUTE('print, a[-8:*]')
  if ~res then EXIT, status=1
end
