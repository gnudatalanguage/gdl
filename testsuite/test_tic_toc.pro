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
;
cumul=TIC('cumul')
;
; how many time we compute ...
if ~KEYWORD_SET(nb_times) then nb_times=4
val_times=DBLARR(nb_times)
;
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
; We accepte a tolerance of 1 % (ok on various Linux, might be wrong
; on OSX [cf "test_wait"])
tolerance=0.01
if (ABS((TOTAL(val_times)-val_cumul)/val_cumul) GT tolerance) then errors=1
;
BANNER_FOR_TESTSUITE, 'TEST_TIC_TOC', errors, short=short
;
if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

