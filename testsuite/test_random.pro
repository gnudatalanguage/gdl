;
; Alain Coulais, 5 Janvier 2011, under GNU GPL v2 or later
;
; few tests can help to check whether the
; computations are in good range or not
;
; this code should be able to run on box without X11,
; and should be OK for Z, SVG and PS
;
; Modifications history :
; - 2017-12-13 : AC. ading TEST_RANDOM_POISSON, num. tests found.
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
pro PLOT_BATONS, values, offset=offset, color=color, psym=psym, $
                 linestyle=linestyle, _extra=_extra
;
if ~KEYWORD_SET(offset) then off=0 else off=offset
;
if ~KEYWORD_SET(psym) then mypsym=5 else mypsym=psym
;
for ii=0, N_ELEMENTS(values)-1 do begin
   PLOTS, [ii,ii]+off, [0, values[ii]], color=color, linestyle=linestyle
   PLOTS, ii+off, values[ii], color=color, psym=mypsym, _extra=_extra
endfor
;
end
;
; ---------------------------------------
; see figures in https://fr.wikipedia.org/wiki/Loi_de_Poisson
;
pro TEST_RANDOM_POISSON, errors, nb_points=nb_points, $
                         verbose=verbose, test=test
;
nb_pbs=0
;
if ((!d.name EQ 'X') or (!d.name EQ 'WIN')) then WINDOW, 0
;
DEVICE, get_decomposed=old_decomposed
if NOT(old_decomposed) then DEVICE, decomposed=1
;
if KEYWORD_SET(nb_points) then nbps=nb_points else nbps=100000
nbps_f=FLOAT(nbps)
;
if KEYWORD_SET(verbose) then print, 'We use : ', nbps, ' points'
;
res_l1=HISTOGRAM(RANDOMN(seed, nbps, poisson=1))/nbps_f
res_l4=HISTOGRAM(RANDOMN(seed, nbps, poisson=4))/nbps_f
res_l10=HISTOGRAM(RANDOMN(seed, nbps, poisson=10))/nbps_f
;
;indices=INDGEN(N_ELEMENTS(res_l10))
indices=20
xranges=[-0.5, MAX(indices)+0.5]
;
PLOT, INDGEN(N_ELEMENTS(res_l1)), res_l1, xrange=xranges, /xstyle, $
      xtitle='k', ytitle='P(X=k)', title='Poisson law', /nodata
;
PLOT_BATONS, res_l1, col='ff'x, psym=psym, line=2
PLOT_BATONS, res_l4, off=0.05, col='ffff'x, psym=psym, line=2
PLOT_BATONS, res_l10, off=0.1, col='ffffff'x, psym=psym, line=2
;
; Is the sum of all the values equal to One ?!
;
eps=(MACHAR()).eps
txt='Case Poisson Sum '
if (ABS(TOTAL(res_l1)-1.0) GT 5.*eps) then ADD_ERROR, nb_pbs, txt+'1'
if (ABS(TOTAL(res_l4)-1.0) GT 5.*eps) then ADD_ERROR, nb_pbs, txt+'1' 
if (ABS(TOTAL(res_l10)-1.0) GT 5.*eps) then ADD_ERROR, nb_pbs, txt+'1' 
;
; Are the Max in the expected range ?
; if nbps == 10000, you have 1 over 10 calls to fail
;
vals=[0.369090,0.196335,0.125015]
tol=0.01
txt='Case Poisson Max '
if (ABS(MAX(res_l1-vals[0])) GT tol) then ADD_ERROR, nb_pbs, txt+'1'
if (ABS(MAX(res_l4)-vals[1]) GT tol) then ADD_ERROR, nb_pbs, txt+'4'
if (ABS(MAX(res_l10)-vals[2]) GT tol) then ADD_ERROR, nb_pbs, txt+'10'
;
BANNER_FOR_TESTSUITE, "TEST_RANDOM_POISSON", nb_pbs, /short, verb=verbose
;
ERRORS_CUMUL, errors, nb_pbs
;
if KEYWORD_SET(test) then STOP
;
DEVICE, decomposed=old_decomposed
;
end
;
; ---------------------------------------
;
; note by AC 2017-12-13 : can be extend as "test_random_poison" above
; (but different numerical values !) no time to do it now :(
;
pro RANDOM_GAMMA, test=test
;
nbps=200000
nbps_f=FLOAT(nbps)
;
PLOT, HISTOGRAM(RANDOMN(seed, nbps, gamma=1), BINSIZE=0.1)/nbps_f
OPLOT, HISTOGRAM(RANDOMN(seed, nbps, gamma=2), BINSIZE=0.1)/nbps_f
OPLOT, HISTOGRAM(RANDOMN(seed, nbps, gamma=3), BINSIZE=0.1)/nbps_f
OPLOT, HISTOGRAM(RANDOMN(seed, nbps, gamma=4), BINSIZE=0.1)/nbps_f
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------
; we try to have the output for all the mode
;
pro RANDOM_ALL_GAMMA, verbose=verbose
;
init_device_mode=!d.name
;
list_device_mode=['NULL', 'PS', 'X', 'SVG', 'Z', 'WIN']
;
for ii=0, N_ELEMENTS(list_device_mode)-1 do begin
   ;;
   command='SET_PLOT, "'+list_device_mode[ii]+'"'
   test_mode=EXECUTE(command)
   ;;
   if KEYWORD_SET(verbose) then begin
      print, 'Testing mode '+list_device_mode[ii]+', status : ', test_mode
   endif
   if (test_mode EQ 0) then begin
      print, 'Testing mode '+list_device_mode[ii]+' : SKIPPED !'
      CONTINUE
   endif
   ;;
   if ((!d.name EQ 'X') or (!d.name EQ 'WIN')) then WINDOW, 1
   ;;
   if (!d.name EQ 'SVG') OR (!d.name EQ 'PS') then begin
      file='output_test_random_gamma.'+STRLOWCASE(!d.name)
      DEVICE, file=file
   endif
   ;;
   RANDOM_GAMMA
   ;;
   if (!d.name EQ 'SVG') OR (!d.name EQ 'PS') then begin
      DEVICE, /close
      if KEYWORD_SET(verbose) then print, 'file generated : <<'+file+'>>'
   endif
   print, 'Testing mode '+list_device_mode[ii]+', status : Processed'
endfor
;
; switching back to initial device mode (HELP, /device)
SET_PLOT, init_device_mode
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
pro TEST_RANDOM_BINOMIAL, cumul_errors, nbp=nbp, amplitude=amplitude, $
                          help=help, verbose=verbose, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_RANDOM_BINOMIAL, cumul_errors, nbp=nbp, amplitude=amplitude, $'
    print, '                          help=help, verbose=verbose, test=test'
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
errors=0
;
if KEYWORD_SET(verbose) then begin
   print, 'Runing TEST_RANDOM_BINOMIAL for amplitude : ', amplitude
   print, format='(6A12)', ['Amplitude', 'values', 'expected', 'Mean', 'disp.', 'Error']
endif
for ii=0, N_ELEMENTS(values)-1 do begin
   resu=RANDOMU(seed, nbp, BINOMIAL=[amplitude,values[ii]])
   dispersion=ABS(MEAN(resu)-amplitude*values[ii])
   if (dispersion GT amplitude/ratio) then begin
      txt='bad result for (amplitude, value) : ('+string(amplitude)+','+string(values)+')'
      ADD_ERRORS, errors, txt
   endif
   if KEYWORD_SET(verbose) then begin
      print, format='(i12,4f12,6x,I1.1)', amplitude, values[ii], $
             amplitude*values[ii], MEAN(resu), dispersion,  (dispersion GT amplitude/100.)
   endif
   ;;
endfor
;
BANNER_FOR_TESTSUITE, "TEST_RANDOM_BINOMIAL", errors, /short, verb=verbose
;
ERRORS_CUMUL, cumul_errors, errors
;
if KEYWORD_SET(test) then stop
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
cumul_errors=0
;
TEST_RANDOM_POISSON, cumul_errors, verbose=verbose
;
RANDOM_BASICS
;
; test the various DEVICE ...
RANDOM_ALL_GAMMA
;
TEST_RANDOM_BINOMIAL, cumul_errors, verbose=verbose, ampl=1.
TEST_RANDOM_BINOMIAL, cumul_errors, verbose=verbose, ampl=1.5
TEST_RANDOM_BINOMIAL, cumul_errors, verbose=verbose, ampl=10.
TEST_RANDOM_BINOMIAL, cumul_errors, verbose=verbose, ampl=100.
;
; ------------------------
;
BANNER_FOR_TESTSUITE, "TEST_RANDOM", cumul_errors
;
; if /debug OR /test nodes, we don't want to exit
if (cumul_errors GT 0) then begin
    if ~KEYWORD_SET(verbose) then MESSAGE, /continue, 're-run with /verbose for details'
    if ~(KEYWORD_SET(test) or KEYWORD_SET(no_exit)) then EXIT, status=1
endif
if KEYWORD_SET(no_exit) OR KEYWORD_SET(test) then STOP
;
end
