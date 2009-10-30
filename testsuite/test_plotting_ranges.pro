; testing axis range-selection in various plotting routines

pro test_plotting_ranges

  oldname = !D.NAME
  oldxstyle = !X.STYLE & !X.STYLE=0
  oldystyle = !Y.STYLE & !Y.STYLE=0
  set_plot, 'z'

  plot, [1948, 2006], [0,1]
  if !X.CRANGE[1] gt 2100 then message, 'FAILED'

  plot, [0], yrange=[400,1500] 
  if !Y.CRANGE[1] lt 1600 then message, 'FAILED'

  set_plot, oldname
  !X.STYLE = oldxstyle
  !Y.STYLE = oldystyle

end
