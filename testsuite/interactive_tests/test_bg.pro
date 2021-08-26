pro bgrp_event, event

  widget_control, event.top, get_uvalue = state
  widget_control, event.id, get_uvalue = uv

  case uv of
     'QUIT': widget_control, event.top, /destroy
     'SHOW': begin
        widget_control, state.excl1, get_value = ve1
        widget_control, state.excl2, get_value = ve2
        widget_control, state.nexcl, get_value = vne
        print, "Exclusive 1:   ", ve1
        print, "Exclusive 2:   ", ve2
        print, "Non exclusive: ", vne
     end
     'RESET': begin
        widget_control, state.excl1, set_value = 1
        widget_control, state.excl2, set_value = 2
        widget_control, state.nexcl, set_value = [0, 1, 1, 0]
     end
     else: print, uv, ' ', event.value, $
                  event.select ? ' Press' : ' Release'
  endcase

end

pro test_bg

  base = widget_base(/col)

  junk = cw_bgroup(base, $
                   ['one', 'two', 'three', 'four', 'five', 'six', $
                   'seven', 'eight', 'nine', 'ten', 'eleven', 'twelve'], $
                   /row, $
                   uvalue = 'IDXR', $
                   label_top = 'Return Index', $
                   /scroll)
  junk = cw_bgroup(base, $
                   ['one', 'two', 'three', 'four'], $
                   /row, $
                   /return_name, $
                   uvalue = 'NMR', $
                   label_left = 'Return Name', $
                   /frame)
  junk = cw_bgroup(base, $
                   ['one', 'two', 'three', 'four'], $
                   /row, $
                   uvalue = 'BIDR', $
                  /return_id, $
                   label_left = 'Return ID')
  
  junk = cw_bgroup(base, $
                   ['one', 'two', 'three', 'four'], $
                   button_uv = ['first', 'second', 'third','fourth'], $
                   /row, $
                   uvalue = 'UVR', $
                   label_left = 'Return uvalue')

  jb = widget_base(base, $
                   /row)
  excl1 = cw_bgroup(jb, $
                    ['one', 'two', 'three', 'four'], $
                    /col, $
                    uvalue = 'EXR', $
                    /exclusive, $
                    set_value = 1, $
                    label_top = 'Exclusive')
  
  excl2 = cw_bgroup(jb, $
                    ['one', 'two', 'three', 'four'], $
                    /col, $
                    uvalue = 'EXR2', $
                    /exclusive, $
                    set_value = 2, $
                    /no_release, $
                    label_top = 'Exclusive (NR)')

  nexcl = cw_bgroup(jb, $
                    ['one', 'two', 'three', 'four'], $
                    /col, $
                    uvalue = 'NXR', $
                    /nonexclusive, $
                    set_value = [0, 1, 1, 0], $
                    label_top = 'Non-exclusive')

  jb = widget_base(base, $
                   /row)
  junk = widget_button(jb, $
                       value = 'Show values', $
                       uvalue = 'SHOW')
  junk = widget_button(jb, $
                       value = 'Reset', $
                       uvalue = 'RESET')
  junk = widget_button(base, $
                       value = 'Quit', $
                       uvalue = 'QUIT')
  
  widget_control, base, /real, set_uvalue = {excl1: excl1,  $
                                             excl2: excl2, $
                                             nexcl: nexcl}
  xmanager, 'bgrp', base

end
