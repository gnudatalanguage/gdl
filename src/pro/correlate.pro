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

    if (size(x))[0] ne 2 then message, 'Expecting two-dimensional array'
    dbl = keyword_set(double) || size(x, /type) eq 5
    nx = (size(x))[1]
    cov = dbl ? dblarr(nx, nx, /nozero) : fltarr(nx, nx, /nozero)

    ; one option would be to call correlate a couple of time... but the code below should be faster
    ;for i = 0, nx - 1 do for j = 0, nx - 1 do $
    ;  cov[i, j] = correlate(x[i, *], x[j, *], double=double, covariance=covariance)

    tmp = dbl ? dblarr(nx, /nozero) : fltarr(nx, /nozero)
    for i = 0, nx - 1 do tmp[i, *] = mean(x[i, *], double=double)
    for i = 0, nx - 1 do for j = 0, nx - 1 do begin
      cov[i, j] = total((x[i, *] - tmp[i]) * (x[j, *] - tmp[j])) 
    endfor
    cov /= nx - 1
    if keyword_set(covariance) then return, cov

    for i = 0, nx - 1 do tmp[i, *] = sqrt(total((x[i, *] - tmp[i])^2, double=double) / (nx - 1.))
    for i = 0, nx - 1 do begin
      cov[i, *] /= tmp[i]
      cov[*, i] /= tmp[i]
    endfor
    return, cov

  endif 
 
  message, 'Incorrect number of arguments.'

end
