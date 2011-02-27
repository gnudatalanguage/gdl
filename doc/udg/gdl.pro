; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>

; function that runs a given command in GDL and returns the output
function gdl, cmd, status
  spawn, 'echo gdldoc_tmp_$PPID.pro', startup & startup = startup[0]
  openw, tmpfile, startup, /delete, /get_lun
  writeu, tmpfile, cmd + string(10b) + 'exit' + string(10b)
  flush, tmpfile
  spawn, $
    'export GDL_PATH=../../src/pro;' + $
    '../../src/gdl -quiet ' + startup + ' 2>/dev/stdout', out, exit_status=status
  free_lun, tmpfile
  return, out
end
