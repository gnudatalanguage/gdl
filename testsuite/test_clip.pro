; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
; in reply to a bug-report and a patch from Joanna: 
; - https://sourceforge.net/tracker/?func=detail&aid=3029085&group_id=97659&atid=618683
; - https://sourceforge.net/tracker/?func=detail&aid=3029082&group_id=97659&atid=618685
pro test_clip

  ; this is a known-bug case -> not bothering non-developers
  if strpos(!GDL.RELEASE,'CVS') eq -1 then exit, status=77 

  set_plot, 'z'

  erase
  plot,findgen(10),/nodata,xstyle=4,ystyle=4
  xyouts,-0.5,4,'a'
  if total(tvrd()) eq 0 then begin
    message, 'failed with XYOUTS', /conti
    exit, status=1
  endif

  erase
  plot,findgen(10),/nodata,xstyle=4,ystyle=4
  plots, [-.5,-.5], [1,4]
  if total(tvrd()) eq 0 then begin
    message, 'failed with PLOTS', /conti
    exit, status=1
  endif

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
