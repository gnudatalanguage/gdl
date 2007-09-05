;
; AC, June 2007
; a bug for subselection was corrected on 05 Sept 07
;
; please run the other procedure "TEST_PLOT_INFO" to have informations ...
;
; benchmark of plot, with and without NaN and Inf
;
; ------------------------------------
; scalable plot (without NaN or Inf)
;
; This program may evoluate to manage different kind of curves
; From current tests, RandomU is the worst case
;
pro TEST_PLOT_BENCH_ONE, psym=psym, linestyle=linestyle, nbp=nbp
;
if (N_ELEMENTS(nbp) EQ 0) then begin 
    nbp=1e6
    print, 'Default number of points is:', nbp
endif
;
x=RANDOMU(seed,nbp)
t0=SYSTIME(1)
plot,x, psym=psym, linestyle=linestyle
t1=SYSTIME(1)-t0
print, 'total time:', t1
;
end
;
; ----------------------------------------
; similar to TEST_PLOT_BENCH_ONE but
; loop at different scales (10^n, from n=1 to n=6) 
;
pro TEST_PLOT_BENCHMARK, test=test, all=all, power=power, help=help
;
if (N_ELEMENTS(test) EQ 0) then test=0
test=ROUND(test)
if (test GT 4) OR (test LT 0) then test=0
if (N_ELEMENTS(power) EQ 0) then power=6
if KEYWORD_SET(all) then test=-1
;
if (test EQ 0) OR KEYWORD_SET(help) then begin
    print, 'TEST_PLOT_BENCHMARK, test=test, all=all, power=power, help=help'
    print, '   if keyword test=1, then findgen only'
    print, '   if keyword test=2, then RandomU only'
    print, '   if keyword test=3, then RandomN only'
    print, '   if keyword test=4, then Findgen containing 10 or 100 <<Nan>> !'
    print, ' if /all, all tests (1,2, 3 and 4)'
    stop
    return
endif
;
!p.multi=[0,2,0]
window, xsize=800, ysize=400
if KEYWORD_SET(all) OR (TEST EQ 1) then begin
    print, 'Findgen'
    for i=1,power do begin
        x=FINDGEN(10.^i)
        t0=SYSTIME(1)
        plot,x
        t1=SYSTIME(1)-t0
        t0=SYSTIME(1)
        plot, MAX(x)-x
        t2=SYSTIME(1)-t0   
        print, 'i, total time:', i, t1, t2
    endfor
endif
;
if KEYWORD_SET(all) OR (TEST EQ 2) then begin
    print, 'RandomU'
    for i=1,power do begin
        x=RANDOMU(seed,10.^i)
        t0=SYSTIME(1)
        plot,x
        t1=SYSTIME(1)-t0
        t0=SYSTIME(1)
        plot, x, x, title='X versus X: line'
        t2=SYSTIME(1)-t0   
        print, 'i, total time:', i, t1, t2
    endfor
endif
;
if KEYWORD_SET(all) OR (TEST EQ 3) then begin
    print, 'RandomN'
    for i=1,power do begin
        x=RANDOMN(seed,10.^i)
        t0=SYSTIME(1)
        plot,x
        t1=SYSTIME(1)-t0
        t0=SYSTIME(1)
        plot, x, x, title='X versus X: line'
        t2=SYSTIME(1)-t0   
        print, 'i, total time:', i, t1, t2
    endfor
endif
;
if KEYWORD_SET(all) OR (TEST EQ 4) then begin
    print, 'Findgen + 10 or 110 NaN at Random index'
    for i=2,power do begin
        x=FINDGEN(10.^i)
        y1=10.^i*RANDOMU(seed,10)
        y2=10.^i*RANDOMU(seed,100)
        x[y1]=!values.f_nan
        t0=SYSTIME(1)
        plot,x, title='10'
        t1=SYSTIME(1)-t0
        x[y2]=!values.f_nan
        t0=SYSTIME(1)
        plot, x, title='110'
        t2=SYSTIME(1)-t0   
        print, 'i, total time:', i, t1, t2
    endfor
endif
;
!p.multi=0
;
if KEYWORD_SET(test) then stop
;
end


