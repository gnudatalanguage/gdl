spawn, '../../../../src/gdl -quiet -e "exit, status=44" 1>/dev/null', $
  exit_status=s
print, 'spawned GDL process exited with code ', strtrim(s, 2)
