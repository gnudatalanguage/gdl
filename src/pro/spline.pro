function spline,xx, yy, tt, Deriv = deriv
; renamed from original cspline function from idlastro, GD 2021
; original header & content follows, CSPLINE->SPLINE
;+
; NAME:
;      SPLINE
;
; PURPOSE:
;      Function to evaluate a natural cubic spline at specified data points
; EXPLANATION:
;      Combines the Numerical Recipes functions SPL_INIT and SPL_INTERP
;
; CALLING SEQUENCE:
;      result = spline( x, y, t, [ DERIV = ])
;
; INPUTS:
;      x - vector of spline node positions, must be monotonic increasing or
;          decreasing
;      y - vector of node values
;      t - x-positions at which to evaluate the spline, scalar or vector
;
; INPUT-OUTPUT KEYWORD:
;      DERIV - values of the second derivatives of the interpolating function 
;               at the node points.   This is an intermediate step in the 
;               computation of the natural spline that requires only the X and 
;               Y vectors.    If repeated interpolation is to be applied to 
;               the same (X,Y) pair, then some computation time can be saved 
;               by supplying the DERIV keyword on each call.   On the first call
;               DERIV will be computed and returned on output.    
;
; OUTPUT:
;       the values for positions t are returned as the function value
;       If any of the input variables are double precision, then the output will
;       also be double precision; otherwise the output is floating point.
;
; EXAMPLE:                               
;       The following uses the example vectors from the SPL_INTERP documentation
;
;       IDL> x = (findgen(21)/20.0)*2.0*!PI ;X vector
;       IDL> y = sin(x)                     ;Y vector
;       IDL> t = (findgen(11)/11.0)*!PI     ;Values at which to interpolate 
;       IDL> cgplot,x,y,psym=1                ;Plot original grid
;       IDL> cgplot, /over, t,spline(x,y,t),psym=2 ;Overplot interpolated values
;
; METHOD:
;      The "Numerical Recipes" implementation of the natural cubic spline is 
;      used, by calling the intrinsic IDL functions SPL_INIT and SPL_INTERP.
;
; HISTORY:
;      version 1  D. Lindler  May, 1989
;      version 2  W. Landsman April, 1997
;      Rewrite using the intrinsic SPL_INIT & SPL_INTERP functions
;      Converted to IDL V5.0   W. Landsman   September 1997
;      Work for monotonic decreasing X vector    W. Landsman   February 1999
;-
;--------------------------------------------------------------------------

 On_error,2
 compile_opt idl2

 if N_params() LT 3 then begin
        print,'Syntax:  result = spline( x, y, t, [ DERIV = ] )'
        return,-1
 endif 
                
 n = N_elements(xx)
 if xx[n-1] LT xx[0] then begin               ;Descending order?
        xrev = reverse(xx)
        yrev = reverse(yy)
        if N_elements(Deriv) NE n then begin
                 if min( xx - xx[1:*]) LT 0 then $
                          message,'ERROR - Input vector not monotonic' 
                 deriv = spl_init( xrev, yrev)
        endif
        return, spl_interp( xrev, yrev, deriv, tt)
 endif

 if N_elements(Deriv) NE n then deriv = spl_init( xx, yy)
 return, spl_interp( xx, yy, deriv, tt)

 end
