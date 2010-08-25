; the name 'zzz' is chosen to place this test at the end of the list
pro test_zzz
  if strpos(!GDL.RELEASE,'CVS') eq -1 then exit, status=77
  spawn, './checks', exit_status=status
  exit, status=status
end
