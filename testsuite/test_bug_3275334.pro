pro test_bug_3275334
  if GETENV('DISPLAY') eq '' then exit, status=77
  set_plot, 'z' & tv, dist(10) & set_plot, 'x' & tv, tvrd()
end
