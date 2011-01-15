pro test_bug_3100945
  if ~ncdf_exists() then exit, status=77
  nc = ncdf_create('/dev/null', /clobber)
  ncdf_attput, nc, 'gatt0', 0., /global
  ncdf_attput, nc, 'gatt1', 1., /global
  dt = ncdf_dimdef(nc, 'T', /unlimited)
  va = ncdf_vardef(nc, 'A', [dt])
  ncdf_attput, nc, va, 'att0', 10.
  ncdf_attput, nc, va, 'att1', 11.
  ncdf_control, nc, /endef
  if ncdf_attname(nc, 0, /global) ne 'gatt0' then exit, status=1
  if ncdf_attname(nc, 1, /global) ne 'gatt1' then exit, status=1
  if ncdf_attname(nc, va, 0) ne 'att0' then exit, status=1
  if ncdf_attname(nc, va, 1) ne 'att1' then exit, status=1
end
