pro test_bug_3055720
  if ~ncdf_exists() then exit, status=77
  nc = ncdf_create('test_bug_3055720.nc', /clo)
  nc_va = ncdf_vardef(nc, 'A', [ncdf_dimdef(nc, 'X', 10)])
  ncdf_control, nc, /endef 
  ; just ensuring it fails
  if execute("ncdf_varput, nc, nc_va, fltarr(10+1)") eq 1 then exit, status=1
end
