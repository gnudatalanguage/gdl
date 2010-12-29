; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
;
; assumes the following sequence of events:
; - ./configure
; - make 
; - ./configure --enable-python_module
; - make
; - make check

pro test_python_module_0

  image = '../src/.libs/libgdl.' + (!VERSION.OS_NAME eq 'Darwin' ? "dylib" : "so")
  if ~file_test(image) then begin
    message, /conti, image + ' not found'
    exit, status=77
  endif

  if ~file_test('GDL.so') then spawn, 'ln -s ' + image + ' GDL.so'

end
