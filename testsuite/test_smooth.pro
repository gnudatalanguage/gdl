;
; ARSC 13/07/2006
; some tests for SMOOTH
;
pro TEST_SMOOTH, nbp=nbp
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
a(2*indgen(nbp))=a(2*indgen(nbp))+1
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
    a(nbp/2,nbp/2)=width^2
    print, 'Expected: ', TOTAL(a), ', Comp1: ', TOTAL(SMOOTH(a,width)), ', Comp2: ', TOTAL(SMOOTH(a,width*2))
endfor
print, 'second run with background'
nbp=64
level=3.
a=REPLICATE(level,nbp,nbp)
for i=1,3 do begin
    width=2*i+1
    a(nbp/2,nbp/2)=level+width^2
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
    a(nbp/2,nbp)=width^2
    a(nbp/4,nbp)=width^2
    a(3*nbp/4,nbp)=width^2
    print, 'Expected: ', TOTAL(a), ', Comp1: ', TOTAL(SMOOTH(a,width)), ', Comp2: ', TOTAL(SMOOTH(a,width*2))
endfor
;
print, '-------------- 3D test on predictible signal ! --------'
print, 'We used pseudo neutral element which is Dirac at level of Width'
nbp=64
a=fltarr(nbp,nbp,nbp)
for i=1,3 do begin
    width=2*i+1
    a(nbp/2,nbp/2,nbp/2)=width^3
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
a(2*indgen(nbp))=a(2*indgen(nbp))+1
b=a
b(4)=1./0.
print, SMOOTH(a,3)
print, SMOOTH(b,3,/nan)
;
c=replicate(0.,10)
c(6)=20/0.
c(5)=20
print, SMOOTH(c,3,/nan)
print,c

c=replicate(0.,10)
c(5)=20/0.
c(6)=20
print, SMOOTH(c,3,/nan)
print,c

;
print, '---------------- 2D and NaN -------------'
;
a=REPLICATE(0.,7,7)
a(2,3)=1./0.
a(3,3)=20.
;
b=REPLICATE(0.,7,7)
b(3,2)=1./0.
b(3,3)=20.
;
print, SMOOTH(a,3,/nan,/edg)
print, SMOOTH(b,3,/nan,/edg)
print, ' '
print, SMOOTH(a,3,/nan)
print, SMOOTH(b,3,/nan)

;
end
