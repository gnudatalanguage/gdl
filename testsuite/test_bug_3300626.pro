pro test_bug_3300626
    if !version.os_family eq 'Windows' then return
  openr, u, '/dev/zero', /get_lun
  a = assoc(u, bytarr(10,10))
  i = 1
  if execute("b = a[i]") ne 1 then exit, status=1
end
