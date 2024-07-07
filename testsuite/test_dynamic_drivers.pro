; Should test if GDL uses 'our' drivers, and if not, it must mean that
; the installed plplot is not linked with option 'DYNDRIVERS'
; If this is not the case, GDL WILL NOT WORK!
;

pro test_dynamic_drivers
  ret=execute("set_plot,'z'")
  if (ret ne 1) then exit       ; no need to go further, no useful devices.
  
  surfr                         ; set up default 3D
  !P.T3D=1
  set_plot,'z'
  device,set_resolution=[100,100]
  ; plot a box passing at pixel [0,0] if unprojected,
  ; which would be the case if the driver is not 'ours'.
  ; this trick works only with axes, as the other drawings use normal
  ; plplot lines, having reprojected the points ourselves.
  plot,[0,1],[0,1],/nodata,pos=[0,0,1,1]
  a=tvrd()
  ; the 3D projection makes a[0]=0b
  ; when our driver is used. Otherwise
  ; no 3D effect and a[0]=255b
  if (a[0] ne 0b) then EXIT, status=1
end
