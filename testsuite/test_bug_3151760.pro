function is_expression, a
  return, ~arg_present(a)
end

pro test_bug_3151760
  err = 0

  a = strarr(10)
  if is_expression(string(a)) then err++

  a = strarr(10)
  if is_expression(string(a, format='')) then err++

  a = bytarr(10)
  if is_expression(byte(a)) then err++

  a = intarr(10)
  if is_expression(fix(a)) then err++

  a = uintarr(10)
  if is_expression(uint(a)) then err++

  a = lonarr(10)
  if is_expression(long(a)) then err++

  a = ulonarr(10)
  if is_expression(ulong(a)) then err++

  a = lon64arr(10)
  if is_expression(long64(a)) then err++

  a = ulon64arr(10)
  if is_expression(ulong64(a)) then err++
  
  a = fltarr(10)
  if is_expression(float(a)) then err++

  a = dblarr(10)
  if is_expression(double(a)) then err++

  a = complexarr(10)
  if is_expression(complex(a)) then err++

  a = dcomplexarr(10)
  if is_expression(dcomplex(a)) then err++

print, err
  if err ne 0 then exit, status=1
end
