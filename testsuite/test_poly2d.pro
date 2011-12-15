;
; Visual tests for POLY_2D
;
; reworked by Alain on 2011-Dec-15
;
; TEST_POLY2D_NEW is not finished but it is now
; more convenient to see that:
;  -- missing is not working
;  -- we have a problem at some edges or in some regions
;
pro TEST_POLY2D_OLD

XO = [61, 62, 143, 133]  
YO = [89, 34, 38, 105]  
   
; Set up the arrays of points to be fit:  
XI = [24, 35, 102, 92]  
YI = [81, 24, 25, 92]  

; Use POLYWARP to generate the P and Q inputs to POLY_2D:  
POLYWARP, XI, YI, XO, YO, 1, P, Q  
 
; Perform an image warping based on P and Q:  
; Create and display a simple image:  
A = BYTSCL(SIN(DIST(250)))
a1=double(a[0:99,0:199])

window,0,xs=300,ys=300 & tv,a1

B0 = POLY_2D(A1, P, Q,missing=0)
window,1,xs=220,ys=220
tv,b0

B1 = POLY_2D(A1, P, Q,missing=0,cubic=-0.5)
window,2,xs=220,ys=220
tv,b1

B2 = POLY_2D(A1, P, Q,2,missing=0)
window,3,xs=220,ys=220
tv,b1

B3 = POLY_2D(A1, P, Q,1,missing=0)
window,4,xs=220,ys=220
tv,b3

B4 = POLY_2D(A1, P, Q,1,50,100,missing=0)
window,5,xs=220,ys=220
tv,b4



; LINEAR
;A = BYTSCL(SIN(DIST(150)))
;a1=a[0:99,0:199]
window,0,xs=220,ys=220 & tv,a1

; Identity
b1=poly_2d(a1,[0,0,1,0],[0,1,0,0])
window,1,xs=220,ys=220 & tv,b1

; stretch x by factor 2
b1=poly_2d(a1,[0,0,0.5,0],[0,1,0,0])
window,2,xs=220,ys=220 & tv,b1

; shrink y by factor 2
b1=poly_2d(a1,[0,0,1,0],[0,2,0,0],/missing)
window,3,xs=220,ys=220 & tv,b1

; stretch x by factor 2 & shrink y by factor 2
b1=poly_2d(a1,[0,0,0.5,0],[0,2,0,0],/missing)
window,4,xs=220,ys=220 & tv,b1

; shift left by 20 pixels
b1=poly_2d(a1,[20,0,1,0],[0,1,0,0],/missing)
window,5,xs=220,ys=220 & tv,b1

; shift right by 20 pixels
b1=poly_2d(a1,[-20,0,1,0],[0,1,0,0],/missing)
window,6,xs=220,ys=220 & tv,b1

; shift up by 20 pixels
b1=poly_2d(a1,[0,0,1,0],[20,1,0,0],/missing)
window,7,xs=220,ys=220 & tv,b1

; shift down by 20 pixels
b1=poly_2d(a1,[0,0,1,0],[-20,1,0,0],/missing)
window,8,xs=220,ys=220 & tv,b1

; shift left by 40 up by 20 pixels
b1=poly_2d(a1,[40,0,1,0],[20,1,0,0],/missing)
window,9,xs=220,ys=220 & tv,b1

; rotate by 30 degrees
c=cos(30*!pi/180)
s=sin(30*!pi/180)
b1=poly_2d(a1,[0,c,s,0],[0,-s,c,0],/missing)
window,10,xs=220,ys=220 & tv,b1

; stretch x by factor 2 & shrink y by factor 2
b1=poly_2d(a1,[0,0,0.5,0],[0,2,0,0],0,50,100,/missing)
window,11,xs=220,ys=220 & tv,b1

; shift left by 40 up by 20 pixels
b1=poly_2d(a1,[40,0,1,0],[20,1,0,0],0,50,100,/missing)
window,12,xs=220,ys=220 & tv,b1


; X-Translation
z=30.5
p=[[z,0.],[1,0]] & q=[[0.,1],[0,0]]

B1a = POLY_2D(A1, P, Q,missing=0)
window,13,xs=220,ys=220 & tv,b1a

B1b = POLY_2D(A1, P, Q,missing=0,cubic=-0.5)
window,14,xs=220,ys=220 & tv,b1b

diff=b1b-b1a
tv,diff


; Y-Translation
z=40.5
p=[[0,0.],[1,0]] & q=[[z,1],[0,0]]

B1a = POLY_2D(A1, P, Q,missing=0)
window,13,xs=220,ys=220 & tv,b1a

B1b = POLY_2D(A1, P, Q,missing=0,cubic=-0.5)
window,14,xs=220,ys=220 & tv,b1b

diff=b1b-b1a
tv,diff


; XY-Translation
z=40.5
p=[[z,0.],[1,0]] & q=[[z,1],[0,0]]

B1a = POLY_2D(A1, P, Q,missing=0)
window,13,xs=220,ys=220 & tv,b1a

B1b = POLY_2D(A1, P, Q,missing=0,cubic=-0.5)
window,14,xs=220,ys=220 & tv,b1b

diff=b1b-b1a
tv,diff

return
end
;
; reworked by Alain on 2011-Dec-15
;
pro INIT_POLYWARP, P, Q, $
                   xI=xI, yI=yI, xO=xO, yO=yO, degree=degree, $
                   test=test, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro INIT_POLYWARP, P, Q, $'
   print, '                   xI=xI, yI=yI, xO=xO, yO=yO, degree=degree, $'
   print, '                   test=test, help=help'
endif
;
paths=STRSPLIT(!PATH, PATH_SEP(/SEARCH_PATH), /EXTRACT)
polywarp_path=FILE_SEARCH(paths+'/polywarp.pro', count=nfiles)
if (nfiles EQ 0) then begin
   MESSAGE, /continue, 'POLYWARP not in the !PATH ... Fixed values used'
   p=[[-5.37842,-0.320945],[ 0.751471,  0.00222929]]
   q=[[-10.1480,  1.07085],[-0.0168754,-0.000576214]]
endif else begin
   ;; Set up the arrays of points to be fit:  
   if N_ELEMENTS(xI) EQ 0 then xI = [24, 35, 102, 92]  
   if N_ELEMENTS(yI) EQ 0 then yI = [81, 24, 25, 92]  
   if N_ELEMENTS(xO) EQ 0 then xO = [61, 62, 143, 133]  
   if N_ELEMENTS(yO) EQ 0 then yO = [89, 34, 38, 105]  
   if N_ELEMENTS(degree) EQ 0 then degree=1
   ;; Use POLYWARP to generate the P and Q inputs to POLY_2D:  
   POLYWARP, XI, YI, XO, YO, 1, P, Q
endelse
;
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------
;
pro TV_OR_WINDOW, image, position, split, title=title
;
if (split NE 0) then begin
   ws=SIZE(image,/dim)
   WINDOW, xs=ws[0], ys=ws[1], title=title,/free
   TV, image
endif else begin
   TV, image, position
endelse
end
;
; ------------------------------
;
pro TEST_POLY2D_NEW, image,nbpX=nbpX, nbpY=nbpY, $
                     split_windows=split_windows, $
                     test=test, debug=debug, help=help
;
split=0
if KEYWORD_SET(split_windows) then split=1
;
INIT_POLYWARP, P, Q
;
; Perform an image warping based on P and Q:  
; Create and display a simple image:  
;
if N_PARAMS() EQ 0 then begin
   A = BYTSCL(SIN(DIST(250)))
   a1=DOUBLE(a)
endif else begin
   a1=DOUBLE(image)
endelse
;
if (N_ELEMENTS(nbpX) EQ 0 ) then nbpX=100
if (N_ELEMENTS(nbpY) EQ 0 ) then nbpY=200
;
a1=a1[0:nbpX-1,0:nbpY-1]
;
;
WINDOW, 0, xs=300, ys=300, title='input'
TV, a1
;
WINDOW, 1, xs=6*nbpX, ys=nbpY, title='input (L), 5 transforms (/miss)'
position=0
;
TV_OR_WINDOW, a1, position, split, title='ref.'
;
B0 = POLY_2D(a1, P, Q, missing=0)
TV_OR_WINDOW, b0, position+1, split, title='ref.'
TV, b0, position+1
;
b1 = POLY_2D(a1, P, Q, missing=0, cubic=-0.5)
TV_OR_WINDOW, b1, position+2, split, title='ref.'
;
b2 = POLY_2D(a1, P, Q, 2, missing=0)
TV_OR_WINDOW, b2, position+3, split, title=''
;
b3 = POLY_2D(a1, P, Q, 1, missing=0)
TV_OR_WINDOW, b3, position+4, split, title=''
;
; here it is more tricky because the size of "b4"
; is changed. We need to copy it.
b4f=FLTARR(SIZE(a1,/dim))
b4 = POLY_2D(a1, P, Q, 1, 50, 100)
b4f[0:50-1,0:100-1]=b4
TV_OR_WINDOW, b4f, position+5, split, title='sub image'
;
if KEYWORD_SET(debug) then STOP
;
; Same than previous but without activating Missing Keyword
;
WINDOW, 2, xs=6*nbpX, ys=nbpY, title='input (L), 5 trans. (no missing)'
position=0
;
TV_OR_WINDOW, a1, position, split, title=''
;
b0 = POLY_2D(a1, P, Q)
TV_OR_WINDOW, b0, position+1, split, title=''
;
b1 = POLY_2D(a1, P, Q,cubic=-0.5)
TV_OR_WINDOW, b1, position+2, split, title=''
;
b2 = POLY_2D(a1, P, Q, 2)
TV_OR_WINDOW, b2, position+3, split, title=''
;
b3 = POLY_2D(a1, P, Q, 1)
TV_OR_WINDOW, b3, position+4, split, title=''
;
; here it is more tricky because the size of "b4"
; is changed. We need to copy it.
b4f=FLTARR(SIZE(a1,/dim))
b4 = POLY_2D(a1, P, Q, 1, 50, 100)
b4f[0:50-1,0:100-1]=b4
TV_OR_WINDOW, b4f, position+5, split, title=''
;
if KEYWORD_SET(debug) then STOP
;
; --------------------
; LINEAR
WINDOW, 3, xs=4*nbpX, ys=3*nbpY
position=0
;
; Identity
b1=poly_2d(a1,[0,0,1,0],[0,1,0,0])
TV, b1, position
;
; stretch x by factor 2
b1=poly_2d(a1,[0,0,0.5,0],[0,1,0,0])
TV, b1, position+1
;
; shrink y by factor 2
b1=poly_2d(a1,[0,0,1,0],[0,2,0,0],/missing)
TV, b1, position+2
;
; stretch x by factor 2 & shrink y by factor 2
b1=poly_2d(a1,[0,0,0.5,0],[0,2,0,0],/missing)
TV, b1, position+3
;
; shift left by 20 pixels
b1=poly_2d(a1,[20,0,1,0],[0,1,0,0],/missing)
TV, b1, position+4
;
; shift right by 20 pixels
b1=poly_2d(a1,[-20,0,1,0],[0,1,0,0],/missing)
TV, b1, position+5
;
; shift up by 20 pixels
b1=poly_2d(a1,[0,0,1,0],[20,1,0,0],/missing)
TV, b1, position+6
;
; shift down by 20 pixels
b1=poly_2d(a1,[0,0,1,0],[-20,1,0,0],/missing)
TV, b1, position+7
;
; shift left by 40 up by 20 pixels
b1=poly_2d(a1,[40,0,1,0],[20,1,0,0],/missing)
TV, b1, position+8
;
; rotate by 30 degrees
c=cos(30*!pi/180)
s=sin(30*!pi/180)
b1=poly_2d(a1,[0,c,s,0],[0,-s,c,0],/missing)
TV, b1, position+9

; stretch x by factor 2 & shrink y by factor 2
b1=poly_2d(a1,[0,0,0.5,0],[0,2,0,0],0,50,100,/missing)
TV, b1, position+10

; shift left by 40 up by 20 pixels
b1=poly_2d(a1,[40,0,1,0],[20,1,0,0],0,50,100,/missing)
TV, b1, position+11

stop

; X-Translation
z=30.5
p=[[z,0.],[1,0]] & q=[[0.,1],[0,0]]

b1a = POLY_2D(a1, P, Q,missing=0)
WINDOW,13,xs=220,ys=220 & TV, b1a

b1b = POLY_2D(a1, P, Q,missing=0,cubic=-0.5)
WINDOW,14,xs=220,ys=220 & TV, b1b

diff=b1b-b1a
TV,diff


; Y-Translation
z=40.5
p=[[0,0.],[1,0]] & q=[[z,1],[0,0]]

b1a = POLY_2D(a1, P, Q,missing=0)
WINDOW,13,xs=220,ys=220 & TV, b1a

b1b = POLY_2D(a1, P, Q,missing=0,cubic=-0.5)
WINDOW,14,xs=220,ys=220 & TV, b1b

diff=b1b-b1a
TV,diff


; XY-Translation
z=40.5
p=[[z,0.],[1,0]] & q=[[z,1],[0,0]]

b1a = POLY_2D(a1, P, Q,missing=0)
WINDOW,13,xs=220,ys=220 & TV, b1a

b1b = POLY_2D(a1, P, Q,missing=0,cubic=-0.5)
WINDOW,14,xs=220,ys=220 & TV, b1b

diff=b1b-b1a
TV,diff

return
end

pro TEST_POLY2D

TEST_POLY2D_NEW

end
