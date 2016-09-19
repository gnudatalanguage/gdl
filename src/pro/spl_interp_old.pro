;+
; NAME:	
;             SPL_INTERP
;
; DESCRIPTION:
;   Given arrays XA and YA of length N, which tabulate a function (with the
;   XA's in order), and given the array Y2A, which is the output from
;   SPLINE.PRO, this routine returns the cubic-spline interpolated values
;   at the locations of the array X.
;
;   Note that the keyword DOUBLE of the IDL intrinsic function is ignored.
;   Tests in single precision have shown that the routine is exactly
;   similar to the IDL function.
;   SPL_INIT is also recoded similarly.
;
; SOURCE:
;	Numerical Recipes, 1986. (page 89)
; 
; CALLING SEQUENCE:
;	y = spl_interp(xa,ya,y2a,x)
;
; INPUTS:
;	xa - independent variable vector
;	ya - dependent variable vector
;	y2a- second derivative vector from SPLINF.PRO
;	x  - x value of interest
;
; OUTPUTS:
;	y  - cubic-spline interpolated value at x
;
; HISTORY:
;	-- converted to IDL, D. Neill, October, 1991
;       -- arranged as a substitution for SPL_INTERP (for use in GDL)
;          Ph. Prugniel, 2008/02/29
;       -- renamed as SPL_INTERP_OLD since a C++ version implemented in
;          GDL, A. Coulais, 2009/08/27
;-
; -----------------------------------------------------------------------------
; NOTE:
; Name this function: spl_init to use it as a replacement of the IDL intrinsic
;      when using GDL
; But, to make a comparison of numerical results with the IDL intrinsic
; function, as it is made in the attached program: test_splt, change its
; name in:
;   function psplint,xa,ya,y2a,x   ; name used to make comparison test with IDL
; so that IDL can execute either its intrinsic or the substitute.
; Anyway, this comparison has been made with success and if you just
; want to use this function in GDL ... ignore this remark
; -----------------------------------------------------------------------------

;function PSPLINT, xa, ya, y2a, x, DOUBLE=double
function SPL_INTERP_OLD, xa, ya, y2a, x, DOUBLE=double

print, 'GDL syntax, a C++ version is now available'

n = N_ELEMENTS(xa)
klo = VALUE_LOCATE(xa, x)  > 0L < (n-2)
khi = klo + 1
;
; KLO and KHI now bracket the input value of X
;

if MIN(xa[khi]-xa[klo]) eq 0 then $
  message, 'SPLINT - XA inputs must be distinct'
;
; Cubic spline polynomial is now evaluated
;
h = xa[khi] - xa[klo]

a = ( xa[khi] - x ) / h
b = ( x - xa[klo] ) / h

return, a * ya[klo] + b * ya[khi] + $
    ( ( a^3 - a ) * y2a[klo] + ( b^3 - b ) * y2a[khi] ) * ( h^2 ) / 6.D

end	; spl_interp.pro

