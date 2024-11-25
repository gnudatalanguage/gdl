pro TEST_BUGS_POLY2D, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_BUGS_POLY2D, help=help, verbose=verbose, $'
   print, '                 no_exit=no_exit, test=test'
   return
endif
;
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
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
