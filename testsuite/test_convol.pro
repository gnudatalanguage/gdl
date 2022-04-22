;---- to test the exactitude of the result ----
;---- Apr 2022, Jingwei WANG
;
PRO TEST_CONVOL_RES, cumul_errors, quiet=quiet, verbose=verbose, test=test, help=help

  if KEYWORD_SET(help) then begin
    print, 'pro TEST_CONVOL_RES, cumul_errors=0, quiet=quiet, verbose=verbose, test=test, help=help'
    return
  endif 

  nb_pbs_convol=0
  kern= [[-1,6,6,-1],[6,61,61,6],[13,125,125,13],[6,61,61,6],[-1,6,6,-1]]
  array =  [[1,2,3,4,5], [6,7,8,9,10], [11,12,13,14,15], [16,17,18,19,20], [21,22,23,24,25], [16,27,28,29,30]]
  
  ; expected results
  expected_result = [[0,0,0,0,0], [0,0,0,0,0], [0,0,7050,7614,0], [0,0,9880,10434,0], [0,0,0,0,0], [0,0,0,0,0]]
  expected_result_edge_zero = [[613,1380,1820,2240,2532],[1736,3591,4255,4809,5109],[3125,6256,7050,7614,7810],[4475,8901,9880,10434,10515],[5181,10711,12305,12919,12839],[3843,8680,10650,11200,11082]]
  expected_result_edge_wrap = [[5402,4671,5670,6294,6683],[4752,4021,4540,5094,5553],[7332,6601,7050,7614,8063],[10092,9361,9880,10434,10893],[12062,11331,12330,12954,13343],[10222,9491,11060,11754,12073]]
  expected_result_edge_mirror = [[1430,1689,2230,2794,3335],[3480,3739,4280,4844,5385],[6250,6509,7050,7614,8155],[8950,9279,9880,10434,10975],[10500,11379,12590,13204,13745],[10170,12099,14500,15254,15795]]
  expected_result_edge_truncate = [[1357,1639,2180,2744,3285],[3457,3739,4280,4844,5385],[6227,6509,7050,7614,8155],[8937,9279,9880,10434,10975],[10427,11379,12590,13204,13745],[9897,12099,14560,15304,15845]]
  expected_result_edge_constant0 = expected_result_edge_zero
  expected_result_edge_constant1 = [[967,1542,1964,2384,2694],[2023,3625,4265,4819,5143],[3407,6279,7050,7614,7833],[4757,8924,9880,10434,10538],[5468,10745,12315,12929,12873],[4197,8842,10794,11344,11244]]
  
  ; real results
  result = CONVOL(array, kern)
  result_edge_zero = CONVOL(array, kern, /EDGE_ZERO)
  result_edge_wrap = CONVOL(array, kern, /EDGE_WRAP)
  result_edge_mirror = CONVOL(array, kern, /EDGE_MIRROR)
  result_edge_truncate = CONVOL(array, kern, /EDGE_TRUNCATE)
  result_edge_constant0 = CONVOL(array, kern, EDGE_CONSTANT=0) ; which should equal to /edge_zero
  result_edge_constant1 = CONVOL(array, kern, EDGE_CONSTANT=1)
; here one can test the different arrays...

  IF ARRAY_EQUAL(result, expected_result,/NOT_EQUAL) THEN ERRORS_ADD, nb_pbs_convol, 'case (a,k)'
  IF ARRAY_EQUAL(result_edge_zero, expected_result_edge_zero,/NOT_EQUAL) THEN ERRORS_ADD, nb_pbs_convol, 'case edge_zero'
  IF ARRAY_EQUAL(result_edge_wrap, expected_result_edge_wrap,/NOT_EQUAL) THEN ERRORS_ADD, nb_pbs_convol, 'case edge_wrap'
  IF ARRAY_EQUAL(result_edge_mirror, expected_result_edge_mirror,/NOT_EQUAL) THEN ERRORS_ADD, nb_pbs_convol, 'case edge_mirror'
  IF ARRAY_EQUAL(result_edge_constant0, expected_result_edge_constant0,/NOT_EQUAL) THEN ERRORS_ADD, nb_pbs_convol, 'case edge_constant=0'
  IF ARRAY_EQUAL(result_edge_constant1, expected_result_edge_constant1,/NOT_EQUAL) THEN ERRORS_ADD, nb_pbs_convol, 'case edge_constant=1'
;
ERRORS_CUMUL, cumul_errors, nb_pbs_convol
if ~KEYWORD_SET(quiet) then BANNER_FOR_TESTSUITE, 'TEST_CONVOL_RES', nb_pbs_convol
;
if KEYWORD_SET(test) then STOP
;
END

PRO SUB_TEST_CONVOL, a, kern, plot=plot
  n=0 
  if (n_elements(plot) eq 0) then plot=0
  if plot then TVSCL, a, n
  result = CONVOL( a, kern) & if (plot) then TVSCL, result,n++ 
  result = CONVOL( a, kern, /EDGE_ZERO ) & if (plot) then TVSCL, result,n++ 
  result = CONVOL( a, kern, /EDGE_WRAP ) & if (plot) then TVSCL, result,n++ 
  result = CONVOL( a, kern, /EDGE_TRUNCATE ) & if (plot) then TVSCL, result,n++ 
  result = CONVOL( a, kern, /EDGE_MIRROR   ) & if (plot) then TVSCL, result,n++

  result = CONVOL( a, kern, /EDGE_ZERO     ,INVALID=255) & if (plot) then TVSCL, result,n++ 
  result = CONVOL( a, kern, /EDGE_WRAP     ,INVALID=255) & if (plot) then TVSCL, result,n++ 
  result = CONVOL( a, kern, /EDGE_TRUNCATE ,INVALID=255) & if (plot) then TVSCL, result,n++ 
  result = CONVOL( a, kern, /EDGE_MIRROR   ,INVALID=255) & if (plot) then TVSCL, result,n++

  result = CONVOL( a, kern, /EDGE_ZERO     ,INVALID=255, MISSING=0) & if (plot) then TVSCL, result,n++ 
  result = CONVOL( a, kern, /EDGE_WRAP     ,INVALID=255, MISSING=0) & if (plot) then TVSCL, result,n++ 
  result = CONVOL( a, kern, /EDGE_TRUNCATE ,INVALID=255, MISSING=0) & if (plot) then TVSCL, result,n++ 
  result = CONVOL( a, kern, /EDGE_MIRROR   ,INVALID=255, MISSING=0) & if (plot) then TVSCL, result,n++

  result = CONVOL( a, kern, /EDGE_ZERO     ,INVALID=255, MISSING=0, /NORMALIZE) & if (plot) then TVSCL, result,n++ 
  result = CONVOL( a, kern, /EDGE_WRAP     ,INVALID=255, MISSING=0, /NORMALIZE) & if (plot) then TVSCL, result,n++ 
  result = CONVOL( a, kern, /EDGE_TRUNCATE ,INVALID=255, MISSING=0, /NORMALIZE) & if (plot) then TVSCL, result,n++ 
  result = CONVOL( a, kern, /EDGE_MIRROR   ,INVALID=255, MISSING=0, /NORMALIZE) & if (plot) then TVSCL, result,n++

  result = CONVOL( a, kern, /EDGE_ZERO     ,INVALID=255, MISSING=0, /NORMALIZE, /NAN) & if (plot) then TVSCL, result,n++ 
  result = CONVOL( a, kern, /EDGE_WRAP     ,INVALID=255, MISSING=0, /NORMALIZE, /NAN) & if (plot) then TVSCL, result,n++ 
  result = CONVOL( a, kern, /EDGE_TRUNCATE ,INVALID=255, MISSING=0, /NORMALIZE, /NAN) & if (plot) then TVSCL, result,n++ 
  result = CONVOL( a, kern, /EDGE_MIRROR   ,INVALID=255, MISSING=0, /NORMALIZE, /NAN) & if (plot) then TVSCL, result,n++

END




PRO TEST_CONVOL, cumul_errors, plot=plot, test=test, help=help
;
  if KEYWORD_SET(help) then begin
    print, 'PRO TEST_CONVOL, cumul_errors, plot=plot, test=test, help=help'
    return
  endif 

cumul_errors = 0
TEST_CONVOL_RES, cumul_errors, quiet=1
;
  kern= [[-1,6,13,6,-1],[6,61,125,61,6],[13,125,253,125,13],[6,61,125,61,6],[-1,6,13,6,-1]]
; kern=congrid(kern,21,21)

  array = [DIST(100,100)]*0.003 + 0.1*RANDOMN(s,300,300) 
  fact=max(array)
  barray = BYTSCL( array, TOP=254 ) 
  barray[*, RANDOMU(s,40)*300] = 255 
  barray[*, 10:20] = 255 

  uiarray = UINT(array/fact*32768) 
  uiarray[*, RANDOMU(s,40)*300] = 255 
  uiarray[*, 10:20] = 255 

  iarray = FIX( array/fact*32768) 
  iarray[*, RANDOMU(s,40)*300] = 255 
  iarray[*, 10:20] = 255 

  array[*, RANDOMU(s,40)*300] = 255 
  array[*, 10:20] = 255 

  nanarray=array
  nanarray[*, RANDOMU(s,40)*300] = !values.f_nan 
  nanarray[*, 10:20] = !values.f_nan

; here one can test the different arrays...

  sub_test_convol,array,kern,plot=plot
  sub_test_convol,iarray,kern,plot=plot
  sub_test_convol,uiarray,kern,plot=plot
  sub_test_convol,barray,kern,plot=plot

  sub_test_convol,nanarray,kern,plot=plot

;
BANNER_FOR_TESTSUITE, 'TEST_CONVOL', cumul_errors
;
if KEYWORD_SET(test) then STOP
;
END










