; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
pro test_interpol

  ; test data - a parabola probed at four points
  x = [0.,1.,2.,3.]
  y = x*x

  ; 2-parameter case, linear interpolation, sanity check
  if ~array_equal(y, interpol(y, 4)) then begin
    message, '2p, linear', /conti
    exit, status=1
  endif 

  ; 2-parameter case, linear vs. spline interpolation
  wh = where(interpol(y, 7, /spline) gt interpol(y, 7), cnt)
  if cnt ne 0 then begin
    message, '2p, spline', /conti
    exit, status=1
  endif

  ; 3-parameter case, linear vs. spline interpolation
  mid = [.5,1.5,2.5]
  wh = where(interpol(y, x, mid, /spline) gt interpol(y, x, mid), cnt)
  if cnt ne 0 then begin
    message, '3p, spline', /conti
    exit, status=1
  endif

  ; ensuring interpol(/spline) returns the same as spl_interp() 
  ; (intentionally swapping x,y with y,x)
  if ~array_equal( $
    interpol(x, y, mid, /spline), $
    spl_interp(y, x, spl_init(y, x), mid) $
  ) then begin
    message, 'interpol(/spline) != spl_interp', /conti
    exit, status=1
  endif

end
