pro test_python

  has_python = ~ (execute("python, 'numpy'") eq 0)  
  if ~ has_python then exit, status=77

  if abs(python("numpy", "arccos", .5) - acos(.5)) gt 1e-6 then begin
    message, /conti, 'numpy.arccos(.5) != acos(.5)'
    exit, status=1
  endif

end
