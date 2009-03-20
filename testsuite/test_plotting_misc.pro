; author: Sylwester Arabas <slayoo@igf.fuw.edu.pl>
; description: tests designed to be run in parralel with an IDL session to compare plotting results
; usage: GDL> test_plotting_misc

; helper routine
pro next
  null = ""
  read, prompt="press any key to continue...", null
end

; main routine
pro test_plotting_misc

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ; two bugs when plotting in DATA coordinates with !P.MULTI != 0 (fixed in revs. 1.81 and 1.80 of plotting.cpp)
  message, /continue, "Testing: !P.MULTI=[0,2,1]&plot,findgen(10)&xyouts,.5,.5,'a'"
  !P.MULTI=[0,2,1]&plot,findgen(10)&xyouts,.5,.5,'a'
  next
  message, /continue, "Testing: !P.MULTI=[0,2,1]&plot,findgen(10)&plots,[0,1],[0,2]"
  !P.MULTI=[0,2,1]&plot,findgen(10)&plots,[0,1],[0,2]
  next
 
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ; test for "shade_max must exceed shade_min" error messages from plplot (fixed in rev. 1.79 of plotting.cpp)
  message, /continue, "Testing: contour, fltarr(2, 2) + findgen(4), levels=[5], /fill"
  contour, fltarr(2, 2) + findgen(4), levels=[5], /fill
  next
  message, /continue, "Testing: contour, fltarr(2, 2) + findgen(4), levels=[1,2,3,4], /fill"
  contour, fltarr(2, 2) + findgen(4), levels=[1,2,3,4], /fill
  next

end
