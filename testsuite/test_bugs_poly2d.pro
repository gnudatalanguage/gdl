pro TEST_BUGS_POLY2D, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_BUGS_POLY2D, help=help, verbose=verbose, $'
   print, '                 no_exit=no_exit, test=test'
   return
endif
; checking errors for interpolation 0 ONLY - fixme.
nb_errors=0
a=bindgen(3,3)
degree=0
; option MISS changes result
good=[  0b ,  1 ,  2 , 255 ,  3 ,  4 , 255 ,  6 ,  7]
P=[0,-0.1,1,0] & Q=[0.9,1,0,0] & B = POLY_2D(A, P, Q, degree, MISS=-1)
if ~ARRAY_EQUAL(B,good) then nb_errors++
if KEYWORD_SET(test) then STOP,nb_errors

P=[0.9,-0.1,1,0] & Q=[0.9,1,0,0] & B = POLY_2D(A, P, Q, degree, MISS=-1)
if ~ARRAY_EQUAL(B,A) then nb_errors++
if KEYWORD_SET(test) then STOP,nb_errors

; small negative number is 0 if no MISS
P=[0,-1.2E-8,1,0] & Q=[0,1,0,0] & B = POLY_2D(A, P, Q, degree)
if ~ARRAY_EQUAL(B,A) then nb_errors++
if KEYWORD_SET(test) then STOP,nb_errors

good=[  0b ,  1 ,  2 , 255 ,  4 ,  5 , 255 ,  7 ,  8]
P=[0,-1.2E-8,1,0] & Q=[0,1,0,0] & B = POLY_2D(A, P, Q, degree, MISS=-1)
if ~ARRAY_EQUAL(B,good) then nb_errors++
if KEYWORD_SET(test) then STOP,nb_errors

; rotation
P=[2,0,-1,0] & Q=[2,-1,0,0] & B = POLY_2D(A, P, Q, degree, MISS=-1)
good=[ 8b, 7, 6, 5, 4, 3, 2, 1, 0]
if ~ARRAY_EQUAL(B,good) then nb_errors++
if KEYWORD_SET(test) then STOP,nb_errors

; stretch x
a=bindgen(13,3) & P=[0,0,0.5,0] & Q=[0,1,0,0] & B = POLY_2D(A, P, Q, degree, MISS=-1)
good=[13 , 13 , 14 , 14 , 15 , 15 , 16 , 16 , 17 , 17 , 18 , 18 , 19]
if ~ARRAY_EQUAL(B[*,1],good) then nb_errors++
if KEYWORD_SET(test) then STOP,nb_errors

; shift
good = [255 , 255 ,255,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22]
P=[-3,0,1,0] & Q=[0,1,0,0] & B = POLY_2D(A, P, Q, degree, MISS=-1)
if ~ARRAY_EQUAL(B[*,1],good) then nb_errors++
if KEYWORD_SET(test) then STOP,nb_errors

; pixel
good=[255,    255,      3,    255,    255,      6,    255,    255,    255]
a=bindgen(3,3)
P=[-3,0,1,0] & Q=[0,1,0,0] & B = POLY_2D(A, P, Q, degree, MISS=-1, pix=1)
if ~ARRAY_EQUAL(B,good) then nb_errors++
if KEYWORD_SET(test) then STOP,nb_errors

;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_BUGS_POLY2D', nb_errors
;
; ADDING unchecked commands to serve as a coverage test for all cases:
; initialisations: floats at end, since some commands do not accpet floats/doubles/complex
  typecodes=[1,2,3,12,13,14,15,4,5,6,9]
  all_numeric=10
  seed=33
  ; use non-trivial dimensions
  dim1=7
  dim2=9
  a=randomn(seed,dim1,dim2,/double)*randomu(seed,dim1,dim2,/ulong)
  big=ptrarr(11,/allo)
  k=0 & foreach i,typecodes do begin & *big[k]=fix(a,type=i) & k++ &end
; make dummy poly_2D on all types: 1) dimension 1
; the linear case without the x*y coefficient is already tested by test_all_basic_functions.
p=replicate(0.3,4) & q=replicate(0.4,4)
 for k=0,all_numeric do ret=poly_2d(*big[k],p,q,0, 27, 19) 
 for k=0,all_numeric do ret=poly_2d(*big[k],p,q,1, 27, 19) 
 for k=0,all_numeric do ret=poly_2d(*big[k],p,q,2, 27, 19) 
 for k=0,all_numeric do ret=poly_2d(*big[k],p,q,2, 27, 19, cub=-0.5) 
 for k=0,all_numeric do ret=poly_2d(*big[k],p,q,0, 27, 19, miss=-1) 
 for k=0,all_numeric do ret=poly_2d(*big[k],p,q,1, 27, 19, miss=-1) 
 for k=0,all_numeric do ret=poly_2d(*big[k],p,q,2, 27, 19, miss=-1) 
 for k=0,all_numeric do ret=poly_2d(*big[k],p,q,2, 27, 19, cub=-0.5, miss=-1) 
; idem dimension 2
p=replicate(0.3,9) & q=replicate(0.4,9)
 for k=0,all_numeric do ret=poly_2d(*big[k],p,q,0, 27, 19) 
 for k=0,all_numeric do ret=poly_2d(*big[k],p,q,1, 27, 19) 
 for k=0,all_numeric do ret=poly_2d(*big[k],p,q,2, 27, 19) 
 for k=0,all_numeric do ret=poly_2d(*big[k],p,q,2, 27, 19, cub=-0.5) 
 for k=0,all_numeric do ret=poly_2d(*big[k],p,q,0, 27, 19, miss=-1) 
 for k=0,all_numeric do ret=poly_2d(*big[k],p,q,1, 27, 19, miss=-1) 
 for k=0,all_numeric do ret=poly_2d(*big[k],p,q,2, 27, 19, miss=-1) 
 for k=0,all_numeric do ret=poly_2d(*big[k],p,q,2, 27, 19, cub=-0.5, miss=-1) 
; just for the fun
; p=replicate(0.3,9)& p[2]=!values.f_nan & q=replicate(0.4,9) & ret=poly_2d(randomu(33,3,3),p,q,0)
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
