;
; AC, 2016-03-03
;
; basic test to check whether TIC and TOC are working ...
; testing "label" and sum ...
;
pro TEST_TIC_TOC, nb_times=nb_times, no_exit=no_exit, $
                  help=help, test=test, verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_TIC_TOC, nb_times=nb_times, no_exit=no_exit, $'
    print, '                  help=help, test=test, verbose=verbose'
    return
endif
;
fft_dim=1729
; **************IMPORTANT ***************
; make all procedures used in the loop below known, compiled and run before starting the test.
;
TIC & TOC & r = FFT(RANDOMU(seed, fft_dim, fft_dim)) ; this may take time the firsttime they are called.
; the variance test will not work if these are not already executed once.
; how many time we compute ...
if ~KEYWORD_SET(nb_times) then nb_times=4
val_times=DBLARR(nb_times)
;
cumul=TIC('cumul') ;; need to be just before loop
for i=0, nb_times-1 do begin
    ;; Start another clock named FFT
    ;; combined with the iteration number
    clock = TIC('FFT' + STRTRIM(i, 2))
    r = FFT(RANDOMU(seed, fft_dim, fft_dim))
    ;; For each iteration grab the end system time, 
    ;; then calculate and and print out the elapsed time 
    val_times[i]=TOC(clock)
    print, 'Time elapsed in loop '+STRING(i,'(i2)')+' : ', $
           val_times[i], ' seconds.'
endfor
;
; Grab the final system  for the entire loop
; and calculate and print the total elapsed time
;
val_cumul=TOC(cumul)
val_cumul=val_cumul[0]
;
print, 'Cumulative Time elapsed : ', TOTAL(val_times), ' seconds.'
print, 'Total Time elapsed      : ', val_cumul, ' seconds.'
;
errors=0
;
; We accepte a tolerance of 0.5 on the variance of measurements.
tolerance=0.5
;
m=moment(val_times)
precision=m[1]/m[0]
if (precision GT tolerance) then errors=1
print, 'Time Precision          : ', precision, '.'
;
BANNER_FOR_TESTSUITE, 'TEST_TIC_TOC', errors, short=short
;
if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

