pro test_bug_2876161

  err = 0
;
if !version.os_family eq 'Windows' then begin
  message,/continue,' involves spawning & shell tools -> Windows hangup, so fake success returned'
  return
  endif
;
  if n_elements(string([0], " ", [1], /print)) ne 3 then begin
    message, /conti, '[0], " ", [1]'
    err++
  endif

  if n_elements(string(0, " ", [1], /print)) ne 1 then begin
    message, /conti, '0, " ", [1]'
    err++
  endif

  spawn, '../src/gdl -quiet -e "print, [[1,2],[3,4]], ''''"', res, count=cnt
  if cnt ne 2 then begin
    message, /conti, 'print, [[1,2],[3,4]], '''''
    err++
  endif

  spawn, '../src/gdl -quiet -e "print, ''Hello world'', ''''" | wc -l', res, count=cnt
  if fix(res) ne 1 then begin
    message, /conti, 'print, ''Hello world'', '''''
    err++
  endif

  spawn, '../src/gdl -quiet -e "print, 1, ''''" | wc -l', res, count=cnt
  if fix(res) ne 1 then begin
    message, /conti, 'print, 1, '''''
    err++
  endif

  arg = '0, [1], 1, 1, [2,2,3], 4, [3]'
  spawn, '../src/gdl -quiet -e "print, ' + arg + '" | wc -l', res, count=cnt
  if fix(res) ne 5 then begin
    message, /conti, 'print, ' + arg
    err++
  endif

  if err ne 0 then exit, status=1

end
