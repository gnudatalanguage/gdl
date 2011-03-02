pro o__define
  struct = {o, a:0}
end

function o::aFun, v
  return, v
end

pro o::aPro, v
  v = 2
end

pro test_idl8

  a = [1, 2, 3]

  message, 'testing FOREACH statement', /conti
  foreach i, a do i = 0 ; should do nothing
  sum = 0
  foreach i, a do begin
    sum += i
  end
  if sum ne total(a) then begin
    message, '1+2+3 != 6', /conti
    exit, status=1
  endif

  message, 'testing negative array indices (for ranges)', /conti
  if total(a[-2: -1]) ne total(a[n_elements(a) - 2 : n_elements(a) - 1]) then begin
    message, '2+3 != 5', /conti
    exit, status=1
  endif

  message, 'testing negative array indices (for single elements)', /conti
  a = findgen(10)
  res = execute('a=a[-1]') 
  if res ne 1 or a ne 9 then begin
    message, 'a[-1] failed', /conti
    exit, status=1
  endif
  
  message, 'testing the dot operator for member routine access', /conti
  o = obj_new('o')
  if o.aFun(1) ne 1 then begin
    exit, status=1
  endif
  o.aPro, v
  if v ne 2 then begin
    exit, status=1
  endif
  obj_destroy, o

end
