; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
pro TEST_ERFINV

x = (2 * FINDGEN(100) / 99 - 1)[1 : 98]
for i=0, N_ELEMENTS(x) - 1 do begin
   if ABS(IMSL_ERF(ERF(x[i]), /inv) - x[i]) gt 1e-6 then begin
      MESSAGE, 'erf-1(erf(float x)) != x', /conti
      exit, status=1
   endif
   if ABS(IMSL_ERF(ERF(x[i], /double), /inv, /double) - x[i]) gt 1e-7 then begin
      MESSAGE, 'erf-1(erf(float x, /double), /double) != x', /conti
      EXIT, status=1
   endif
endfor

x = (2 * dindgen(100) / 99 - 1)[1 : 98]
for i=0, n_elements(x) - 1 do begin
   if ABS(IMSL_ERF(ERF(x[i]), /inv) - x[i]) gt 1e-7 then begin
      MESSAGE, 'erf-1(erf(double x)) != x', /conti
      EXIT, status=1
   endif
endfor

end
