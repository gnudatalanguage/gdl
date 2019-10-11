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

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ; various tests for checking /NODATA, /OVERPLOT, labelling and mapping support in CONTOUR
  x = 30 * (findgen(5) - 2)
  y = 30 * (findgen(5) - 2)
  z = fltarr(5, 5) + findgen(25)

  message, /continue, "Testing: !P.MULTI = [0,0,0] & contour, z, x, y, /nodata"
  !P.MULTI = [0,0,0] & contour, z, x, y, /nodata
  next
  message, /continue, "Testing: !P.MULTI = [0,0,0] & contour, z, /nodata"
  !P.MULTI = [0,0,0] & contour, z, /nodata
  next
  message, /continue, "Testing: !P.MULTI=[0,2,1] & contour, z, x, y, /nodata & contour, z, /nodata"
  !P.MULTI=[0,2,1] & contour, z, x, y, /nodata & contour, z, /nodata
  next
  message, /continue, "Testing: !P.MULTI=[0,1,2] & contour, z, x, y, /nodata & contour, z, /nodata"
  !P.MULTI=[0,1,2] & contour, z, x, y, /nodata & contour, z, /nodata
  next
  
  message, /continue, "Testing: !P.MULTI = [0,0,0] & plot, findgen(100) & contour, z, x, y, /overplot"
  !P.MULTI = [0,0,0] & plot, findgen(100) & contour, z, x, y, /overplot
  next
  message, /continue, "Testing: !P.MULTI = [0,0,0] & findgen(10) & contour, z, /overplot"
  !P.MULTI = [0,0,0] & plot, findgen(10) & contour, z, /overplot
  next

  message, /continue, "Testing: !P.MULTI = [0,0,0] & map_set, /conti & contour, z, x, y, /overplot"
  !P.MULTI = [0,0,0] & map_set, /conti & contour, z, x, y, /overplot
  next
  message, /continue, "Testing: !P.MULTI = [0,0,0] & map_set, /conti & contour, rebin(z, 10, 10), /overplot"
  !P.MULTI = [0,0,0] & map_set, /conti & contour, rebin(z, 10, 10), /overplot
  next

  ; this is to test how GDL understands regularity of the data (it does not have to be linear!)
  ; all example below should give the same results!
  message, /continue, "Testing: !P.MULTI = [0,0,0] & contour, rebin(z, 10, 10)"
  !P.MULTI = [0,0,0] & contour, rebin(z, 10, 10)
  next
  message, /continue, "Testing: contour, rebin(z, 10, 10), rebin(x, 10), rebin(y, 10), /overplot"
  !P.MULTI = [0,0,0] & contour, rebin(z, 10, 10), rebin(x, 10), rebin(y, 10)
  next
  message, /continue, "Testing: contour, z, x, y, /overplot"
  !P.MULTI = [0,0,0] & contour, z, x, y, /overplot
  next

  message, /continue, "Testing: !P.MULTI = [0,2,1] & map_set, /grid & contour, z, /overplot & contour, z"
  !P.MULTI = [0,2,1] & map_set, /grid & contour, z, /overplot & contour, z
  next
  message, /continue, "Testing: !P.MULTI=[0,1,2] & map_set,/stereo & contour,z,x,y,/over & map_set,/adv,/gnomic & contour,z,x,y,/over"
  !P.MULTI=[0,1,2] & map_set,/stereo & contour,z,x,y,/over & map_set,/adv,/gnomic & contour,z,x,y,/over
  next

  erase

  message, /continue, "Testing: !P.MULTI=[0,1,2] & map_set,/stereo,/grid & contour,z,x,y,/over & map_set,/adv,/gnomic,/grid & contour,z,x,y,/over"
  !P.MULTI=[0,1,2] & map_set,/stereo,/grid & contour,z,x,y,/over & map_set,/adv,/gnomic,/grid & contour,z,x,y,/over
  next

  message, /continue, "Testing: !P.MULTI=[0,1,2] & map_set,/stereo,/grid & contour,z,x,y,/over,/fill & map_set,/adv,/gnomic,/grid & contour,z,x,y,/over,/fill"
  !P.MULTI=[0,1,2] & map_set,/stereo,/grid & contour,z,x,y,/over,/fill & map_set,/adv,/gnomic,/grid & contour,z,x,y,/over,/fill
  next

  message, /continue, "Testing: !P.MULTI = [0,0,0] & contour, z, /follow"
  !P.MULTI = [0,0,0] & contour, z, /follow
  next

end
