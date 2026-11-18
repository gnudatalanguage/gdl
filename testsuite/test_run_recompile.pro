; #2257: recompiling an inactive routine at STOP must preserve the caller.
pro test_run_recompile_write, file, lines
  openw, lun, file, /get_lun
  printf, lun, lines, format='(A)'
  free_lun, lun
end

pro test_run_recompile
  errors = 0
  cd, current=original
  stamp = strtrim(string(systime(1), format='(F20.6)'), 2)
  directory = filepath('gdl-run-recompile-' + stamp, /tmp)
  catch, error
  if error ne 0 then begin
    catch, /cancel
    errors_add, errors, !error_state.msg
    goto, cleanup
  endif

  executable = getenv('GDL_TEST_EXECUTABLE')
  if executable eq '' then executable = get_path_to_exe()
  file_mkdir, directory
  cd, directory
  test_run_recompile_write, 'child.pro', ['pro child', 'end']
  test_run_recompile_write, 'parent.pro', ['pro parent, result', $
    'local = 42', 'child', 'stop', 'result = local', 'end']
  test_run_recompile_write, 'driver.pro', ['parent, resumed', 'driver_finished = 1', 'end']
  test_run_recompile_write, 'mixed.pro', ['function answer', 'return, 42', 'end', $
    'value = answer()', 'end']

  ; Feed the commands to the child prompt. STOP leaves execution inside PARENT.
  test_run_recompile_write, 'commands', [ $
    '.compile mixed', 'resumed = -1', 'parent, resumed', '.r child', $
    'if scope_level() ne 2 then exit, status=1', $
    'if local ne 42 then exit, status=2', '.continue', $
    'if resumed ne 42 then exit, status=3', $
    'if n_elements(value) ne 0 then exit, status=4', $
    'parent, resumed', '.r parent', $
    'if scope_level() ne 1 then exit, status=5', $
    'driver_finished = 0', '.run driver', '.r child', $
    'if scope_level() ne 2 then exit, status=6', '.continue', $
    'if driver_finished ne 1 then exit, status=7', $
    'driver_finished = 0', '.run driver', '.run mixed, child', $
    'if scope_level() ne 1 then exit, status=8', $
    'if n_elements(value) ne 1 then exit, status=9', $
    'if value ne 42 or driver_finished ne 0 then exit, status=10', $
    'parent, resumed', '.rnew child', $
    'if scope_level() ne 1 or n_elements(resumed) ne 0 then exit, status=11', $
    "print, 'Recompile checks passed'", 'exit']

  if !version.os_family eq 'Windows' then begin
    spawn, '("'+executable+'" -quiet < commands 2>&1)', output, exit_status=status
  endif else begin
    spawn, ['/bin/sh', '-c', '"$0" -quiet < commands 2>&1; exit $?', executable], $
      output, /noshell, /sh, exit_status=status
  endelse
  log = strjoin(output, string(10b))
  if (status ne 0) or (strpos(log, 'Recompile checks passed') lt 0) or $
     (strpos(log, 'Procedure was compiled while active: PARENT') lt 0) then begin
    errors_add, errors, 'Child GDL exit status '+strtrim(status, 2)
    print, log
  endif

cleanup:
  catch, /cancel
  cd, original
  if file_test(directory, /directory) then file_delete, directory, /recursive
  banner_for_testsuite, 'TEST_RUN_RECOMPILE', errors
  if errors gt 0 then exit, status=1
end
