; SA
pro test_outofmem, testnum

  ; bigenough should be set to a number that:
  ; - multiplied by the size of byte is allocable
  ; - multiplied by the size of complex type is not allocable
  bigenough = 1000L*1000L*1000L

  if not keyword_set(testnum) then message, "usage: GDL> test_outofmem, testnum (1,2,3...)" 

  case testnum of
    1 : a = dblarr(bigenough)
    2 : a = fltarr(bigenough, /nozero)
    3 : a = bytarr(bigenough) + complex(0,1)
    4 : a = histogram([0], nbins=bigenough, loc=b)
    5 : contour, bytarr(bigenough)
    else : message, "argument out of range"
  endcase

end
