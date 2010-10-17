; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
pro check_ps_color, file=file, decomposed=decomposed, color=color, expected_rgb=expected_rgb

  device, /color, decomposed=decomposed, file=file
  polyfill, [0,1,1,0], [0,0,1,1], color=color, /normal
  device, /close

  spawn, "grep -e '\([0-1]\.[0-9]\+ \)\{3\}\+[setrgbcolor|C]' " + file + "|tail -1", output
  file_delete, tmpfile, /quiet

  reads, output, r,g,b
  if ~array_equal(byte(255*[r,g,b]), expected_rgb) then begin
    message, 'rgb triple read from the PS file does not math the expected one', /conti
    exit, status=1
  endif

end

pro test_ps_decomposed

  tmpfile = 'test_ps_decomposed.ps'
  set_plot, 'ps'

  device, get_decomposed=isdecomposed
  if isdecomposed then begin
    message, 'PS terminal use indexed colours by default', /conti
    exit, status=1
  endif

  check_ps_color, file=tmpfile, color='ff'x, /decomposed, expected_rgb=[255,0,0]
  check_ps_color, file=tmpfile, color='ff00'x, /decomposed, expected_rgb=[0,255,0]
  check_ps_color, file=tmpfile, color='ff0000'x, /decomposed, expected_rgb=[0,0,255]

end 
