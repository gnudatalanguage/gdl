pro test_bug_3061072
  if ~ncdf_exists() then exit, status=77

  n = ncdf_create('/dev/null',/clo)
  ncdf_control, n, /endef

  if execute('a = ncdf_varid(n, "a")') ne 1 then exit, status=1
  if -1 ne ncdf_varid(n, "a") then exit, status=1

  if execute('a = ncdf_attname(n, 0, /global)') ne 1 then exit, status=1
  if "" ne ncdf_attname(n, 0, /global) then exit, status=1

end
