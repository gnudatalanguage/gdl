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

PRO TEST_CONVOL,PLOT=PLOT
  kern= [[-1,6,13,6,-1],[6,61,125,61,6],[13,125,253,125,13],[6,61,125,61,6],[-1,6,13,6,-1]]
;  kern=congrid(kern,21,21)

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

END










