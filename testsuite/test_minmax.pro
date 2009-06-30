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

; SA: test of the DIMENSION keyword for MIN, MAX and other routines
;     this code is written just to make a test of all conditional 
;     branches in the code- it does not test the resutls yet (TODO)
pro DIMENSION_TEST_MINMAX 
  ; the limits on dimension sizes:
  l1=5 & l2=3 & l3=2 & l4=2 & l5=1 & l6=1 & l7=1 & l8=1
  for type = 1, 15 do if type ne 8 and type ne 10 and type ne 11 then begin ; data-type loop 
    for d1=1,l1 do for d2=1,l2 do for d3=1,l3 do for d4=1,l4 do $
    for d5=1,l5 do for d6=1,l6 do for d7=1,l7 do for d8=1,l8 do begin ; dimesion loop
      data = make_array(d1, d2, d3, d4, d5, d6, d7, d8, type=type, index = type ne 7)
      for dim = 0, d8 gt 1 ? 8 : d7 gt 1 ? 7 : d6 gt 1 ? 6 : d5 gt 1 ? 5 :$
      d4 gt 1 ? 4 : d3 gt 1 ? 3 : d2 gt 1 ? 2 : d1 gt 1 ? 1 : 0 do begin
        for nan = 0, 1 do begin
          minval = min(data,                                 dim=dim, nan=nan)
          minval = min(data, minsub,                         dim=dim, nan=nan)
          minval = min(data, minsub, max=maxval,             dim=dim, nan=nan)
          minval = min(data, minsub, max=maxval, sub=maxsub, dim=dim, nan=nan)

          maxval = max(data,                                 dim=dim, nan=nan)
          maxval = max(data, maxsub,                         dim=dim, nan=nan)
          maxval = max(data, maxsub, min=minval,             dim=dim, nan=nan)
          maxval = max(data, maxsub, min=minval, sub=minsub, dim=dim, nan=nan)

          if type ne 7 then begin
            totl = total(data, dim)
            totl = total(data, dim, /cumul,                             nan=nan)
            totl = total(data, dim, /double,                            nan=nan)
            totl = total(data, dim, /double, /cumul,                    nan=nan)
  
            prod = product(data, dim,                                   nan=nan)
            prod = product(data, dim, /cumul,                           nan=nan)
          endif
        endfor

        medn = MEDIAN(data, dim=dim)
        medn = MEDIAN(data, dim=dim, /double)
        medn = MEDIAN(data, dim=dim, /even)

        if dim gt 0 then begin
          revs = REVERSE(data, dim)
          ;data = reverse(data, dim, /over)
        endif

      endfor
    endfor
  endif
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
print, '' & print, 'Testing the DIMENSION keyword'
DIMENSION_TEST_MINMAX
;
end
