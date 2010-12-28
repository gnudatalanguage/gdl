pro test_bug_2555865

  err = 0
  i = 0
  n = 100
  v = ptrarr(n)
  f = strarr(n)
  a = strarr(n)

  v[i] = ptr_new(  42) & f[i] = '(%"%d")' & a[i] = '42' & ++i
  v[i] = ptr_new(  42) & f[i] = '(%"%f")' & a[i] = '42.000000' & ++i
  v[i] = ptr_new(  42) & f[i] = '(%"%g")' & a[i] = '42' & ++i

  n = i
  for i = 0, n - 1 do begin
    if string(*v[i], format=f[i]) ne a[i] then begin
      message, 'string(' + strtrim(string(*v[i]), 2) + ', format=''' + f[i] + ''') != ''' + a[i] + '''', /conti
      ++err
    endif
  endfor

  if err ne 0 then exit, status=1
  
end
