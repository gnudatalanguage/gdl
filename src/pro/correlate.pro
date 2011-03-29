; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
; part of GNU Data Language - GDL 
function correlate, x, y, covariance=covariance, double=double
  on_error, 2
  if n_params() eq 2 then begin
    l = n_elements(x) < n_elements(y)
    mx = mean(x[0:l-1], double=double)
    my = mean(y[0:l-1], double=double)
    cov = total((x - mx) * (y - my)) / (l - 1.)
    if keyword_set(covariance) then return, cov
    sx = sqrt(total((x[0:l-1] - mx)^2, double=double) / (l - 1.))
    sy = sqrt(total((y[0:l-1] - my)^2, double=double) / (l - 1.))
    return, cov / sx / sy
  endif else if n_params() eq 1 then begin
    message, 'single-argument calls not implemented yet (FIXME!)'
  endif else message, 'Incorrect number of arguments.'
end
