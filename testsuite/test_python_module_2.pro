pro test_python_module_2

  if ~file_test('GDL.so') then begin
    message, /conti, 'GDL.so not found'
    exit, status=77
  endif

  spawn, 'rm GDL.so'

end
