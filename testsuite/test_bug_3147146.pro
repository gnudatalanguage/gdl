pro test_bug_3147146

  err = 0
  i = 0
  n = 100
  v = ptrarr(n)
  f = strarr(n)
  a = strarr(n)

  v[i] = ptr_new(  -3) & f[i] = '(%"%09o")' & a[i] = '000177775' & ++i
  v[i] = ptr_new( -3l) & f[i] = '(%"%09o")' & a[i] = '*********' & ++i
  v[i] = ptr_new(-3ll) & f[i] = '(%"%09o")' & a[i] = '*********' & ++i
  v[i] = ptr_new(  -3) & f[i] = '(%"%9o")'  & a[i] = '   177775' & ++i
  v[i] = ptr_new(  -3) & f[i] = '(%"%9x")'  & a[i] = '     FFFD' & ++i
  v[i] = ptr_new(  -3) & f[i] = '(%"%09x")' & a[i] = '00000FFFD' & ++i
  v[i] = ptr_new(  -3) & f[i] = '(%"%9z")'  & a[i] = '     FFFD' & ++i
  v[i] = ptr_new(  -3) & f[i] = '(%"%09z")' & a[i] = '00000FFFD' & ++i
  v[i] = ptr_new(dcomplex(-1,-1))   & f[i] = '(2O06)'   & a[i] = '************'                   & ++i

  n = i
  for i = 0, n - 1 do begin
    if string(*v[i], format=f[i]) ne a[i] then begin
      message, 'string(' + strtrim(string(*v[i]), 2) + ', format=''' + f[i] + ''') != ''' + a[i] + '''', /conti
      ++err
    endif
  endfor

  if err ne 0 then exit, status=1
  
end
