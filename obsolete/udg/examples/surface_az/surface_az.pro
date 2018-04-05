!P.MULTI = [0,3,3]
d = dist(10)
for az = 0, 90, 11 do $
  surface, d, az=az, title='az=' + strtrim(az,2)
