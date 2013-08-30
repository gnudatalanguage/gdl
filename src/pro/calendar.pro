; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
; depends on the UNIX 'cal' utility 
;
pro CALENDAR, p1, p2, test=test
;
ON_ERROR, 2
;
;; sanity checks
msg = ' must not be undefined, zero-valued nor multi-element value'
msg_y = 'valid year range is 1 ... 9999'
msg_m = 'valid month range is 1 ... 12'
m_range = [1,12]
y_range = [1,9999]
if N_PARAMS() eq 1 then begin
    p1=FLOOR(p1)
    if ~KEYWORD_SET(p1) || N_ELEMENTS(p1) ne 1 then $
      MESSAGE, 'first argument' + msg
    if p1 lt y_range[0] || p1 gt y_range[1] then MESSAGE, msg_y
endif
if N_PARAMS() eq 2 then begin
    p2=FLOOR(p2)
    if ~KEYWORD_SET(p2) || N_ELEMENTS(p2) ne 1 then $
      MESSAGE, 'second argument' + msg
    if p1 lt m_range[0] || p1 gt m_range[1] then MESSAGE, msg_m
    if p2 lt y_range[0] || p2 gt y_range[1] then MESSAGE, msg_y
endif

;; acquireing the calendar from cal
cmd = 'LC_ALL=C cal'
chrsz = 3
if N_PARAMS() eq 1 then begin
    cmd += ' ' + STRING(p1) 
    chrsz = 1
endif else if N_PARAMS() eq 2 then begin
    cmd += ' ' + STRING(p1) + ' ' + STRING(p2)
endif
spawn, cmd, text, exit_status=status, err
if status ne 0 then MESSAGE, 'failed to execute cal'
;
;; finding the length of the longest line
maxlen = 0.
for line = 0, N_ELEMENTS(text) - 1 do $
  maxlen = max([maxlen, STRLEN(text[line])])

;; setting up the plot space
x_rng = [-0.05,1.05] * maxlen
y_rng = [1,-.1] * N_ELEMENTS(text)
PLOT, [0], [0], xrange=x_rng, yrange=y_rng, /nodata, $
  xmargin=[1,1], ymargin=[1,1], xstyle=5, ystyle=5

;; drawing a frame
PLOTS, $
  [x_rng[0], x_rng[0], x_rng[1], x_rng[1], x_rng[0]], $
  [y_rng[0], y_rng[1], y_rng[1], y_rng[0], y_rng[0]]

;; plotting the output from cal
for line = 0, N_ELEMENTS(text) - 1 do $
  for char = 0, STRLEN(text[line]) do $
  XYOUTS, char, line, STRMID(text[line], char, 1), chars=chrsz
;
if KEYWORD_SET(test) then STOP
;
end
