;
; ARSC 15 mars 2006
;
; A test suite for DERIV function
; -- we check basic errors (wrong numbers of inputs, bad type)
; -- we compute derivation of a line
; -- we compute derivation of Gaussian
;
pro test_deriv, nbp=nbp, sigma=sigma, test=test, verbose=verbose
;
suite=''
;
if N_ELEMENTS(nbp) EQ 0 then nbp=101 else nbp=(nbp/2)*2+1
;
if N_ELEMENTS(sigma) EQ 0 then sigma=10.
;
; -----------------------------------
; We prepare some basic quantities
;
; Constant position
;
x=FINDGEN(nbp)-nbp/2.
;
; Random position
;
xr=RANDOMN(seed, 1000)
xr=xr > (-10.) < 10.  ;; we remove "tails"
xr=xr*sigma
xr=xr[SORT(xr)]
xr=xr[UNIQ(xr)]
;
; --------------------------------------
print, '--------- basic checks ---------------'
print, 'Checking number of provided vectors'
print, 'Case 1a: should issue: % DERIV: Incorrect number of arguments'
if ~ (execute("temp=DERIV()") eq 0) then exit, status=1
print, 'Case 1b: should issue: % DERIV: Incorrect number of arguments'
if ~ (execute("temp=DERIV(x,x,x)") eq 0) then exit, status=1
;
print, 'Checking type of vectors'
print, 'Case 2a: should issue: % DERIV: X and Y vectors must have same size !'
if ~ (execute("temp=DERIV(x,'a')") eq 0) then exit, status=1
print, 'Case 2b: should issue: % DERIV: No STRING vector allowed !'
if ~ (execute("temp=DERIV(REPLICATE('a',nbp))") eq 0) then exit, status=1
print, 'Case 2c: should issue: % DERIV: No STRING vector allowed !'
if ~ (execute("temp=DERIV(x,REPLICATE('a',nbp))") eq 0) then exit, status=1
;
; --------------------------------------
; Derivation of a slope is constant --> very convenient test !
; This straigh line should give constant derivation, even at the Edges
;
print, '------------- Test on Constant lines -------------'
print, 'derivation of lines should give constant'
if keyword_set(verbose) then print, 'Should plot 3 lines'
;
if keyword_set(verbose) then plot, x, DERIV(x), yrange=[0,1.2]
if n_elements(uniq(deriv(x))) ne 1 then begin
  exit, status=1
endif
;
if keyword_set(verbose) then oplot, x, DERIV(x,x)-1./3.
if n_elements(uniq(deriv(x,x))) ne 1 then begin
  exit, status=1
endif
if keyword_set(verbose) then oplot, xr, DERIV(xr,xr)-2./3.
der = deriv(xr,xr)
if max(der) - min(der) gt .1 then begin
  exit, status=1
endif
;
if keyword_set(verbose) then read, 'Press Enter', suite
;
; -----------------------------------
; we generate a gaussian shape
;
y=EXP(-(x/sigma)^2)
;
; Basic tests
;
dy1=DERIV(y)
dy2=DERIV(x,y)
;
; constant steps
;
error=TOTAL(ABS(dy1-dy2))
if (error GT 0.) then begin
    print, 'Expected value is Zero:', error
    if ~keyword_set(verbose) then exit, status=1
endif else begin
    print, 'Case with constant step well done'
endelse
;
; no-sorted data
;
print, '------------------'
print, 'Should issue :% DERIV: X colocations MUST be sorted !'
x2=x
x2(nbp/2)=x2(nbp/2+4)
if ~ (execute("dy2=DERIV(x2,y)") eq 0) then exit, status=1
;
; Zero step
;
print, '------------------'
print, 'Should issue :% DERIV: At least 2 X colocations are EQUAL !'
x2=x
x2(nbp/2+1)=x2(nbp/2)
if ~ (execute("dy2=DERIV(x2,y)") eq 0) then exit, status=1
;
print, '------------------ Derivation of a Gaussian -------------'
print, 'now, line and squares should be overplotted'
;
print, 'Small change in steps (first test)'
;
x2=x
x2(nbp/2)=x2(nbp/2)+1e-3
dy2=DERIV(x2,y)
;
if keyword_set(verbose) then begin
  plot, x, dy1
  oplot, x2, dy2, psym=-6
  read, 'Press Enter', suite
endif
;
print, 'random type 1 (small random)'
;
x2=x
x2=x2+(RANDOMU(seed, nbp)-0.5)/20.
dy2=DERIV(x2,y)
if keyword_set(verbose) then begin
  plot, x, dy1
  oplot, x2, dy2, psym=-6
  read, 'Press Enter', suite
endif
;
print, 'random type 2 (strongly random)'
;
xr=RANDOMN(seed, 100)
xr=xr > (-6.) < 6.  ;; we remove "tails"
xr=xr*sigma
xr=xr[SORT(xr)]
xr=xr[UNIQ(xr)]
yr=EXP(-(xr/sigma)^2)
dyr=DERIV(xr,yr)
;
if keyword_set(verbose) then begin
  plot, x, dy1
  oplot, xr, dyr, psym=-6
  read, 'Press Enter', suite
endif
;
print, 'random type 2 (strongly random, high number of points)'
;
yr=EXP(-(xr/sigma)^2)
dyr=DERIV(xr,yr)
;
if keyword_set(verbose) then begin
  plot, x, dy1
  oplot, xr, dyr, psym=-6
  read, 'Press Enter', suite
endif
;
print, 'Tests for DERIV() are now finished ...'
;
if KEYWORD_SET(test) then STOP
;
end
