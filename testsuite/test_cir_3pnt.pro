; Test routine for cir_3pnt

pro test_cir_3pnt

  x = [1.2, 3.7, 4.9]
  y = [2., -5., 6.1]

  print, "X =", x
  print, "Y =", y
  print, "Centre should be: 7.2860995      0.22717838"
  print, "Radius should be: 6.3390459"
  
  cir_3pnt, x, y, r, x0, y0
  print, "As is   : XC =", x0, " YC =", y0, " R =", r

  cir_3pnt, reverse(x), reverse(y), r, x0, y0
  print, "Reverse : XC =", x0, " YC =", y0, " R =", r

  cir_3pnt, shift(x, 1), shift(y, 1), r, x0, y0
  print, "Shifted : XC =", x0, " YC =", y0, " R =", r

  cir_3pnt, y, x, r, x0, y0
  print, "Swap X,Y: XC =", x0, " YC =", y0, " R =", r

; This one is the RSI example, and contains a horizontal
  
  x = [1.0, 2.0, 3.0]
  y = [1.0, 2.0, 1.0]

  print, "X =", x
  print, "Y =", y
  print, "Centre should be: 2.0, 1.0"
  print, "Radius should be: 1.0"

  cir_3pnt, x, y, r, x0, y0
  print, "As is   : XC =", x0, " YC =", y0, " R =", r
  
  cir_3pnt, y, x, r, x0, y0
  print, "Swap X,Y: XC =", x0, " YC =", y0, " R =", r

end
