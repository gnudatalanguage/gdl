; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
pro test_pmulti

  foreach dev, ['z', 'ps', 'svg'] do begin

    set_plot, dev
    if dev ne 'z' then device, filename='/dev/null'

    !P.MULTI=[1,3,1] & plot, [0] 
    if !P.MULTI[0] ne 0 then begin
      message, '!P.MULTI[1,3,1] check failed for device >>' + dev + '<<', /conti
      exit, status=1
    endif

    !P.MULTI=[2,3,1] & plot, [0] 
    if !P.MULTI[0] ne 1 then begin
      message, '!P.MULTI[2,3,1] check failed for device >>' + dev + '<<', /conti
      exit, status=1
    endif
 
  endforeach

end
