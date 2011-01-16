pro test_bug_2610174

  set_plot, 'z'
  plot, [0]
  xyouts, 0, 0, '==', width=wi2_1, charsize=1

  xyouts, 0, 0, '====', width=wi4_1
  if wi2_1 ne .5 * wi4_1 then exit, status=1

  xyouts, 0, 0, '==', width=wi2_2, charsize=2
  if wi2_1 ne .5 * wi2_2 then exit, status=1

end
