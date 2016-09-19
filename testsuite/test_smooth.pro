;
; under GNU GPL v2 or later
;
; Important remark: this code is not ready for inclusion in Regression
; Tests (make check) Help welcome.
;
; Alain Coulais
; Initial version 13/07/2006, only some basic fonctional tests for SMOOTH
;
; Extended on 7 May 2012 for testing new algo and time
;
; Just testing if new algo for SMOOTH is well scaled
; and errors did not diverge
; 
; A way to test the patch and also that a new algo
; did not change the outputs values too much ...
;
; If a third algo will be provided, we have to  manage "/slow"
; and put it as param ...
;
; -----------------------------------------------
;
pro Internal_Calcul, array, window, times, error, $
                     test=test, verbose=verbose
;
times=FLTARR(2)
;
t0=SYSTIME(1)
r1=SMOOTH(array,window,/slow)
times[0]=SYSTIME(1)-t0

t0=systime(1)
r2=SMOOTH(array,window)
times[1]=systime(1)-t0

error=SQRT(TOTAL((r1-r2)^2))
if KEYWORD_SET(verbose) then begin
   print, 'kernel ', window, ': ', times, ', Error :', error
endif
;
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
pro TEST_TIME_SMOOTH, testcase, nbpX=nbpX, nbpY=nbpY, $
                      test=test, help=help, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_TIME_SMOOTH, testcase, nbpX=nbpX, nbpX=nbpX, $'
   print, '                      test=test, help=help, verbose=verbose'
   return
endif
;
if N_PARAMS() EQ 0 then testcase=0
;
if ~KEYWORD_SET(nbpX) then nbpX=1024
if ~KEYWORD_SET(nbpY) then nbpY=nbpX
;
max_testcase=4
if testcase EQ 0 then test_array=DIST(nbpX)
if testcase EQ 1 then test_array=RANDOMU(seed, nbpX, nbpY)
if testcase EQ 2 then test_array=RANDOMU(seed, nbpX)
if testcase EQ 3 then test_array=REPLICATE(1.,nbpX, nbpY)
if testcase EQ 4 then test_array=REPLICATE(1.,nbpX)
;
if testcase LT 0 OR testcase GT max_testcase then MESSAGE, 'bad testcase ...'
;
kernels=[3,5,7,9,15,21,31,41,51]
nb_kernels=N_ELEMENTS(kernels)
alltimes=FLTARR(2,nb_kernels)
allerrors=FLTARR(nb_kernels)
;
for ii=0, nb_kernels-1 do begin
   Internal_Calcul, test_array, kernels[ii] , times, error, verbose=verbose
   alltimes[*,ii]=times
   allerrors[ii]=error
endfor
;
WINDOW, 0
PLOT, kernels, alltimes[0,*], psym=-2, linestyle=1, $
      xtitle='kernel size', ytitle='time (s)'
OPLOT, kernels, alltimes[1,*], psym=-4, linestyle=4
; legend
xpos=[0.05, 0.2, 0.25]*!x.crange[1]
ypos=[0.9, 0.9]*!y.crange[1]
OPLOT, xpos[0:1], ypos, psym=-2, linestyle=1
XYOUTS, xpos[2], ypos, 'Old Slow Ref'
ypos=[0.8, 0.8]*!y.crange[1]
OPLOT, xpos[0:1], ypos, psym=-4, linestyle=4
XYOUTS, xpos[2], ypos, 'New faster'
;
WINDOW, 1
PLOT, kernels, allerrors, psym=-2, linestyle=2, $
      xtitle='kernel size', ytitle='time (s)'
;
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
pro TEST_SMOOTH, nbp=nbp
;
print, 'No clear test defined now'
print, 'Only Time Test between two versions ...'
print, 'Help welcome !'
;
if (N_ELEMENTS(nbp) EQ 0) then nbp=9
;
; extra check specific for GDL
;
if (TAG_NAMES(/STRUCT,{a:0}) eq '$truct') then begin
    ;;
    r=REPLICATE(1.,nbp)
    ;;
    ;; type error
    a=SMOOTH(r,'a')
    ;;
    ;; length errors (3 cases (Neg., {0,1}, > nbp))
    a=SMOOTH(r,-10)
    a=SMOOTH(r,nbp+4)
    a=SMOOTH(r,0)
endif
;
a=fltarr([10,10,9])
temp=SMOOTH(a,[3,3])
;
; common checks on calculations
;
a=REPLICATE(1.,nbp)
a[2*indgen(nbp)]=a[2*indgen(nbp)]+1
;
print, '------------- 1D test, no edge trunc. ------------'
print, a
print, SMOOTH(a,3)
print, SMOOTH(a,4)
print, SMOOTH(a,5)
print, SMOOTH(a,6)
print, '------------- 1D test, edge trunc. ------------'
print, SMOOTH(a,3,/edge)
print, SMOOTH(a,4,/edge)
print, SMOOTH(a,5,/edge)
print, SMOOTH(a,6,/edge)
;
;gnbp=1e5
;a=randomu(seed,gnbp)
;
print, '-------------- 2D test on predictible signal ! --------'
print, 'We used pseudo neutral element which is Dirac at level of Width'
print, 'First run without background'
nbp=256
a=fltarr(nbp,nbp)
for i=1,3 do begin
    width=2*i+1
    a[nbp/2,nbp/2]=width^2
    print, 'Expected: ', TOTAL(a), ', Comp1: ', TOTAL(SMOOTH(a,width)), ', Comp2: ', TOTAL(SMOOTH(a,width*2))
endfor
print, 'second run with background'
nbp=64
level=3.
a=REPLICATE(level,nbp,nbp)
for i=1,3 do begin
    width=2*i+1
    a[nbp/2,nbp/2]=level+width^2
    print, 'Expected: ', width^2, ', Computed: ', TOTAL(SMOOTH(a,width))-nbp*nbp*level
endfor
;
;
print, '-------------- another  2D test on predictible signal ! --------'
print, 'We used multi  pseudo neutral element which is Dirac at level of Width'
nbp=256
a=fltarr(nbp,nbp*2)
for i=1,3 do begin
    width=2*i+1
    a[nbp/2,nbp]=width^2
    a[nbp/4,nbp]=width^2
    a[3*nbp/4,nbp]=width^2
    print, 'Expected: ', TOTAL(a), ', Comp1: ', TOTAL(SMOOTH(a,width)), ', Comp2: ', TOTAL(SMOOTH(a,width*2))
endfor
;
print, '-------------- 3D test on predictible signal ! --------'
print, 'We used pseudo neutral element which is Dirac at level of Width'
nbp=64
a=fltarr(nbp,nbp,nbp)
for i=1,3 do begin
    width=2*i+1
    a[nbp/2,nbp/2,nbp/2]=width^3
    print, TOTAL(a), TOTAL(SMOOTH(a,width)), TOTAL(SMOOTH(a,width*2))
endfor
;
print, '---------------- IDL vs GDL -------------'
print, 'The following computations gave same results on IDL and GDL'
print, 'We used tabulated values (all tests must be OK !)'
results=[660.19915,802392.73,49713.301,49687.058,49720.668]
error=1d-6
if (ABS(TOTAL(SMOOTH(DIST(12),3),/double)- results[0]) GT error) then print, 'OK 1' else print, 'PB 1' 
if (ABS(TOTAL(SMOOTH(DIST(128),3),/double)- results[0]) GT error) then print, 'OK 2' else print, 'PB 2' 
if (ABS(TOTAL(SMOOTH(DIST(128,12),3),/double)- results[0]) GT error) then print, 'OK 3' else print, 'PB 3' 
if (ABS(TOTAL(SMOOTH(DIST(12,12),5),/double)- results[0]) GT error) then print, 'OK 4' else print, 'PB 4' 
if (ABS(TOTAL(SMOOTH(DIST(128,12),3,/edge),/double)- results[0]) GT error) then print, 'OK 5' else print, 'PB 5' 
;
print, '---------------- SMOOTH is invariant by transposition -------------'
print, 'Both should be Zero: true in FDL, first wrong in IDL'
;
print, TOTAL(SMOOTH(DIST(128,12),3)-TRANSPOSE(SMOOTH(DIST(12,128),3)))
print, TOTAL(SMOOTH(1.D*DIST(128,12),3)-TRANSPOSE(SMOOTH(1.D*DIST(12,128),3)))
;
print, 'Basic test suite now finished'
print, ''
print, 'we are not ready for NaN'
;
return
;
; we are not ready for NaN
print, '---------------- How SMOOTH manage NaN ? -------------'
nbp=9
a=REPLICATE(1.,nbp)
a[2*indgen(nbp)]=a[2*indgen(nbp)]+1
b=a
b[4]=1./0.
print, SMOOTH(a,3)
print, SMOOTH(b,3,/nan)
;
c=replicate(0.,10)
c[6]=20/0.
c[5]=20
print, SMOOTH(c,3,/nan)
print,c

c=replicate(0.,10)
c[5]=20/0.
c[6]=20
print, SMOOTH(c,3,/nan)
print,c

;
print, '---------------- 2D and NaN -------------'
;
a=REPLICATE(0.,7,7)
a[2,3]=1./0.
a[3,3]=20.
;
b=REPLICATE(0.,7,7)
b[3,2]=1./0.
b[3,3]=20.
;
print, SMOOTH(a,3,/nan,/edg)
print, SMOOTH(b,3,/nan,/edg)
print, ' '
print, SMOOTH(a,3,/nan)
print, SMOOTH(b,3,/nan)

;
end
