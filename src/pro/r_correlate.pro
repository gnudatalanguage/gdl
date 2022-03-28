;+
; R_CORRELATE
; 	First attempt at a partial implementation of the IDL
; 	R_CORRELATE function.
;
; Usage:
; 	rho = r_correlate(x, y)
;
; Arguments:
; 	x, y	(real)	The values to be correlated.
;
; Keywords:
; 	d	float	A variable to return the sum-squared
; 			difference of ranks.
; 	/kendall	If set, then compute the Kendall tau
; 			correlation. 
;
; Notes:
; 	For now the PROBD & ZD outputs are not implemented.
;
; History:
;	First cut: 1/5/18; SJT
;	Add kendall: 5/1/21; SJT
;-
; LICENCE:
; Copyright (C) 2018-2021: SJT
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   

function get_rank, x, f3f

; GET_RANK
; 	Return the ranks of the elements in an array. Internal routine
; 	for R_CORRELATE
;
; Usage:
;	rank = get_rank(x[, f3f])
;
; Returns:
;	The list of ranks, for equal elements the ranks are averaged
;	-- hence it is a floating point value.
;
; Argument:
; 	x	sortable The array to be ranked, any type that can be
; 			sorted will work.
; 	f3f	long	sum(f^3-f), where f is the number of elements
; 			in each group of equal values.
;
; History:
; 	Original: 1/5/18; SJT
; 	More efficient algorithm: 2/5/18; SJT


  compile_opt idl2, hidden
  
  rank = fltarr(n_elements(x))
  idx = sort(x)
  xs = double(x[idx])
  
  ux = uniq(xs) & nux = n_elements(ux)
  ux = [-1, ux]

  for j = 0, nux-1 do xs[ux[j]+1:ux[j+1]] = float(ux[j]+1+ux[j+1])/2.
  rank[idx] = xs

  if arg_present(f3f) then begin
     f = ux[1:*] - ux
     f3f =  total(f^3-f)
  endif
  
  return, rank
end
     
function betacf, a, b, x, itmax = itmax, eps = eps, fpmin = fpmin
  compile_opt idl2,  hidden

; Set limits if they are not passed
  if ~keyword_set(itmax) then itmax = 100l
  if ~keyword_set(eps) then eps = 3.d-7
  if ~keyword_set(fpmin) then fpmin = 1.d-30
  
  ab = a + b
  ap1 = a + 1
  am1 = a - 1

  c = 1.d
  d = 1.d - ab*x/ap1
  if abs(d) lt fpmin then d = fpmin
  d = 1.d/d
  h = d

  j = 0l
  repeat begin
     j++
     if j gt itmax then begin
        message, /cont, "A or B too large or ITMAX too " + $
                 "small."
        break
     endif

     j2 = 2l*j
     aa = j*(b-j)*x / ((am1+j2)*(a+j2))
     d = 1.d + aa*d
     if abs(d) lt fpmin then d = fpmin

     c = 1.d + aa/c
     if abs(c) lt fpmin then c = fpmin
     d = 1./d

     h *= d*c
     aa = -(a+j)*(ab+j)*x/((a+j2)*(ap1+j2))
     d = 1.d + aa*d
     if abs(d) lt fpmin then d = fpmin

     c = 1.d + aa/c
     if abs(c) lt fpmin then c = fpmin
     d = 1./d

     dc = d*c
     h *= dc
  endrep until abs(dc-1.d) lt eps 

  return, h
end
  
function betai, a, b, x, _extra = _extra

; BETAI
; 	The incomplete BETA function. Internal routine for R_CORRELATE
;
; Usage:
; 	i = betai(a, b, x)
;
; Returns:
; 	The incomplete beta function for the arguments given.
;
; Arguments:
; 	a	real	First argument
; 	b	real	Second argument
; 	x	real	The integration limit
;
; Keywords:
;	itmax	int	The maximum number of iterations.
;	eps	real	The tolerance for the integration
;	fpmin	real	A small value to prevent divisions by zero.
;
; Notes:
;	Based on the procedure in Numerical methods.
;	Not a replacement for IDL's routine as it does
;	not support complex values, nor array arguments.
;
; History:
; 	Original: 3-4/5/18; SJT

  compile_opt idl2, hidden

  if x lt 0.d || x gt 1.d then begin
     message, /cont, "Invalid value of X ("+string(x)+")."
     return, !values.d_nan
  endif

  if x eq 0.d || x eq 1.d then bt = 0.d $
  else bt = exp(lngamma(a+b) - lngamma(a) - lngamma(b) + $
                a*alog(x) + b*alog(1.d - x))

  if x lt (a+1.)/(a+b+2.) then $
     bti = bt*betacf(a, b, x, _extra = _extra) / a $
  else bti =  1.d - bt*betacf(b, a, 1.d - x, _extra = _extra) / b

  return, bti

end

function concordance, x, y, double = dp

; CONCORDANCE
; 	Compute the ranking concordance of two arrays.
;
; Usage:
; 	ccd = concordance(x, y)
;
; Arguments:
; 	x, y	numeric	Two arrays of equal length, both sortable
; 			 numeric types.
;
; Keyword:
; 	/double		If set, then the differencing will be doube in
; 			double precision, even if the type does not
; 			require it.
;
; History:
; 	Original: 4/5/18; SJT
; 	Add double key: 8/5/18; SJT

  compile_opt idl2, hidden

  rv = dblarr(4)

  nx = n_elements(x)
  if nx ne n_elements(y) then begin
     message, /cont, "Arrays must be the same length."
     return, rv
  endif

  if keyword_set(double) then begin
     xx = double(x[*])
     yy = double(y[*])
  endif else begin
     switch size(x, /type) of
        1:
        2:
        12: begin
           xx = float(x[*])
           break
        end
        3:
        13:
        14:
        15: begin
           xx = double(x[*])
           break
        end
        4:
        5: begin
           xx = x[*]
           break
        end
        else: begin
           message, "Invalid data type for X"
           return, !values.d_nan
        end
     endswitch
     switch size(y, /type) of
        1:
        2:
        12: begin
           yy = float(y[*])
           break
        end
        3:
        13:
        14:
        15: begin
           yy = double(y[*])
           break
        end
        4:
        5: begin
           yy = y[*]
           break
        end
        else: begin
           message, "Invalid data type for Y"
           return, replicate(!values.d_nan, 4)
        end
     endswitch
  endelse
  
  for i = 1l, nx-1 do begin
     dx = xx-xx[i:*]
     dy = yy-yy[i:*]
     dxxdy = dx*dy
     rv += [total(dxxdy gt 0), $
            total(dxxdy lt 0), $
            total(dy eq 0 and dx ne 0), $
            total(dx eq 0 and dy ne 0)]
  endfor
  return, rv
end

function kendall, x, y, probability = pr, double = dp
  
; KENDALL
; 	Return the Kendall tau correlation.
;
; Usage:
;	tau = kendall(x,y)
;
; Returns:
; 	The Kendall tau rank correlation.
;
; Arguments:
; 	x, y	numeric	The arrays to be correlated. Must be numeric
; 			and sortable, and of the same length.
;
; Keyword:
; 	probability	double	A variable to return the probability
; 				that the variables are uncorrelated.
; 	/double			If set then internal computations are
; 				done in double precision. The results
; 				are always double as total is used.
;
; Notes:
; 	This is about 25 times quicker than r_correlate(.../kendall)
;
; History:
; 	Original: 4/5/18; SJT
; 	Add double keyword: 8/5/18; SJT

  compile_opt idl2, hidden
  
  ccd = concordance(x, y, double = dp)

  tau = $
     (ccd[0]-ccd[1])/(sqrt(ccd[0]+ccd[1]+ccd[2])*sqrt(ccd[0]+ccd[1]+ccd[3]))

  if arg_present(pr) then begin
     n = n_elements(x)
     v = (4.d*n + 10.d) / (9.d*n*(n-1))
     z = tau/sqrt(v)
     pr = erfc(abs(z)/sqrt(2.d))
  endif

  return, tau
end

function r_correlate, x, y, d = d, kendall = kendall

  compile_opt idl2
  
  nvals = n_elements(x)
  if nvals ne n_elements(y) then begin
     message, /cont, "Input arrays must be of equal length"
     return,[0.,0.]
  endif

  if keyword_set(kendall) then begin
     t = kendall(x, y, prob = p)
     return, [t, p]
  endif
  
  rx = get_rank(x, sx)
  ry = get_rank(y, sy)

  d =  total((rx-ry)^2)
  
  rxa = mean(rx)
  rya = mean(ry)

  rho = total((rx - rxa)*(ry - rya)) / $
        (sqrt(total((rx - rxa)^2)) * sqrt(total((ry - rya)^2)))

  if abs(rho) ge 1 then tt = 0.d $
  else begin
     df = nvals - 2.d
     t = rho * sqrt(df/(1.-rho^2))
     tt = betai(df/2.d, 0.5d, df/(df+t^2))
     bb = t_pdf(double(t), nvals-2)
  endelse
  return, [rho, tt]
end
