; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
pro test_erfinv

  x = (2 * findgen(100) / 99 - 1)[1 : 98]
  for i=0, n_elements(x) - 1 do begin
    if abs(imsl_erf(erf(x[i]), /inv) - x[i]) gt 1e-6 then begin
      message, 'erf-1(erf(float x)) != x', /conti
      exit, status=1
    endif
    if abs(imsl_erf(erf(x[i], /double), /inv, /double) - x[i]) gt 1e-7 then begin
      message, 'erf-1(erf(float x, /double), /double) != x', /conti
      exit, status=1
    endif
  endfor

  x = (2 * dindgen(100) / 99 - 1)[1 : 98]
  for i=0, n_elements(x) - 1 do begin
    if abs(imsl_erf(erf(x[i]), /inv) - x[i]) gt 1e-7 then begin
      message, 'erf-1(erf(double x)) != x', /conti
      exit, status=1
    endif
  endfor

end
