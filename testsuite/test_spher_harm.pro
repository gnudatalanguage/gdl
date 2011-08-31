;
; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
;
; Revised by Alain Coulais, 31-Aug-2011
;
; this one does not perform any sanity checks, and is intended
; only for testing the C++ implementation as done below
;
function SPHER_HARM_GDL, theta, phi, l, m
;
calcul1=SQRT((2.*l+1.)/4./!PI*FACTORIAL(l-ABS(m))/FACTORIAL(l+ABS(m)))
calcul2=LEGENDRE(COS(theta), l, ABS(m))*EXP(COMPLEX(0., m*phi))
return, calcul1*calcul2*(m lt 0 ? -1. : 1.)^m
;
end
;
; tests SPHER_HARM (and btw FACTORIAL and LEGENDRE)
;
pro test_spher_harm, test=test, help=help, debug=debug
;
;; testing output shape
if SIZE(spher_harm(0.,0.,0,0), /dimensions) ne 0 then begin
   MESSAGE, 'spher_harm(scalar, ...) should be scalar', /conti
   if ~KEYWORD_SET(debug) then EXIT, status=1
endif
if SIZE(spher_harm([0.],[0.],0,0), /dimensions) ne 1 then begin
   MESSAGE, 'spher_harm([scalar], ...) should be vector', /conti
   if ~KEYWORD_SET(debug) then EXIT, status=1
endif
if N_ELEMENTS(spher_harm([0.,0.,0.],0.,0,0)) ne 3 then begin
   MESSAGE, 'spher_harm(vector, scalar, ...) should be vector', /conti
   if ~KEYWORD_SET(debug) then EXIT, status=1
endif
if N_ELEMENTS(spher_harm(0.,[0.,0.,0.],0,0)) ne 3 then begin
   MESSAGE, 'spher_harm(scalar, vector, ...) should be vector', /conti
   if ~KEYWORD_SET(debug) then EXIT, status=1
endif
;;
;; testing output type
in = [ $
     ptr_new(1b), $
     ptr_new(1), $
     ptr_new(1l), $
     ptr_new(1.), $
     ptr_new(1d), $
     ptr_new(COMPLEX(1,1)), $
     ptr_new(DCOMPLEX(1,1)), $
     ptr_new([1b]), $
     ptr_new([1]), $
     ptr_new([1l]), $
     ptr_new([1.]), $
     ptr_new([1d]), $
     ptr_new([COMPLEX(1,1)]), $
     ptr_new([DCOMPLEX(1,1)]) $
     ]
;;
foreach i, in do begin
   if (SIZE(SPHER_HARM(*i, 0d, 0, 0), /type) ne 9) then begin
      MESSAGE, 'SPHER_HARM(*, double, ...) should be dcomplex'
      if ~KEYWORD_SET(debug) then EXIT, status=1
   endif
   if (SIZE(SPHER_HARM(*i, 0., 0, 0, /double), /type) ne 9) then begin
      MESSAGE, 'SPHER_HARM(*, *, ..., /double) should be dcomplex'
      if ~KEYWORD_SET(debug) then EXIT, status=1
   endif
   if SIZE(*i, /type) ne 9 && SIZE(*i, /type) ne 5 then begin
      if (SIZE(SPHER_HARM(*i, 0., 0, 0), /type) ne 6) then begin
         MESSAGE, 'SPHER_HARM(not double, not double, ...) should be complex'
         if ~KEYWORD_SET(debug) then EXIT, status=1
      endif
   endif
endforeach
;;
foreach i, in do ptr_free, i   
;
; testing output values against GDL-computed harmonics
thetas = FINDGEN(10.)
phis = DINDGEN(5.)
ls = INDGEN(2)
eps = 1e-6
foreach theta, thetas do begin
   foreach phi, phis do begin
      foreach l, ls do begin
         foreach m, INDGEN(2*l+1) - l do begin
            ;; scalar computations
            if ABS(SPHER_HARM(theta, phi, l, m) - SPHER_HARM_GDL(theta, phi, l, m)) gt eps then begin
               MESSAGE, 'spher_harm() != spher_harm_gdl()', /conti
               if ~KEYWORD_SET(debug) then EXIT, status=1
            endif      
            ;; vector computations
            if total(abs(spher_harm(thetas, phi, l, m) - spher_harm_gdl(thetas, phi, l, m))) gt eps then begin
               MESSAGE, 'spher_harm(vector, scalar, ...) != spher_harm_gdl(vector, scalar, ...)', /conti
               if ~KEYWORD_SET(debug) then EXIT, status=1
            endif
            if total(abs(spher_harm(theta, phis, l, m) - spher_harm_gdl(theta, phis, l, m))) gt eps then begin
               MESSAGE, 'spher_harm(scalar, vector, ...) != spher_harm_gdl(scalar, vector, ...)', /conti
               if ~KEYWORD_SET(debug) then EXIT, status=1
            endif
            if total(abs(spher_harm(theta, phis, l, m) - spher_harm_gdl(theta, phis, l, m))) gt eps then begin
               MESSAGE, 'spher_harm(vector, vector, ...) != spher_harm_gdl(vector, vector, ...)', /conti
               if ~KEYWORD_SET(debug) then EXIT, status=1
            endif
            
            ;; one more check for negative-m case
            if m lt 0 then begin
               if spher_harm(theta, phi, l, m) ne (-1)^(-m) * conj(spher_harm(theta, phi, l, -m)) then begin 
                  MESSAGE, 'spher_harm(..., -m) != -1^m * conj(spher_harm(..., m))', /conti
                  if ~KEYWORD_SET(debug) then EXIT, status=1
               endif
            endif
         endforeach
      endforeach
   endforeach
endforeach
;;
if KEYWORD_SET(test) then STOP
;;
end
