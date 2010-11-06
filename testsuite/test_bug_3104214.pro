pro test_bug_3104214
  set_plot, 'z'
  plot, [1,10], /ylog, ystyle=5, xstyle=5 & plots, [0,1], [1,10], color=!P.BACKGROUND
  if total(tvrd()) ne 0. then exit, status=1
end
