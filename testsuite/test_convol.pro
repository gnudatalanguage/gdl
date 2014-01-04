PRO TEST_CONVOL
  kern= [[-1,6,13,6,-1],[6,61,125,61,6],[13,125,253,125,13],[6,61,125,61,6],[-1,6,13,6,-1]]


  array = [DIST(300,300)]*0.003 + 0.1*RANDOMN(s,300,300) 
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
; here one can test the different arrays...
  result = CONVOL( array, kern, INVALID=255, MISSING=0,/NORMALIZE, /EDGE_ZERO )
  TVSCL, array,1 & TVSCL, result,2 
  result = CONVOL( barray, kern, INVALID=255, MISSING=0,/NORMALIZE, /EDGE_ZERO )
  TV, barray,1 & TV, result,2
  kern=rebin(kern,20,20)
  result = CONVOL( iarray, kern, INVALID=255, MISSING=0,/NORMALIZE, /EDGE_ZERO )
  TVSCL, iarray,1 & TVSCL, result,2
END










