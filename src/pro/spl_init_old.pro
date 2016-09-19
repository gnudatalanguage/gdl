;+
; NAME:	
;             SPL_INIT
;
; DESCRIPTION:
;   Given arrays X and Y of length N containing a tabulated function, i.e.
;   Yi = f(Xi), with X1 > X2 > ... > Xn, and given values YP1 and YPN for the
;   first derivative of the interpolating function at points 1 and N,
;   respectively, this routine returns and array Y2 of length N which contains
;   the second derivatives of the interpolating function at the tabulated 
;   points Xi.  If YP1 and/or YPN are equal to 1.E30 or larger, the routine 
;   is signalled to set the corresponding boundary condition for a natural 
;   spline, with zero second derivative on that boundary.
;
;   This routine is a replacement for the IDL intrinsic function
;   to be used with GDL while this latter does not have it as an
;   intrinsic.
;
;   Note that the keyword DOUBLE of the IDL intrinsic function is ignored.
;   Tests in single precision have shown that the routine is exactly
;   similar to the IDL function.
;   SPL_INTERP is also recoded similarly.
;
; SOURCE:
;	Numerical Recipes, 1986. (page 88)
; 
; CALLING SEQUENCE:
;	y2 = SPL_INIT( x, y, YP0=yp1, YPN_1=ypn)
;
; INPUTS:
;	x - independent variable vector
;	y - dependent variable vector
;	yp1 - first derivative at x(0)
;	ypn - first derivative at x(n-1)
;
; OUTPUTS:
;	y2 - second derivatives at all x, of length n
;
; HISTORY:
;	-- converted to IDL, D. Neill, October, 1991
;       -- arranged as a substitution for SPL_INIT (for use in GDL)
;          Ph. Prugniel, 2008/02/29
;       -- correction a bug for SIG by AC on 2009/08/27
;       -- renamed a SPL_INIT_OLD since a C++ version implemented in
;          GDL, A. Coulais, 2009/08/27
;
; -----------------------------------------------------------------------------
; NOTE:
; Name this function: spl_init to use it as a replacement of the IDL intrinsic
;      when using GDL
; But, to make a comparison of numerical results with the IDL intrinsic
; function, as it is made in the attached program: test_splf, change its
; name in "psplinf", so that IDL can execute either its intrinsic or
; the substitute.
; Anyway, this comparison has been made with success and if you just
; want to use this function in GDL ... ignore this remark
; -----------------------------------------------------------------------------

;function PSPLINF, x, y, YP0=yp1, YPN_1=ypn, DOUBLE=double
function SPL_INIT_OLD, x, y, YP0=yp1, YPN_1=ypn, DOUBLE=double, debug=debug

print, 'GDL syntax, a C++ version is now available'

n = N_ELEMENTS(x)

; we should use the same type as for the input y !
y2 = DBLARR(n)
u = DBLARR(n)
;
; The lower boundary condition is set either to be "natural"
;
if (N_ELEMENTS(yp1) EQ 0) then begin
    y2[0] = 0.
    u[0] = 0.
    ;;
    ;; or else to have a specified first derivative
    ;;
endif else begin
    y2[0] = -0.5
    u[0] = ( 3. / ( x[1]-x[0] ) ) * ( ( y[1]-y[0] ) / ( x[1]-x[0] ) - yp1 )
endelse

; I suppose we can also take advantage here of the TRISOL function
; from IDL... we can remove the for loops
;
; This is the decomposition loop of the tridiagonal algorithm.  Y2 and
; U are used for temporary storage of the decomposed factors.
;

; AC 09/08/25 : bug which may appear with unsorted X data ...
psig_old = DOUBLE((x - SHIFT(x, -1))) / (SHIFT(x, +1) - SHIFT(x, -1))
psig=psig_old
for ii=1, n-2 do psig[ii]=(x[ii]-x[ii-1])/((x[ii+1]-x[ii-1]))

if KEYWORD_SET(debug) then begin
   print, psig_old
   print, psig
endif

pu = (DOUBLE(SHIFT(y,-1) - y) / (SHIFT(x,-1) - x) - $
      (y - SHIFT(y,1)) / (x - SHIFT(x,1))) / (SHIFT(x,-1)- SHIFT(x,1))

for i=1,n-2 do begin
    p = psig[i] * y2[i-1] + 2.0D
    y2[i] = ( psig[i]-1.0D ) / p
    u[i]=( 6.0D * pu[i] - psig[i]*u[i-1] ) / p
    ;;if KEYWORD_SET(debug) then print, i, psig[i], pu[i], p
endfor

;
; The upper boundary condition is set either to be "natural"
;
if n_elements(ypn) eq 0 then begin
    qn=0.
    un=0.
;
; or else to have a specified first deriviative
;
endif else begin
    qn=0.5
    dx=x[n-1]-x[n-2]
    un=(3./dx)*(ypn-(y[n-1]-y[n-2])/dx)
endelse
;
y2[n-1] = ( un - qn * u[n-2] ) / ( qn * y2[n-2] + 1. )

;
; This is the backsubstitution loop of the tridiagonal algorithm
;
if KEYWORD_SET(debug) then begin
   print, "psig:",psig
   print, "pu:",pu
   for ii=0, n-1 do print, ii, y2[ii] , u[ii]
endif

for k=n-2,0,-1 do begin
    y2[k] = y2[k] * y2[k+1] + u[k]
endfor

;
return, y2

end	; spl_init.pro

