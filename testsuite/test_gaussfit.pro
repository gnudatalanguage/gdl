;
; AC 2023-11-10
;
; Basic tests for GAUSSFIT. Please repor any idea to extend
; these tests, which are very important in the domains
; where IDL/GDL are used
; -------------------------------------------------------------
; Modifications history :
;
; - 2023-11-14 : AC. I forgot than the steps between X points
; may not be equal to unity. I add few tests for that.
;
; -------------------------------------------------------------
;
function THE_GAUSS_FUNCTION, params, x=x, nbp=nbp, first_x=first_x, $
                             scale=scale, debug=debug, test=test, show=show
;
; maybe the logic is not obvious, but 
;
if ~KEYWORD_SET(npb) then nbp=100
if ~ISA(first_x) then first_x=-nbp/2
if ~KEYWORD_SET(scale) then scale=1
;
if N_ELEMENTS(params) LT 3 then MESSAGE, 'At least 3 element in PARAMS'
if N_ELEMENTS(params) GT 6 then MESSAGE, 'At max 6 element in PARAMS'
;
a=params
x = first_x+scale*FINDGEN(nbp)
z = (x -a[1])/a[2]
y = a[0]*exp(-z^2/2.)
if N_ELEMENTS(a) GT 3 then y=y+a[3]
if N_ELEMENTS(a) GT 4 then y=y+a[4]*x
if N_ELEMENTS(a) EQ 6 then y=y+a[5]*x*x
;
if KEYWORD_SET(show) then PLOT, x, y
if KEYWORD_SET(debug) then begin
   print, 'Gauss params : ', params
   print, 'First, Scale : ', first_x, scale
   print, 'Min/max X : ', min(x), max(x)
endif
;
if KEYWORD_SET(test) then STOP
;
return, y
end
;
; -------------------------------------------------------------
;
function CRITERE, a, b, lenght, verbose=verbose
;
if ~ISA(lenght) then begin
   res=(SQRT(TOTAL((a-b)^2)))
endif else begin
   xxx=(a[0:lenght-1]-b[0:lenght-1])^2
   res=SQRT(TOTAL(xxx))
endelse
;
if KEYWORD_SET(verbose) then print, 'ESP : ', res
;
return, res
;
end
;
; -------------------------------------------------------------
;
pro TEST_GAUSSFIT_SCALE, cumul_errors, scale=scale, $
                         test=test, verbose=verbose
;
nb_errors=0
;
if KEYWORD_SET(verbose) then v=1
;
; this is the way to test with various steps in X
if ~KEYWORD_SET(scale) then scale=1
;
;  Only 3 params, the 3 last ones should stay close to zero
;
eps3=1e-3
par_input=[10, 1, 3, 0, 0, 0]
y=THE_GAUSS_FUNCTION(par_input, x=x, scale=scale)
;
for ii=3, 6 do begin
   yfit=GAUSSFIT(x,y, estim, nterms=ii)
   if (CRITERE(par_input,estim,ii,v=v) GT eps3) then $
      ERRORS_ADD, nb_errors, 'c3 nt'+STRING(ii)
endfor
;
eps3=0.004
par_input=[10, 1, 3, -21.3, 0, 0]
y=THE_GAUSS_FUNCTION(par_input, x=x, scale=scale, first=-5)
;
for ii=4, 6 do begin
   yfit=GAUSSFIT(x,y, estim, nterms=ii)
   if (CRITERE(par_input,estim,ii,v=v) GT eps3) then $
      ERRORS_ADD, nb_errors, 'c4 nt'+STRING(ii)
endfor
;
; same than previous, but negative peak
eps3=0.004
par_input=[-53, 1, 3, -21.3, 0, 0]
y=THE_GAUSS_FUNCTION(par_input, x=x, scale=scale, first=-7)
;
for ii=4, 6 do begin
   yfit=GAUSSFIT(x,y, estim, nterms=ii)
   if (CRITERE(par_input,estim,ii,v=v) GT eps3) then $
      ERRORS_ADD, nb_errors, 'c4 neg nt'+STRING(ii)
endfor
;
; ----- final ----
;
txt=' (scale ='+STRING(format='(f5.2)', scale)+')'
BANNER_FOR_TESTSUITE, 'TEST_GAUSSFIT_SCALE'+txt, nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------------------------
;
pro TEST_GAUSSFIT_INIT, cumul_errors, test=test, verbose=verbose, scale=scale
;
nb_errors=0
;
if KEYWORD_SET(verbose) then v=1
;
; this is the way to test with various steps in X
if ~KEYWORD_SET(scale) then scale=1
;
;  Only 3 params, the 3 last ones should stay close to zero
;
eps3=4e-5
if scale LT 1 then eps3=2e-3
par_input=[10, 10, 1, 0, 0, 0]
y=THE_GAUSS_FUNCTION(par_input, x=x, scale=scale)
;
yfit=GAUSSFIT(x,y, estim)
if (CRITERE(par_input,estim,v=v) GT eps3) then ERRORS_ADD, nb_errors, 'c3'
;
yfit=GAUSSFIT(x,y, estim, nterms=3)
if (CRITERE(par_input,estim,3,v=v) GT eps3) then ERRORS_ADD, nb_errors, 'c3 nt3'
;
yfit=GAUSSFIT(x,y, estim, nterms=4)
if (CRITERE(par_input,estim,4,v=v) GT eps3) then ERRORS_ADD, nb_errors, 'c3 nt4'
;
yfit=GAUSSFIT(x,y, estim, nterms=5)
if (CRITERE(par_input,estim,5,v=v) GT eps3) then ERRORS_ADD, nb_errors, 'c3 nt5'
;
yfit=GAUSSFIT(x,y, estim, nterms=6)
if (CRITERE(par_input,estim,6,v=v) GT eps3) then ERRORS_ADD, nb_errors, 'c3 nt6'
;
;  Now 4 params, the 2 last ones should stay close to zero
;
eps4=1e-2
par_input=[10, 30, 1,50, 0, 0]
y=THE_GAUSS_FUNCTION(par_input, x=x, scale=1)
;
yfit=GAUSSFIT(x,y, estim)
if (CRITERE(par_input,estim,v=v) GT eps4) then ERRORS_ADD, nb_errors, 'c4'
;
yfit=GAUSSFIT(x,y, estim, nterms=4)
if (CRITERE(par_input,estim,4,v=v) GT eps4) then ERRORS_ADD, nb_errors, 'c4 nt4'
;
yfit=GAUSSFIT(x,y, estim, nterms=5)
if (CRITERE(par_input,estim,5,v=v) GT eps4) then ERRORS_ADD, nb_errors, 'c4 nt5'
;
yfit=GAUSSFIT(x,y, estim, nterms=6)
if (CRITERE(par_input,estim,6,v=v) GT eps4) then ERRORS_ADD, nb_errors, 'c4 nt6'
;
;  Now 5 params, the last one should stay close to zero
;
eps5=1e-2
par_input=[10, 30, 1,50, -4, 0]
y=THE_GAUSS_FUNCTION(par_input, x=x, scale=1)
;
yfit=GAUSSFIT(x,y, estim)
if (CRITERE(par_input,estim,v=v) GT eps5) then ERRORS_ADD, nb_errors, 'c5'
;
yfit=GAUSSFIT(x,y, estim, nterms=5)
if (CRITERE(par_input,estim,5,v=v) GT eps5) then ERRORS_ADD, nb_errors, 'c5 nt5'
;
yfit=GAUSSFIT(x,y, estim, nterms=6)
if (CRITERE(par_input,estim,6,v=v) GT eps5) then ERRORS_ADD, nb_errors, 'c5 nt6'
;
;  Now 6 params
;
eps6=1e-2
par_input=[10, 30, 1,50, -4, .0005]
y=THE_GAUSS_FUNCTION(par_input, x=x, scale=1)
;
yfit=GAUSSFIT(x,y, estim)
if (CRITERE(par_input,estim,v=v) GT eps5) then ERRORS_ADD, nb_errors, 'c6'
;
yfit=GAUSSFIT(x,y, estim, nterms=6)
if (CRITERE(par_input,estim,6,v=v) GT eps5) then ERRORS_ADD, nb_errors, 'c6 nt6'
;
; ----- final ----
;
txt=' (scale ='+STRING(format='(f5.2)', scale)+')'
BANNER_FOR_TESTSUITE, 'TEST_GAUSSFIT_INIT'+txt, nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------------------------
;
function MASQUE, indice
res=REPLICATE(1.,6)
if indice LT 6 then res[indice:*]=0
return, res
end
; -------------------------------------------------------------
;
pro TEST_GAUSSFIT_BASIC, cumul_errors, test=test, verbose=verbose, debug=debug
;
nb_errors=0
;
if KEYWORD_SET(verbose) then v=1
;
par_ref=[10, 45.25, 3, .5, .02, .002]
eps_ref=0.0005
eps6=0.002
;
for kk=0, 2 do begin
   if KEYWORD_SET(debug) then print, 'kk=', kk
   par_input=par_ref
   if kk EQ 1 then par_input[0]=(-1)*par_input[0]
   if kk EQ 2 then par_input[2]=2*par_input[2]
   ;;
   for ii=3, 6 do begin
      if ii EQ 6 then eps=eps6 else eps=eps_ref
      ;;
      para=par_input[0:ii-1]
      y=THE_GAUSS_FUNCTION(para, x=x, scale=1, first=1)
      ;;
      yfit=GAUSSFIT(x, y, estim)
      fullpara=par_input*MASQUE(ii)
      if (CRITERE(fullpara, estim,v=v) GT eps) then $
         ERRORS_ADD, nb_errors, 'a'+STRCOMPRESS(string(ii))
      if KEYWORD_SET(debug) then print, '6 ', fullpara, estim
      ;;
      yfit=GAUSSFIT(x, y, estim, nterms=ii)
      if (CRITERE(para,estim,v=v) GT eps) then $
         ERRORS_ADD, nb_errors, 'nt'+STRCOMPRESS(string(ii))
      if KEYWORD_SET(debug) then print, 'nt ', para, estim
   endfor
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_GAUSSFIT_BASIC', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------------------------
;
pro TEST_GAUSSFIT_NOISE, cumul_errors, test=test, verbose=verbose, debug=debug
;
nb_errors=0
if ~KEYWORD_SET(debug) then debug=0
;
if KEYWORD_SET(verbose) then v=1
;
par_input=[10, 45.25, 3, .5, .02, .002]
y=THE_GAUSS_FUNCTION(par_input, x=x, scale=1, first=1)
;
noise_u=RANDOMU(123, N_ELEMENTS(y))
noise_n=RANDOMU(456, N_ELEMENTS(y))

eps=.7
err_u=1.5*STDDEV(noise_u)
err_n=1.5*STDDEV(noise_n)
;
for ii=3, 6 do begin
   para=par_input[0:ii-1]
   y=THE_GAUSS_FUNCTION(para, x=x, scale=1, first=1)
   ;;
   yfit=GAUSSFIT(x, y+noise_u, estim)
   fullpara=par_input*MASQUE(ii)
   if (CRITERE(fullpara, estim,v=v) GT eps) then $
      ERRORS_ADD, nb_errors, 'a'+STRCOMPRESS(string(ii))
   if debug then print, '6 ', fullpara, estim
   if (STDDEV(yfit-y-noise_u) GT err_u) then ERRORS_ADD, nb_errors, 'b1 u'
   ;;
   yfit=GAUSSFIT(x, y+noise_u, estim, nterms=ii)
   if (CRITERE(para,estim,v=v) GT eps) then $
      ERRORS_ADD, nb_errors, 'nt'+STRCOMPRESS(string(ii))
   if debug then print, 'nt ', para, estim
   if (STDDEV(yfit-y-noise_u) GT err_u) then ERRORS_ADD, nb_errors, 'b2 u'
   ;;
   yfit=GAUSSFIT(x, y+noise_n, estim)
   fullpara=par_input*MASQUE(ii)
   if (CRITERE(fullpara, estim,v=v) GT eps) then $
      ERRORS_ADD, nb_errors, 'a'+STRCOMPRESS(string(ii))
   if debug then print, '6 ', fullpara, estim
   if (STDDEV(yfit-y-noise_n) GT err_n) then ERRORS_ADD, nb_errors, 'b3 n'
   ;;
   yfit=GAUSSFIT(x, y+noise_n, estim, nterms=ii)
   if (CRITERE(para,estim,v=v) GT eps) then $
      ERRORS_ADD, nb_errors, 'nt'+STRCOMPRESS(string(ii))
   if debug then print, 'nt ', para, estim
   if (STDDEV(yfit-y-noise_n) GT err_n) then ERRORS_ADD, nb_errors, 'b4 n'
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_GAUSSFIT_NOISE', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ----------------------------------
;
pro TEST_GAUSSFIT, help=help, test=test, verbose=verbose, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_GAUSSFIT, help=help, test=test, verbose=verbose, no_exit=no_exit'
   return
endif 
;
cumul_errors=0
;
TEST_GAUSSFIT_BASIC, cumul_errors
;
TEST_GAUSSFIT_INIT, cumul_errors
TEST_GAUSSFIT_INIT, cumul_errors, scale=1.2
TEST_GAUSSFIT_INIT, cumul_errors, scale=0.82
;
TEST_GAUSSFIT_SCALE, cumul_errors
TEST_GAUSSFIT_SCALE, cumul_errors, sca=.75
TEST_GAUSSFIT_SCALE, cumul_errors, sca=1.5
;
TEST_GAUSSFIT_NOISE, cumul_errors
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_GAUSSFIT', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;

end
