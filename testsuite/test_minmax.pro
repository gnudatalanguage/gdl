;
; AC June 2007
; 
; Purpose: a collection for testing Inf and Nan managing 
; in Min/Max procedures (up to now, Inf was not managed,
; NaN was not well managed if present in first place)
;
; You can call procedure "TEST_MINMAX" which call different subroutines
;
pro MINMAX_BASIC, nan=nan, inf=inf, nbp=nbp, $
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
; ----------------------------
; this procedure checks Nan and Inf when we would
; like or not to return Max when calling Min and reverse
; (because Branches in soft are differents)
;
pro MINMAX_CROSS, nan=nan, inf=inf, nbp=nbp, $
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
;
a=findgen(nbp)
a[position]=problem
if KEYWORD_SET(verbose) then print, a
;
print, 'Type of problem:', STRING(problem)
;
mini=MIN(a, max=tyty)      & print, 'MIN         :', mini, tyty
mini=MIN(a, max=tyty,/nan) & print, 'MIN and /NAN:', mini, tyty
maxi=MAX(a, min=tyty)      & print, 'MAX         :', maxi, tyty
maxi=MAX(a, min=tyty,/nan) & print, 'MAX and /NAN:', maxi, tyty
print, '--------------'
;
end
;
; ------------------------------------------------
; calling differents cases (position of data to be flagged)
; and returning only Min OR Max
;
pro MULTI_BASIC_MINMAX
;
MINMAX_BASIC, /nan, nbp=10, pos=4
MINMAX_BASIC, /nan, nbp=10, pos=9
MINMAX_BASIC, /nan, nbp=10, pos=0
;
MINMAX_BASIC, /inf, nbp=10, pos=4
MINMAX_BASIC, /inf, nbp=10, pos=9
MINMAX_BASIC, /inf, nbp=10, pos=0
;
end
;
; ------------------------------------------------
; calling differents cases (position of data to be flagged)
; and returning BOTH Min AND Max
;
pro MULTI_CROSS_MINMAX
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
;
; -----------------------------------
; basic becnhmark test for Min/Max when NaN and Inf are
; present (or not)
;
pro BENCH_TEST_MINMAX, nbp=nbp, test=test
;
a=RANDOMN(seed,nbp)
b=a
;
print, 'First column should alway have the same value'
;
t0=SYSTIME(1)
print, 'result: ', MIN(b), ' CASE: no Nan included', SYSTIME(1)-t0
t0=SYSTIME(1)
print, 'result: ', MIN(b,/nan), ' CASE: flag /Nan included, no Nan/inf data', SYSTIME(1)-t0
;
print, 'now with Nan/inf'
b=a
ii=ROUND(RANDOMU(seed,10)*nbp)
b[ii]=!values.f_nan
t0=SYSTIME(1)
print, 'result: ', MIN(b), ' CASE: Nan included, no flag', SYSTIME(1)-t0
t0=SYSTIME(1)
print, 'result: ', MIN(b,/nan), ' CASE: Nan included, flag', SYSTIME(1)-t0
;
b[ii]=!values.f_infinity
t0=SYSTIME(1)
print, 'result: ', MIN(b), ' CASE: Inf included, no flag', SYSTIME(1)-t0
t0=SYSTIME(1)
print, 'result: ', MIN(b,/nan), ' CASE: Inf included, flag', SYSTIME(1)-t0
;
if KEYWORD_SET(test) then STOP
;
end
;
; calling all tests
;
pro TEST_MINMAX
;
;
print, '' & print, 'Basic tests'
MULTI_BASIC_MINMAX
;
print, '' & print, 'Cross tests'
MULTI_CROSS_MINMAX
;
print, '' & print, 'Benchmarking for 1e6 points'
BENCH_TEST_MINMAX, nbp=1e6
;
end
