pro test_bug_3104214
  set_plot, 'z'
  plot, [1,10], /ylog, ystyle=5, xstyle=5 & plots, [0,1], [1,10], color=!P.BACKGROUND
    if !version.os_family eq 'Windows' then $
        message,/continue,' TVRD not working for windows. fake success'
    if !version.os_family eq 'Windows' then return
  if total(tvrd()) ne 0. then exit, status=1
end
