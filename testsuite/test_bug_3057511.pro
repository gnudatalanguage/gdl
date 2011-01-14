pro test_bug_3057511
  if ~ncdf_exists() then exit, status=77
  if execute("a = ncdf_dimdef(ncdf_create('/dev/null',/clo), 'x')") eq 1 then exit, status=1
  if execute("a = ncdf_dimdef(ncdf_create('bug.nc',/clo), 'x', /unlimited)") ne 1 then exit, status=1
end
