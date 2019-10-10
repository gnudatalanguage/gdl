;
; A Coulais 15 March 2006, revisited 10 Oct 2012
;
; A test suite for DERIV function
; -- we check basic errors (wrong numbers of inputs, bad type)
; -- we compute derivation of a line
; -- we compute derivation of Gaussian
;
; This test failed sporadically due to consequence of usage of RANDOM()
;
pro TEST_DERIV, nbp=nbp, sigma=sigma, no_exit=no_exit, $
                help=help, test=test, verbose=verbose
;
if KEYWORD_SET(test) then begin
    print, 'pro TEST_DERIV, nbp=nbp, sigma=sigma, no_exit=no_exit, $'
    print, '                help=help, test=test, verbose=verbose'
    return
endif
;
suite=''
nb_errors=0
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
if ~ (EXECUTE("temp=DERIV()") eq 0) then nb_errors++
print, 'Case 1b: should issue: % DERIV: Incorrect number of arguments'
if ~ (EXECUTE("temp=DERIV(x,x,x)") eq 0) then nb_errors++
;
print, 'Checking type of vectors'
print, 'Case 2a: should issue: % DERIV: X and Y vectors must have same SIZE !'
if ~ (EXECUTE("temp=DERIV(x,'a')") eq 0) then nb_errors++
print, 'Case 2b: should issue: % DERIV: No STRING vector allowed !'
if ~ (EXECUTE("temp=DERIV(REPLICATE('a',nbp))") eq 0) then nb_errors++
print, 'Case 2c: should issue: % DERIV: No STRING vector allowed !'
if ~ (EXECUTE("temp=DERIV(x,REPLICATE('a',nbp))") eq 0) then nb_errors++
;
; --------------------------------------
; Derivation of a slope is constant --> very convenient test !
; This straigh line should give constant derivation, even at the Edges
;
print, '------------- Test on Constant lines -------------'
print, 'derivation of lines should give constant'
if KEYWORD_SET(verbose) then print, 'Should plot 3 lines'
;
if KEYWORD_SET(verbose) then PLOT, x, DERIV(x), yrange=[0,1.2]
if N_ELEMENTS(UNIQ(deriv(x))) ne 1 then begin
    print, 'Error in basic LINE deriv test'
    nb_errors++
endif
;
if KEYWORD_SET(verbose) then OPLOT, x, DERIV(x,x)-1./3.
if N_ELEMENTS(UNIQ(DERIV(x,x))) ne 1 then begin
    print, 'Error in (x,x) LINE deriv test'
    nb_errors++
endif
if KEYWORD_SET(verbose) then oplot, xr, DERIV(xr,xr)-2./3.
der = DERIV(xr,xr)
if ((MAX(der) - MIN(der)) gt .1) then begin
    print, 'Error in RANDOM Gaussian test'
    nb_errors++
endif
;
if KEYWORD_SET(verbose) then read, 'Press Enter', suite
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
    if ~KEYWORD_SET(verbose) then nb_errors++
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
if ~ (execute("dy2=DERIV(x2,y)") eq 0) then nb_errors++
;
; Zero step
;
print, '------------------'
print, 'Should issue :% DERIV: At least 2 X colocations are EQUAL !'
x2=x
x2(nbp/2+1)=x2(nbp/2)
if ~ (execute("dy2=DERIV(x2,y)") eq 0) then nb_errors++
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
if KEYWORD_SET(verbose) then begin
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
if KEYWORD_SET(verbose) then begin
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
if KEYWORD_SET(verbose) then begin
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
if KEYWORD_SET(verbose) then begin
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
