; for widget_control,/reset and also used in .reset
pro gdl_reset_widgets
  COMPILE_OPT idl2, HIDDEN
  list=widget_info(/man)
  if (list[0] ne 0) then begin
     n=n_elements(list)
     for i=n-1,0,-1 do widget_control,list[i],/destroy
  endif
end
