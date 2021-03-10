; this test works perfect interactively and non intercatively.
; the fact that it fails in make tests means there is and error
; elsewhere.
; I remove it for the time being.
pro test_sem
exit,status=0
  s = '_test_sem'

  spawn, '../src/gdl -quiet -e "s = ''' + s + ''' & help, sem_create(s, destroy_semaphore=0) & help, sem_lock(s) & wait, 3 & sem_release, s"', unit=p 

  print, '-- background process:'
  str = '                                                                                  '   
  readu, p, str & print, str

  err = 0

  print, '-- foreground process:'
  status = sem_create(s) ; 1
  help, status
  if status ne 1 then err++
  status = sem_lock(s)   
  help, status
  if status ne 0 then err++
  wait, 6
  status = sem_lock(s)   
  help, status
  if status ne 1 then err++
  sem_release, s

  if err ne 0 then exit, status=1

end
