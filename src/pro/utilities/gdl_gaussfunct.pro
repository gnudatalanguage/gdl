; evaluates the sum of a Gaussian and a second-order polynomial. see
; funct.pro and CURVEFIT.
pro gdl_gaussfunct,x,a,f,pder
  ; no check on arguments validity!
  ON_ERROR,2
  COMPILE_OPT IDL2, hidden
  na=n_elements(a)
  if a[2] ne 0.0 then z = (x-a[1])/a[2] else z= 10.
  ez = exp(-z^2/2.)*(abs(z) le 7.)
  f = a[0]*ez 
  case na of
     4:f=f+a[3]
     5:f=f+a[3]+a[4]*x
     6:f=f+a[3]+a[4]*x+a[5]*x^2
     else:
  endcase
  if n_params(0) le 3 then return
  pder = fltarr(n_elements(x),na)
  pder[0,0] = ez
  if a[2] ne 0 then pder[0,1] = a[0] * ez * z/a[2]
  pder[0,2] = pder[*,1] * z
  if na eq 3 then return
  pder[*,3] = 1.
  if na eq 4 then return
  pder[0,4] = x
  if na eq 5 then return
  pder[0,5] = x^2
  return
END
