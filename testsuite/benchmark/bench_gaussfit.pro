;
; AC public version 2023/09/18
;
pro BENCH_GAUSSFIT, mpfit_version=mpfit_version, inputs=inputs, noise=noise, $
                    nb_iters=nb_iters, nbp=nbp, test=test, debug=debug
;
; we can switch between internal GSL or external MPFIT
; It should work with IDL as long as GAUSSFIT_MPFIT + MPFIT lin.
; are in the path (see in src/pro sub dirs. in GDL ...)
;
if KEYWORD_SET(mpfit_version) then begin
   print, 'Running MPFIT version'
endif else begin
   print, 'Internal GAUSFIT based on the GSL'
endelse
;
; loop iteration number
if ~KEYWORD_SET(nb_iters) then nb_iters=100
;
; noise level
if ~KEYWORD_SET(noise) then noise=0.2
;
; size of the input vector
;
if ~KEYWORD_SET(nbp) then n=100 else n=nbp
x = (FINDGEN(n)-(n/2))/4
;
; inputs parameters ... at least 3 elements !
;
if ~KEYWORD_SET(inputs) then begin
   a = [4.0, 1.0, 2.0, 1.0, 0.25, 0.01]
endif else begin
   nelts_inputs=SIZE(inputs,/n_elements)
   if (nelts_inputs LT 3) OR (nelts_inputs GT 6) then begin
      MESSAGE, 'input= should have >=3 and =< 6 elemenst'
   endif
   a=inputs
endelse
maxi=N_ELEMENTS(a)
;
; loop on the inputs
;
for ii=3, maxi do begin
   z = (x -a[1])/a[2]
   seed = 123321
   y = a[0]*exp(-z^2/2)
   if (ii GE 4) then y =y + a[3]
   if (ii GE 5) then y = y + a[4]*x
   if (ii GE 6) then y = y + a[5]*x^2
   tic
   for i=0,nb_iters-1 do begin
      bruit=noise*RANDOMN(seed, n)
      if KEYWORD_SET(mpfit_version) then begin
         yfit = GAUSSFIT_MPFIT(x, y+bruit, coeff, NTERMS=ii)
      endif else begin
         yfit = GAUSSFIT(x, y+bruit, coeff, NTERMS=ii)
      endelse
   endfor
   toc
   if KEYWORD_SET(debug) then begin
      PLOT, x, y+bruit
      OPLOT, x, yfit, thick=2, color=COLOR2COLOR(!color.red)
      wait, 1
   endif
endfor

if ~KEYWORD_SET(test) then STOP
end

