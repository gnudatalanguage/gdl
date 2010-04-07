; the name 'zzz' is chosen to place this test at the end of the list
pro test_zzz
  spawn, './checks', exit_status=status
  exit, status=status
end
