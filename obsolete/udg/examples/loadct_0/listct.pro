pro listct
  !X.STYLE=5
  !Y.STYLE=5
  !P.MULTI=[0,3,14]
  !X.MARGIN=[10,0]
  !Y.MARGIN=[1,0]
  device, /color
  for i=0, 40 do begin
    loadct, i, /silent
    contour, [[indgen(255)],[indgen(255)]], nlevels=256, /fill
    xyouts, -77, .5, strmid(i, 2)
  endfor
end
