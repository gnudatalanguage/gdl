;
; Alain C., ADASS, 13 Nov 2024
;
; Assuming ROTATE() works fine !
;
; ------------------------------
;
pro TEST_ROT_ANGLE_CPLX, angle, cumul_errors, help=help
;
errors=0
;
in2d=COMPLEX(DIST(20,30), DIST(20,30))
out2d=ROT(in2d, angle)
realpart=FLOAT(out2d)
imagpart=IMAGINARY(out2d)
;
if (TOTAL(ABS(realpart-imagpart)) GT 1.e-7) then begin
   ERRORS_ADD, errors, 'unexpected rounding errors for angle='+STRING(angle)
endif
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_ROT_ANGLE_CPLX", errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------
;
; We use ROTATE() as a reference for orthogonal angles ...
;
pro TEST_ROT_ROTATE, angle, cumul_errors, offset=offset, help=help
;
if (N_PARAMS() LT 1) then begin
   MESSAGE, /cont, 'Missing ANGLE as input'
   return
end
errors=0
;
direction=-1
if (angle EQ 0) then direction=0
if (angle EQ 90) then  direction=3
if (angle EQ 180) then  direction=2
if (angle EQ 270) then  direction=1
if (direction LT 0) then begin
   MESSAGE, /cont, 'inadequate ANGLE, please check'
   return
endif
;
if ~KEYWORD_SET(offset) then offset=0
;
GIVE_LIST_NUMERIC, list_numeric_types, list_numeric_names
;
for jj=0, N_ELEMENTS(list_numeric_types)-1 do begin
   for ii=3,7 do begin
      input=INDGEN(ii,ii+offset, type=list_numeric_types[jj])   
      res=ROT(input,angle)-ROTATE(input,direction)
      if (TOTAL(ABS(res)) GT 1e-7) then begin
         txt=' & type '+ list_numeric_names[jj]
         ERRORS_ADD, errors, 'sym with size '+STRING(ii)+txt
      endif
   endfor
endfor
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_ROT_ROTATE", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------
;
pro test_rot
;
cumul_errors=0
;
angle=90.
print, 'Testing for angle =', angle
TEST_ROT_ROTATE, angle, cumul_errors
;
angle=180.
print, 'Testing for angle =', angle
TEST_ROT_ROTATE, angle, cumul_errors
TEST_ROT_ROTATE, angle, cumul_errors, offset=1
TEST_ROT_ROTATE, angle, cumul_errors, offset=2
TEST_ROT_ROTATE, angle, cumul_errors, offset=3
;
angle=270.
print, 'Testing for angle =', angle
TEST_ROT_ROTATE, angle, cumul_errors
;
print, 'Testing for Complex ...'
angle=10.
TEST_ROT_ANGLE_CPLX, angle, cumul_errors
angle=123.
TEST_ROT_ANGLE_CPLX, angle, cumul_errors
angle=234.5
TEST_ROT_ANGLE_CPLX, angle, cumul_errors
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_ROT', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
