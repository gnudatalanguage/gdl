;
; Alain Coulais, 5 Janvier 2011, under GNU GPL v2 or later
;
; few tests can help to check whether the
; computations are in good range or not
;
; ---------------------------------------
; because regression was introduced between July 17 and October 2
pro RANDOM_BASICS
;
a=RANDOMU(seed)
a=RANDOMU(1)
;
a=RANDOMU(seed, 12)
a=RANDOMU(1,12)
;
a=RANDOMU(seed, [12,3])
a=RANDOMU(1, [12,3])
;
end
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
pro RANDOM_BINOMIAL, nbp=nbp, amplitude=amplitude, no_exit=no_exit, $
                     help=help, verbose=verbose, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro RANDOM_BINOMIAL, nbp=nbp, amplitude=amplitude, no_exit=no_exit, $'
    print, '                     help=help, verbose=verbose, test=test'
    return
endif
;
if ~KEYWORD_SET(nbp) then nbp=10000
if ~KEYWORD_SET(amplitude) then amplitude=10.
;
; Amplitude is a strictly posivite Integer
;
amplitude=FLOOR(amplitude)
if (amplitude EQ 1) then ratio=50. else ratio=100.
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
   if (dispersion GT amplitude/ratio) then error=error+1
   if KEYWORD_SET(verbose) then begin
      print, format='(i12,4f12,6x,I1.1)', amplitude, values[ii], $
             amplitude*values[ii], MEAN(resu), dispersion,  (dispersion GT amplitude/100.)
   endif
   ;;
endfor
;
if KEYWORD_SET(test) then stop
;
if (error GT 0) then begin
    MESSAGE, /continue, 'Error detected'
    stop
    if ~KEYWORD_SET(no_exit) then EXIT, status=1
endif else begin
    MESSAGE, /continue, 'success'
endelse
;
end
;
; ------------------------------------------
; extensions welcome
;
pro TEST_RANDOM, no_exit=no_exit, help=help, verbose=verbose, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_RANDOM, no_exit=no_exit, help=help, verbose=verbose, test=test'
endif
;
RANDOM_BASICS
;
RANDOM_GAMMA
;
RANDOM_BINOMIAL, verbose=verbose, test=test, no_exit=no_exit, ampl=1.
RANDOM_BINOMIAL, verbose=verbose, test=test, no_exit=no_exit, ampl=1.5
RANDOM_BINOMIAL, verbose=verbose, test=test, no_exit=no_exit, ampl=10.
RANDOM_BINOMIAL, verbose=verbose, test=test, no_exit=no_exit, ampl=100.
;
end

