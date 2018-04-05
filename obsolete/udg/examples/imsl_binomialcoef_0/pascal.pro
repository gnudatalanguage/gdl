pro pascal, n
  tri = replicate('   ', 2 * n - 1, n) 
  for i=0, n-1 do for j=0, i do tri[2*j + (n-i)-1, i] = $
    string(imsl_binomialcoef(i, j), f='(I3)')
  print, tri
end
