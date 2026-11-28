function test_spawn_unit_backward_write, record
  catch, err
  if err ne 0 then begin
    msg = !error_state.msg
    catch, /cancel
    return, strpos(msg, 'backwards') ge 0
  endif
  record[0] = [172b]
  return, 0
end

pro test_spawn_unit
  catch, err
  if err ne 0 then begin
    catch, /cancel
    print, !error_state.msg
    exit, status=1
  endif
  str_a = 'AQQ'
  spawn, 'echo ' + str_a, unit=u
  str_b = '   '
  readu, u, str_b
  free_lun, u
  if str_a ne str_b then exit, status=1

  ; GDL pads a record written past the end of a pipe through its output stream.
  defsysv, '!GDL', exists=is_gdl
  if is_gdl then begin
    gap = 2051L
    spawn, ['cat'], /noshell, unit=u
    record = assoc(u, bytarr(1), gap)
    writeu, u, 42b
    record[0] = [173b]
    record[1] = [174b]
    if ~test_spawn_unit_backward_write(record) then exit, status=1
    record[4] = [175b]
    flush, u
    data = bytarr(gap+5)
    readu, u, data
    free_lun, u
    expected = bytarr(gap+5)
    expected[0] = 42b
    expected[gap:gap+1] = [173b,174b]
    expected[gap+4] = 175b
    if ~array_equal(data, expected) then exit, status=1
  endif
end
