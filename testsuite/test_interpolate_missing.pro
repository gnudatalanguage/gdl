pro interpolate_missing_pro

  err = 0

  a = interpolate([1,3],[-1,0,1,2], missing=-10)
  if a[0] ne -10 || a[3] ne -10 then err++

  a = interpolate([1,3,4],[-1,0,1,3], missing=-10, /cubic)
  if a[0] ne -10 || a[3] ne -10 then err++

  if err ne 0 then exit, status=1

end
