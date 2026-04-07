;
; Testing DILATE
; Todo: DILATION of images
;
; Licensed under GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
;
; ---------------------
; Modification history:
;
; 2018-08-30 : Bin Wu <bin.wu (at) edinsights (dot) no>
; - Initial version
; - Basic DILATE tests
;
; 2026-03-16 : Alain C.
; - limited clean-up
; - adding new cases ERODE(DILATE()) & DILATE(ERODE())
; 
; ---------------------
;
pro TEST_DILATE_THEN_ERODE, cumul_errors, test=test
;
errors=0
;
a=BYTSCL(DIST(300))
b=a GE 100
S=REPLICATE(1, 3, 3)
res =ERODE(DILATE(B, S), S)
if TOTAL((res-b)[1:-2,1:-2],/preserve) GT 0 then ERRORS_ADD, 'Pb 1'
if (ABS(TOTAL(res-b)-137700) GT 1e-5)  then ERRORS_ADD, 'Pb 2 borders'
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_DILATE_THEN_ERODE", errors, /status, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------
; Test based on EXAMPLE 2 from
; http://www.harrisgeospatial.com/docs/DILATE.html
;
pro TEST_DILATE_1D, cumul_errors, test=test
;
errors=0
;
input=BYTE([2,1,3,3,3,3,1,2])
S=[1,1]
;
result1=DILATE(input, S, /GRAY)
expected1=BYTE([1,3,3,3,3,3,2,2])
IF ~ARRAY_EQUAL(result1, expected1) THEN $
   ERRORS_ADD, errors, 'Unexpected return values of DILATION on GRAYscale Image: DILATION using no padding (case 1)'
;
result2=DILATE([0, input], S, /GRAY)
result2=result2[1:N_ELEMENTS(input)]
expected2=BYTE([2,3,3,3,3,3,2,2])
IF ~ARRAY_EQUAL(Result2,expected2) THEN $
   ERRORS_ADD, errors, 'Unexpected return values of DILATION on GRAYscale Image: DILATION using padding (case 2)'
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_DILATE_1D", errors, /status, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------
;
;; Test on 8-bit grayscale images. (There is no reference image can be used. Forget it.)
;;File=FILE_SEARCH_FOR_TESTSUIT('tiff/8bit_gray_geo.tif')
;;Image=READ_TIFF(File)
;;Image=DILATE(Image,REPLICATE(1,5,5),/GRAY)
;
pro TEST_DILATE_BYTE, cumul_errors, test=test
;
errors=0
; 
image=BYTE([[0,1,0,0],[0,1,0,0],[0,1,1,0],[1,0,0,0],[0,0,0,0]])
S=[1,1]
result=DILATE(image,S,0,0)
expected=BYTE([[0,1,1,0],[0,1,1,0],[0,1,1,1],[1,1,0,0],[0,0,0,0]])
IF ~ARRAY_EQUAL(Result, expected) THEN $
   ERRORS_ADD, errors, 'Unexpected return value of DILATION on BINARY Image'
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_DILATE_BYTE", errors, /status, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------------------
;
pro TEST_DILATE, no_exit=no_exit, help=help, test=test, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_DILATE, help=help, test=test, no_exit=no_exit, verbose=verbose'
   return
endif
;
cumul_errors=0
;
TEST_DILATE_BYTE, cumul_errors
TEST_DILATE_THEN_ERODE, cumul_errors
TEST_DILATE_1D, cumul_errors
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_DILATE', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
