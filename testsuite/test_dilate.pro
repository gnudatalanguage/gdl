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
; ---------------------
;

PRO TEST_DILATE, HELP = HELP, NERR_TOTAL, TEST = TEST, verbose = verbose

  IF KEYWORD_SET(HELP) THEN BEGIN
    PRINT, 'PRO TEST_DILATE, HELP=HELP, TEST=TEST, no_exit=no_exit, verbose=verbose'
    RETURN
  ENDIF
  NERR = 0

  ;; Test on 8-bit grayscale images. (There is no reference image can be used. Forget it.)
  ;File = FILE_SEARCH_FOR_TESTSUIT('tiff/8bit_gray_geo.tif')
  ;Image = READ_TIFF(File)
  ;Image = DILATE(Image,REPLICATE(1,5,5),/GRAY)

  ; Test on EXAMPLE 1 from http://www.harrisgeospatial.com/docs/DILATE.html
  Image = BYTE([[0,1,0,0],[0,1,0,0],[0,1,1,0],[1,0,0,0],[0,0,0,0]])
  S = [1,1]
  Result = DILATE(Image,S,0,0)
  Result_IDL = BYTE([[0,1,1,0],[0,1,1,0],[0,1,1,1],[1,1,0,0],[0,0,0,0]])
  IF ~ARRAY_EQUAL(Result, Result_IDL) THEN $
    ERRORS_ADD, NERR, 'Unexpected return value of DILATION on BINARY Image'

  ; Test on EXAMPLE 2 from http://www.harrisgeospatial.com/docs/DILATE.html
  Image = BYTE([2,1,3,3,3,3,1,2])
  ; S = [1,1]
  Result = DILATE(Image, S, /GRAY)
  Result_IDL = BYTE([1,3,3,3,3,3,2,2])
  IF ~ARRAY_EQUAL(Result, Result_IDL) THEN $
    ERRORS_ADD, NERR, 'Unexpected return values of DILATION on GRAYscale Image: DILATION using no padding'
  Result = DILATE([0, Image], S, /GRAY)
  Result = Result[1:N_ELEMENTS(image)]
  Result_IDL = BYTE([2,3,3,3,3,3,2,2])
  IF ~ARRAY_EQUAL(Result, Result_IDL) THEN $
    ERRORS_ADD, NERR, 'Unexpected return values of DILATION on GRAYscale Image: DILATION using padding'

  BANNER_FOR_TESTSUITE, 'TEST_DILATE', NERR, /STATUS
  ERRORS_CUMUL, NERR_TOTAL, NERR
  IF KEYWORD_SET(TEST) THEN STOP
END
