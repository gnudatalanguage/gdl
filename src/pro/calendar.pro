; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
; depends on the UNIX 'cal' utility 

pro calendar, p1, p2
  on_error, 2

  ; sanity checks
  msg = ' must not be undefined, zero-valued nor multi-element value'
  msg_y = 'valid year range is 1 ... 9999'
  msg_m = 'valid month range is 1 ... 12'
  m_range = [1,12]
  y_range = [1,9999]
  if n_params() eq 1 then begin
    if ~keyword_set(p1) || n_elements(p1) ne 1 then $
      message, 'first argument' + msg
    if p1 lt y_range[0] || p1 gt y_range[1] then message, msg_y
  endif
  if n_params() eq 2 then begin
    if ~keyword_set(p2) || n_elements(p2) ne 1 then $
      message, 'second argument' + msg
    if p1 lt m_range[0] || p1 gt m_range[1] then message, msg_m
    if p2 lt y_range[0] || p2 gt y_range[1] then message, msg_y
  endif

  ; acquireing the calendar from cal
  cmd = 'LC_ALL=C cal'
  chrsz = 3
  if n_params() eq 1 then begin
    cmd += ' ' + string(p1) 
    chrsz = 1
  endif else if n_params() eq 2 then begin
    cmd += ' ' + string(p1) + ' ' + string(p2)
  endif
  spawn, cmd, text, exit_status=status, err
  if status ne 0 then message, 'failed to execute cal'
  
  ; finding the length of the longest line
  maxlen = 0.
  for line = 0, n_elements(text) - 1 do $
    maxlen = max([maxlen, strlen(text[line])])

  ; setting up the plot space
  x_rng = [-0.05,1.05] * maxlen
  y_rng = [1,-.1] * n_elements(text)
  plot, [0], [0], xrange=x_rng, yrange=y_rng, /nodata, $
    xmargin=[1,1], ymargin=[1,1], xstyle=5, ystyle=5

  ; drawing a frame
  plots, $
    [x_rng[0], x_rng[0], x_rng[1], x_rng[1], x_rng[0]], $
    [y_rng[0], y_rng[1], y_rng[1], y_rng[0], y_rng[0]]

  ; plotting the output from cal
  for line = 0, n_elements(text) - 1 do $
    for char = 0, strlen(text[line]) do $
      xyouts, char, line, strmid(text[line], char, 1), chars=chrsz

end
