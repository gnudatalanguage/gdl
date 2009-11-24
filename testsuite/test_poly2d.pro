pro test_poly2d

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
