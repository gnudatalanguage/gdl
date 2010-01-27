; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
pro test_zeropoly
  
  n = 4         ; number of test polynomials
  eps = 1e-6    ; a small value used for camparing floating point numbers
  c = ptrarr(n) ; polynomial coefficients
  r = ptrarr(n) ; polynomial roots

  ; IDL-doc example (IMSL_ZEROPOLY)
  c[0] = ptr_new([-2, 4, -3, 1])
  r[0] = ptr_new([1, complex(1,-1), complex(1,1)])

  ; GSL-doc example 
  c[1] = ptr_new([-1, 0, 0, 0, 0, 1])
  r[1] = ptr_new(exp([0,2,4,6,8] * !PI * complex(0,1) / 5))
  
  ; IDL-doc examples (FZ_ROOTS)
  c[2] = ptr_new([-2, -9, -7, 6])
  r[2] = ptr_new([-.5, -1./3, 2.])

  c[3] = ptr_new([2, 0, 3, 0, 1])
  r[3] = ptr_new(complex(0,[-sqrt(2), sqrt(2), -1, 1]))

  ; trying with different input and output types for all test polynomials
  for double_kw = 0, 1 do for i = 0, n - 1 do for t = 2, 15 do begin
    if (t ge 6 && t le 11) || t eq 12 || t eq 13 || t eq 15 then continue
    z = imsl_zeropoly(fix(*c[i], type=t), double=double_kw)
    nz = n_elements(z) 
    if nz ne n_elements(*r[i]) then message, 'TOTAL FAILURE!'
    ; checking the results (which might be ordered differently)
    for jz = 0, nz - 1 do for jr = 0, nz - 1 do begin
      if abs(z[jz] - (*r[i])[jr]) lt eps then begin
        z[jz] = complex(!VALUES.F_NAN, !VALUES.F_NAN)
        continue
      endif
    endfor
    wh = where(finite(z), cnt)
    if cnt ne 0 then message, 'FAILED for test ' + string(i)
  endfor

end
