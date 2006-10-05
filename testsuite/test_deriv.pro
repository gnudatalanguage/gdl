;
; ARSC 15 mars 2006
;
; A test suite for DERIV function
; -- we check basic errors (wrong numbers of inputs, bad type)
; -- we compute derivation of a line
; -- we compute derivation of Gaussian
;
pro test_deriv, nbp=nbp, sigma=sigma, test=test
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
temp=DERIV()
print, 'Case 1b: should issue: % DERIV: Incorrect number of arguments'
;temp=DERIV(x,x,x)
print, 'This test was not done because it STOPs the programm !'
;
print, 'Checking type of vectors'
print, 'Case 2a: should issue: % DERIV: X and Y vectors must have same size !'
temp=DERIV(x,'a')
print, 'Case 2b: should issue: % DERIV: No STRING vector allowed !'
temp=DERIV(REPLICATE('a',nbp))
print, 'Case 2c: should issue: % DERIV: No STRING vector allowed !'
temp=DERIV(x,REPLICATE('a',nbp))
;
; --------------------------------------
; Derivation of a slope is constant --> very convenient test !
; This straigh line should give constant derivation, even at the Edges
;
print, '------------- Test on Constant lines -------------'
print, 'derivation of lines should give constant'
print, 'Should plot 3 lines'
;
plot, x, DERIV(x), yrange=[0,1.2]
;
oplot, x, DERIV(x,x)-1./3.
oplot, xr, DERIV(xr,xr)-2./3.
;
read, 'Press Enter', suite
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
dy2=DERIV(x2,y)
;
; Zero step
;
print, '------------------'
print, 'Should issue :% DERIV: At least 2 X colocations are EQUAL !'
x2=x
x2(nbp/2+1)=x2(nbp/2)
dy2=DERIV(x2,y)
;
print, '------------------ Derivation of a Gaussian -------------'
print, 'now, line and squares should be oberplotted'
;
print, 'Small change in steps (first test)'
;
x2=x
x2(nbp/2)=x2(nbp/2)+1e-3
dy2=DERIV(x2,y)
;
plot, x, dy1
oplot, x2, dy2, psym=-6
read, 'Press Enter', suite
;
print, 'random type 1 (small random)'
;
x2=x
x2=x2+(RANDOMU(seed, nbp)-0.5)/20.
dy2=DERIV(x2,y)
plot, x, dy1
oplot, x2, dy2, psym=-6
read, 'Press Enter', suite
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
plot, x, dy1
oplot, xr, dyr, psym=-6
read, 'Press Enter', suite
;
print, 'random type 2 (strongly random, high number of points)'
;
yr=EXP(-(xr/sigma)^2)
dyr=DERIV(xr,yr)
;
plot, x, dy1
oplot, xr, dyr, psym=-6
read, 'Press Enter', suite
;
print, 'Tests for DERIV() are now finished ...'
;
if KEYWORD_SET(test) then STOP
;
end
