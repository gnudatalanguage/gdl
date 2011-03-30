pro test_bug_3244840
  openw, u, 'test_bug_3244840.tmp', /get_lun, /delete
  printf, u, ''
  printf, u, 0., format="(f6.2)"
  point_lun, u, 0
  line = ''
  readf, u, line
  readf, u, line
  free_lun, u
  if line ne '  0.00' then exit, status=1
end
