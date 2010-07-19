; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
; in reply to bug-report by Joanna (https://sourceforge.net/tracker/?func=detail&aid=3029085&group_id=97659&atid=618683)
pro test_clip

  set_plot, 'z'

  erase
  plot, findgen(10), clip=[6,6,5,5], xstyle=4, ystyle=4
  if total(tvrd()) ne 0 then begin
    message, 'failed with PLOT', /conti
    exit, status=1
  endif

  erase
  plot, [0], [0], /nodata, xstyle=4, ystyle=4
  oplot, findgen(10), clip=[6,6,5,5]
  if total(tvrd()) ne 0 then begin
    message, 'failed with OPLOT', /conti
    exit, status=1
  endif
  
  erase
  contour, findgen(10,10), clip=[6,6,5,5], xstyle=4, ystyle=4
  if total(tvrd()) ne 0 then begin
    message, 'failed with CONTOUR', /conti
    exit, status=1
  endif
 
  erase
  surface, findgen(10,10), clip=[6,6,5,5], xstyle=4, ystyle=4, zstyle=4
  if total(tvrd()) ne 0 then begin
    message, 'failed with SURFACE', /conti
    exit, status=1
  endif
  
end
