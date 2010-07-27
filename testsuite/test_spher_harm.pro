; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>

; this one does not perform any sanity checks, and is intedned
; only for testing the C++ implementation as done below
function spher_harm_gdl, theta, phi, l, m
  return, sqrt( $
    (2. * l + 1) / 4. / !PI * factorial(l - abs(m)) / factorial(l + abs(m)) $
  ) * legendre(cos(theta), l, abs(m)) * exp(complex(0, m * phi)) * (m lt 0 ? -1. : 1.)^m
end

; tests SPHER_HARM (and btw FACTORIAL and LEGENDRE)
pro test_spher_harm

  ; testing output shape
  if size(spher_harm(0.,0.,0,0), /dimensions) ne 0 then begin
    message, 'spher_harm(scalar, ...) should be scalar', /conti
    exit, status=1
  endif
  if size(spher_harm([0.],[0.],0,0), /dimensions) ne 1 then begin
    message, 'spher_harm([scalar], ...) should be vector', /conti
    exit, status=1
  endif
  if n_elements(spher_harm([0.,0.,0.],0.,0,0)) ne 3 then begin
    message, 'spher_harm(vector, scalar, ...) should be vector', /conti
    exit, status=1
  endif
  if n_elements(spher_harm(0.,[0.,0.,0.],0,0)) ne 3 then begin
    message, 'spher_harm(scalar, vector, ...) should be vector', /conti
    exit, status=1
  endif

  ; testing output type
  in = [ $
    ptr_new(1b), $
    ptr_new(1), $
    ptr_new(1l), $
    ptr_new(1.), $
    ptr_new(1d), $
    ptr_new(complex(1,1)), $
    ptr_new(dcomplex(1,1)), $
    ptr_new([1b]), $
    ptr_new([1]), $
    ptr_new([1l]), $
    ptr_new([1.]), $
    ptr_new([1d]), $
    ptr_new([complex(1,1)]), $
    ptr_new([dcomplex(1,1)]) $
  ]
  foreach i, in do begin
    if (size(spher_harm(*i, 0d, 0, 0), /type) ne 9) then begin
      message, 'spher_harm(*, double, ...) should be dcomplex'
      exit, status=1
    endif
    if (size(spher_harm(*i, 0., 0, 0, /double), /type) ne 9) then begin
      message, 'spher_harm(*, *, ..., /double) should be dcomplex'
      exit, status=1
    endif
    if size(*i, /type) ne 9 && size(*i, /type) ne 5 then begin
      if (size(spher_harm(*i, 0., 0, 0), /type) ne 6) then begin
        message, 'spher_harm(not double, not double, ...) should be complex'
        exit, status=1
      endif
    endif
  endforeach
  foreach i, in do ptr_free, i

  ; testing output values against GDL-computed harmonics
  thetas = findgen(10.)
  phis = dindgen(5.)
  ls = indgen(2)
  eps = 1e-7
  foreach theta, thetas do foreach phi, phis do foreach l, ls do begin
    foreach m, indgen(2*l+1) - l do begin
      ; scalar computations
      if abs(spher_harm(theta, phi, l, m) - spher_harm_gdl(theta, phi, l, m)) gt eps then begin
        message, 'spher_harm() != spher_harm_gdl()', /conti
        exit, status=1
      endif

      ; vector computations
      if total(abs(spher_harm(thetas, phi, l, m) - spher_harm_gdl(thetas, phi, l, m))) gt eps then begin
        message, 'spher_harm(vector, scalar, ...) != spher_harm_gdl(vector, scalar, ...)', /conti
        exit, status=1
      endif
      if total(abs(spher_harm(theta, phis, l, m) - spher_harm_gdl(theta, phis, l, m))) gt eps then begin
        message, 'spher_harm(scalar, vector, ...) != spher_harm_gdl(scalar, vector, ...)', /conti
        exit, status=1
      endif
      if total(abs(spher_harm(theta, phis, l, m) - spher_harm_gdl(theta, phis, l, m))) gt eps then begin
        message, 'spher_harm(vector, vector, ...) != spher_harm_gdl(vector, vector, ...)', /conti
        exit, status=1
      endif

      ; one more check for negative-m case
      if m lt 0 then begin
        if spher_harm(theta, phi, l, m) ne (-1)^(-m) * conj(spher_harm(theta, phi, l, -m)) then begin 
          message, 'spher_harm(..., -m) != -1^m * conj(spher_harm(..., m))', /conti
          exit, status=1
        endif
      endif

    endforeach
  endforeach

end
