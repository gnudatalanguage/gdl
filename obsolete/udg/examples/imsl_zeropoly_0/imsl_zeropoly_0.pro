c = [-1,2,3,-4]
x = -1 + findgen(100) / 40
device, /color, /decomposed
plot, x, c[0] + c[1] * x + c[2] * x^2 + c[3] * x^3, $
  xtitle='X', ytitle='Y', thick=3 
oplot, x, replicate(0,n_elements(x)), color='ff0000'x
foreach z, imsl_zeropoly(c) do $
  plots, z, 0., psym=6, thick=3, color='0000ff'x
