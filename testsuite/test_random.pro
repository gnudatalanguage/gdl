;
; Alain Coulais, 5 Janvier 2011, under GNU GPL v2 or later
;
; few tests can help to check whether the
; computations are in good range or not
;
; ---------------------------------------
;
pro RANDOM_GAMMA
;
window, 1
;
PLOT, HISTOGRAM(RANDOMN(SEED, 20000, gamma=1), BINSIZE=0.1)
OPLOT, HISTOGRAM(RANDOMN(SEED, 20000, gamma=2), BINSIZE=0.1)
OPLOT, HISTOGRAM(RANDOMN(SEED, 20000, gamma=3), BINSIZE=0.1)
OPLOT, HISTOGRAM(RANDOMN(SEED, 20000, gamma=4), BINSIZE=0.1)
;
end
;
; ------------------------------------------
;
; Idea: when the number is big enough, mean value
; of the realization should be close to the "value".
; If computation is wrong (e.g. calling bad noise, algo),
; we can expect not to have goor prediction ;-)
; (and this test fails "often" when NPB =< 100)
;
pro RANDOM_BINOMIAL, nbp=nbp, amplitude=amplitude, $
                     verbose=verbose, test=test
;
if ~KEYWORD_SET(nbp) then nbp=10000
if ~KEYWORD_SET(amplitude) then amplitude=10.
;
values=[0.10,0.25,0.50,0.75,0.90]
error=0
;
if KEYWORD_SET(verbose) then begin
   print, format='(6A12)', ['Amplitude', 'values', 'expected', 'Mean', 'disp.', 'Error']
endif
for ii=0, N_ELEMENTS(values)-1 do begin
   resu=RANDOMU(seed, nbp, BINOMIAL=[amplitude,values[ii]])
   dispersion=ABS(MEAN(resu)-amplitude*values[ii])
   if (dispersion GT amplitude/100.) then error=error+1
   if KEYWORD_SET(verbose) then begin
      print, format='(5f12,6x,I1.1)', amplitude, values[ii], $
             amplitude*values[ii], MEAN(resu), dispersion,  (dispersion GT amplitude/100.)
   endif
   ;;
endfor
;
if KEYWORD_SET(test) then stop
;
if (error GT 0) then EXIT, status=1
;
end
;
; ------------------------------------------
; extensions welcome
;
pro TEST_RANDOM
;
RANDOM_GAMMA
RANDOM_BINOMIAL, ampl=1.
RANDOM_BINOMIAL, ampl=10.
RANDOM_BINOMIAL, ampl=100.
;
end

