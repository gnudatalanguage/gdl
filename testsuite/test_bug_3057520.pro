pro test_bug_3057520
  if ~ncdf_exists() then exit, status=77
  nc = ncdf_create('/dev/null', /clobber)
  dt = ncdf_dimdef(nc, 'T', /unlimited)
  dx = ncdf_dimdef(nc, 'X', 1024)
  va = ncdf_vardef(nc, 'A', [dx, dt])
  ncdf_control, nc, /endef
  if execute("ncdf_varput, nc, va, [1], offset=0") eq 1 then exit, status=1
  if execute("ncdf_varput, nc, va, [1], offset=[0,0,0]") ne 1 then exit, status=1
end
