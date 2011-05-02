; testing axis range-selection in various plotting routines
; TODO:
; - repeat the tests for SURFACE, CONTOUR, etc, and for X/Y axes
; - repeat the tests for autoscaling, different !*.STYLEs, and *RANGE keywords
; - test /[XY]LOG cases
pro test_plotting_ranges

  err = 0

  set_plot, 'z'

  plot, [1948, 2006], [0,1]
  if !X.CRANGE[1] gt 2100 then err += 1

; still to be fixed 
;  plot, [0], yrange=[400,1500] 
;  if !Y.CRANGE[1] lt 1600 then err += 1

  if err ne 0 then exit, status=1

end
