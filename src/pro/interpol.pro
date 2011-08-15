; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
; relies on findex.pro by Paul Ricchiazzi
function interpol, p0, p1, p2, lsquadratic=lsquadratic, quadratic=quadratic, spline=spline
  on_error, 2

  ; sanity checks
  if n_params() eq 1 then $
    message, 'Two or three parameters required'
  if keyword_set(lsquadratic) then $
    message, 'LSQUADRATIC keyword not supported yet (FIXME!)'
  if keyword_set(quadratic) then $
    message, 'QUADRATIC keyword not supported yet (FIXME!)'

;  if n_params() eq 3 and n_elements(p0) ne n_elements(p1) then $
;    message, 'In the three-parameter case the first and second argument must be of equal length'
; <see bug no. 3104537>
  if n_params() eq 3 then begin
    if n_elements(p0) ne n_elements(p1) then $
      message, 'In the three-parameter case the first and second argument must be of equal length'
    all_equal_test=abs((p1 - shift(p1,+1))(1:*))
    if min(temporary(all_equal_test)) eq 0 then message, $ ; usually only triggered for integer arrarys
      'In the three-parameter case, the second argument must be strictly increasing or strictly decreasing.'
  endif
; </...>

  isint = size(p0, /ty) lt 4 || size(p0, /ty) gt 11

  if n_params() eq 2 then begin
    ; regular grid case
    if size(p1, /dimensions) eq 0 then begin
      ind = findgen(p1) / (p1 - (p1 eq 1 ? 0 : 1)) * (n_elements(p0) - 1)
    endif else begin
      message, 'In the two-parameter case the second parameter must be a scalar'
      ; TODO: IDL does something else here...
    endelse
  endif else if ~keyword_set(spline) then begin
    ; irregular grid case
    ind = findex(p1, p2)
  endif

  if keyword_set(spline) then begin
    ; spline case
    if n_params() eq 2 then begin
      x = findgen(n_elements(p0))
      y = spl_interp(x, p0, spl_init(x, p0), ind)
    endif else begin
      y = spl_interp(p1, p0, spl_init(p1, p0), p2)
    endelse
    return, fix(temporary(y), type=size(p0, /type))
  endif else begin
    ; linear interpolation case
    return, interpolate(isint ? float(p0) : p0, ind)
  endelse

end
