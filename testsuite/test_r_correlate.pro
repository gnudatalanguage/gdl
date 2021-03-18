; Test routine for R_CORRELATE

pro test_r_correlate
  
  a = [-0.932941, -0.903630, 0.877947, -2.29233, -2.38904, -0.141365, $
       -0.222776, 0.279628, -1.78928,  0.841182, -0.490696, 0.958234, $
       -1.01543, 0.763536, 0.278744, 0.497412, -0.826807, -1.43363, $
       0.512874, 1.06678]

  b = [0.467973, 0.824236, 0.408903, -1.15425, 0.462008, 0.298134, $
       -0.354388,  -0.465703, 0.699337, 1.89301, -0.190556, -1.04368, $
       0.466777, 0.800296, 0.642730, 0.990312, -0.965548, -0.0831093, $
       0.514957, 0.908564] 

  print, "No trend."
  print, "Expect"
  print, "Spearman:       0.26917294      0.25113144"
  print, "Kendall:        0.20000000      0.21761949"
  print, "Find:"
  print, "Spearman: ", r_correlate(a, b)
  print, "Kendall:  ", r_correlate(a, b, /kendall)

  print, "With trend."
  print, "Expect:"
  print, "Spearman:       0.99248123   7.0973850e-18"
  print, "Kendall:        0.95789474   3.5294693e-09"
  print, "Find:"
  print, "Spearman: ", r_correlate(findgen(20)+a, findgen(20)+b)
  print, "Kendall:  ", r_correlate(findgen(20)+a, findgen(20)+b, /kendall)

end
