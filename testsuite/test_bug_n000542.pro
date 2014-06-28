pro test_bug_n000542
  img = fltarr(10,10)
  knl = [       $
    [ 0, 2, 0], $
    [-2, 0, 2], $
    [ 0,-2, 0]  $
  ]
  help, convol(img, knl, invalid=0)
  help, convol(img, knl, missing=0)
  help, convol(img, knl, /normalize)
end
