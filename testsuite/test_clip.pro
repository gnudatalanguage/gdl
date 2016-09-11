; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
; in reply to a bug-report and a patch from Joanna: 
; - https://sourceforge.net/tracker/?func=detail&aid=3029085&group_id=97659&atid=618683
; - https://sourceforge.net/tracker/?func=detail&aid=3029082&group_id=97659&atid=618685
pro test_clip

  set_plot, 'z'
  errcnt =  0

  erase
  plot,findgen(10),/nodata,xstyle=4,ystyle=4
  polyfill, [10,11,12], [10,12,11], /line_fill
  if total(tvrd()) eq 0 then begin
    message, 'failed with POLYFILL', /conti
    errcnt++
  endif

  erase
  plot,findgen(10),/nodata,xstyle=4,ystyle=4
  xyouts,-0.5,4,'a'
  if total(tvrd()) eq 0 then begin
    message, 'failed with XYOUTS', /conti
    errcnt++
  endif

  erase
  plot,findgen(10),/nodata,xstyle=4,ystyle=4
  plots, [-.5,-.5], [1,4] 
  if total(tvrd()) eq 0 then begin
    message, 'failed with PLOTS', /conti
    errcnt++
  endif

  erase
  plot, findgen(10), clip=[6,6,5,5], xstyle=4, ystyle=4
  if total(tvrd()) ne 0 then begin &$
    message, 'failed with PLOT', /conti &$
    errcnt++ &$
  endif

  erase
  plot, [0], [0], /nodata, xstyle=4, ystyle=4
  oplot, findgen(10), clip=[6,6,5,5]
  if total(tvrd()) ne 0 then begin &$
    message, 'failed with OPLOT', /conti &$
    errcnt++ &$
  endif
 
  erase
  contour, findgen(10,10), clip=[6,6,5,5], xstyle=4, ystyle=4
  if total(tvrd()) ne 0 then begin &$
    message, 'failed with CONTOUR', /conti &$
    errcnt++ &$
  endif

  erase
  surface, findgen(10,10), clip=[6,6,5,5], xstyle=4, ystyle=4, zstyle=4
  if total(tvrd()) ne 0 then begin &$
    message, 'failed with SURFACE', /conti &$
    errcnt++ &$
  endif
  
  if errcnt ne 0 then exit, status=1

end
