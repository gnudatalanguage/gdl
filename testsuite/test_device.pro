pro test_device

  set_plot, 'ps'

  device, xsize=10
  if !D.X_SIZE ne 10000 then begin
    message, '!D.X_SIZE ne 10000', /conti
    exit, status=1
  endif

  device, ysize=20
  if !D.Y_SIZE ne 20000 then begin
    message, '!D.Y_SIZE ne 20000', /conti
    exit, status=1
  endif
  
  device, xsize=10, /inches
  if !D.X_SIZE ne 25400 then begin
    message, '!D.X_SIZE ne 25400', /conti
    exit, status=1
  endif

  device, ysize=20, /inches
  if !D.Y_SIZE ne 50800 then begin
    message, '!D.Y_SIZE ne 50800', /conti
    exit, status=1
  endif

end
