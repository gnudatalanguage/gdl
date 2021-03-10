pro test_spawn_unit
  str_a = 'AQQ'
  spawn, 'echo ' + str_a, unit=u
  str_b = '   '
  readu, u, str_b
  if str_a ne str_b then exit, status=1
end
