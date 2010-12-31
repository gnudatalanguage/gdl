; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
; uses GDL.so symlink created in test_python_module_0 and deleted test_python_module_2

pro test_python_module_1

  if ~file_test('GDL.so') then begin
    message, /conti,  'GDL.so not found'
    exit, status=77
  endif

  if !VERSION.OS_NAME eq 'Darwin' then begin
    spawn, 'otool -L GDL.so | grep Python | cut -d'' '' -f1', python
    python = file_dirname(strtrim(python, 2)) + '/bin/python'
  endif else begin
    spawn, 'ldd GDL.so | grep python | cut -d'' '' -f3', python
    python = file_dirname(strtrim(python, 2)) + '/../bin/python' ; TODO python2.X
  endelse

  if ~file_test(python) then begin
    message, /conti, python + ' not found'
    exit, status=77
  endif

  ; testing if importing works
  spawn, 'echo "import GDL" | ' + python, exit_status=ex
  if ex ne 0 then exit, status=1

  ; testing GDL.pro
  spawn, 'echo "import GDL; GDL.pro(\"print\", \"Hello world!\")" | ' + python, out, exit_status=ex
  if ex ne 0 then exit, status=1
  if strpos(out[0], 'Hello world!') eq -1 then begin
    message, /conti, 'Hello world != Hello world (GDL.pro failed?)'
    exit, status=1
  endif

  ; testing GDL.function
  spawn, 'echo "import GDL; print GDL.function(\"sin\", 1)" | ' + python, out, exit_status=ex
  if ex ne 0 then exit, status=1
  if strpos(out[0], '0.841') eq -1 then begin
    message, /cont, 'sin(1) != 0.841... (GDL.function failed?)'
    exit, status=1
  endif

  ; testing Numpy
  spawn, 'echo "import GDL; print GDL.function(\"findgen\", 100)" | ' + python, out, exit_status=ex
  if ex ne 0 then exit, status=1

end
