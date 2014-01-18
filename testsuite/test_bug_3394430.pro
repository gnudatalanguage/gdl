pro test_bug_3394430
  set_plot, 'z'
  !p.multi=[0,2,2]
  plot, findgen(10)
  polyfill, [2,4,2], [1,1,4]
  if !p.multi[1] ne 2 or !p.multi[2] ne 2 then exit, status=1
end
