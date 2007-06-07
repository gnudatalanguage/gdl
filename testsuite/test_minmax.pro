;
; AC 2007-june-01
; 
; some tests (only on "float" type) for Min/Max
;
pro TEST_MINMAX, nan=nan, inf=inf, nbp=nbp, $
                 position=position, verbose=verbose
;
if N_ELEMENTS(nbp) EQ 0 then nbp=10
;
problem=0.
if KEYWORD_SET(nan) then problem=!values.f_nan
if KEYWORD_SET(inf) then problem=!values.f_infinity
if (problem EQ 0.) then begin
    print, 'Must select /Nan or /Inf'
    return
endif
;
if N_ELEMENTS(position) EQ 0 then position=4
;
if position GT nbp-1 then begin
    print, 'POSITION must be below NBP !'
    return
endif
;
if KEYWORD_SET(verbose) then begin
    print, 'Type of Problem: ', problem
    print, 'Position: ', position
endif
;
a=findgen(nbp)
a[position]=problem
if KEYWORD_SET(verbose) then print, a
;
print, 'Type of problem:', STRING(problem)
print, 'MIN         :', MIN(a)
print, 'MIN and /NAN:', MIN(a,/nan)
print, 'MAX         :', MAX(a)
print, 'MAX and /NAN:', MAX(a,/nan)
print, '--------------'
;
end
;
;
pro MINMAX_CROSS, nan=nan, inf=inf, negatif=negatif, nbp=nbp, $
                  position=position, verbose=verbose
;
if N_ELEMENTS(nbp) EQ 0 then nbp=10
;
problem=0.
if KEYWORD_SET(nan) then problem=!values.f_nan
if KEYWORD_SET(inf) then problem=!values.f_infinity
;
if KEYWORD_SET(negatif) then problem=-problem
;
if (problem EQ 0.) then begin
    print, 'Must select /Nan or /Inf'
    return
endif
;
if N_ELEMENTS(position) EQ 0 then position=4
;
if position GT nbp-1 then begin
    print, 'POSITION must be below NBP !'
    return
endif
;
if KEYWORD_SET(verbose) then begin
    print, 'Type of Problem: ', problem
    print, 'Position: ', position
endif
;
;
a=findgen(nbp)
a[position]=problem
if KEYWORD_SET(verbose) then print, a
;
print, 'Type of problem:', STRING(problem)
print, 'Number of points :+', string(nbp)+', Position of problem: ', string(position)
;
mini=MIN(a, max=tyty)      & print, 'MIN         :', mini, tyty
mini=MIN(a, max=tyty,/nan) & print, 'MIN and /NAN:', mini, tyty
maxi=MAX(a, min=tyty)      & print, 'MAX         :', maxi, tyty
maxi=MAX(a, min=tyty,/nan) & print, 'MAX and /NAN:', maxi, tyty
print, '--------------'
;
end
;
;
;
pro MULTI_TEST_MINMAX1
;
TEST_MINMAX, /nan, nbp=10, pos=4
TEST_MINMAX, /nan, nbp=10, pos=9
TEST_MINMAX, /nan, nbp=10, pos=0
;
TEST_MINMAX, /inf, nbp=10, pos=4
TEST_MINMAX, /inf, nbp=10, pos=9
TEST_MINMAX, /inf, nbp=10, pos=0
;
end

pro MULTI_TEST_MINMAX2
;
MINMAX_CROSS, /nan, nbp=10, pos=4
MINMAX_CROSS, /nan, nbp=10, pos=9
MINMAX_CROSS, /nan, nbp=10, pos=0
;
MINMAX_CROSS, /inf, nbp=10, pos=4
MINMAX_CROSS, /inf, nbp=10, pos=9
MINMAX_CROSS, /inf, nbp=10, pos=0
;
end

pro big_test_minmax, nbp=nbp, test=test, verbose=verbose
;
if N_ELEMENTS(nbp) EQ 0 then nbp=1e5
;
a=RANDOMN(seed,nbp)
b=a
;
t0=SYSTIME(1)
if KEYWORD_SET(verbose) then print, MIN(b)
print, 'no Nan included', SYSTIME(1)-t0
t0=SYSTIME(1)
if KEYWORD_SET(verbose) then print, MIN(b,/nan)
print, 'flag /Nan included, no Nan/inf data', SYSTIME(1)-t0
;
print, 'now with Nan/inf'
b=a
ii=ROUND(RANDOMU(seed,10)*nbp)
b[ii]=!values.f_nan
t0=SYSTIME(1)
if KEYWORD_SET(verbose) then print, MIN(b)
print, 'Nan included, no flag', SYSTIME(1)-t0
t0=SYSTIME(1)
if KEYWORD_SET(verbose) then print, MIN(b,/nan)
print, 'Nan included, flag', SYSTIME(1)-t0
;
b[ii]=!values.f_infinity
t0=SYSTIME(1)
if KEYWORD_SET(verbose) then print, MIN(b)
print, 'Inf included, no flag', SYSTIME(1)-t0
t0=SYSTIME(1)
if KEYWORD_SET(verbose) then print, MIN(b,/nan)
print, format='(a25,f)', 'Inf included, flag', SYSTIME(1)-t0
;
if KEYWORD_SET(test) then STOP

end

pro MINMAX_INF_NAN, nan=nan, prefixe=prefixe, test=test
;
if NOT(KEYWORD_SET(nan)) then nan=0
;
a=replicate(!values.f_nan,10)
;
if KEYWORD_SET(prefixe) then a=[0.,a]
;
print, 'Only NaN:', min(a, nan=nan), max(a, nan=nan)  
;
a[5]=!values.f_infinity
a[6]=-a[5]
print, 'NaN with +- Inf:', min(a, nan=nan), max(a, nan=nan)  
;
b=replicate(!values.f_infinity,10)
if KEYWORD_SET(prefixe) then b=[0.,b]
;
print, 'Only +Inf:', min(b, nan=nan), max(b, nan=nan)  
;
b[6]=-b[5]
print, '+/- Inf :', min(b, nan=nan), max(b, nan=nan)
;
b[5]=!values.f_nan
print, '+/- Inf and NaN: ', min(b, nan=nan), max(b, nan=nan)

if KEYWORD_SET(test) then stop

end


