pro test_correlate

  err = 0
  
  ; vectors strictly anti-corelated
  if correlate([0,1,2], [2,1,0]) ne -1 then begin
    message, '1', /conti
    err++
  endif
  
  ; correlation of two constant vectors -> nan
  if ~finite(correlate([0,0,0], [1,1,1]), /nan) then begin
    message, '2', /conti
    err++
  endif

  ; covariance of two constant vectors -> 0
  if correlate([0,0,0], [1,1,1], /cov) ne 0. then begin
    message, '3', /conti
    err++
  endif

  ; correlation of two vectors of different length
  a = [1,2,3]
  b = [2,3,4,5,6]
  if correlate(a,b) ne correlate(b,a) or correlate(a,b) ne correlate(a,b[0:2]) then begin
    message, '4', /conti
    err++
  endif

  if err ne 0 then exit, status=-1

end
