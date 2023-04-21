FUNCTION LAPLACIAN,ARRAY, MISSING=MISSING, ADD_BACK=ADD_BACK,$
  BIAS=BIAS, CENTER=CENTER, EDGE_TRUNCATE=EDGE_TRUNCATE,$
  EDGE_WRAP=EDGE_WRAP, EDGE_ZERO=EDGE_ZERO, INVALID=INVALID,$
     KERNEL_SIZE=KERNEL_SIZE, NAN=NAN, NORMALIZE=NORMALIZE 
;+
; Borrowed from
; Project     : SOHO - CDS
;
; Name        : 
;	LAPLACIAN()
; Purpose     : 
;	Calculate the Laplacian of an array.
; Explanation : 
;	The numerical second derivative is calculated in the X and Y
;	directions, and then added together.
; Use         : 
;	Result = LAPLACIAN2(ARRAY)
; Inputs      : 
;	ARRAY	= Image to take Laplacian of.
; Opt. Inputs : 
;	None.
; Outputs     : 
;	Result of function is the Laplacian of the array.
; Opt. Outputs: 
;	None.
; Keywords    : 
;	MISSING	 = Value flagging missing pixels.  Any such pixels are not
;		   included in calculating the Laplacian.  The output value for
;		   any point with a missing pixel adjacent to it will be set to
;		   the missing pixel value.
; Common      : 
;	None.
; Restrictions: 
;	The image array must be two-dimensional.
;
;	In general, the SERTS image display routines use several non-standard
;	system variables.  These system variables are defined in the procedure
;	IMAGELIB.  It is suggested that the command IMAGELIB be placed in the
;	user's IDL_STARTUP file.
;
;	Some routines also require the SERTS graphics devices software,
;	generally found in a parallel directory at the site where this software
;	was obtained.  Those routines have their own special system variables.
;
; Side effects: 
;	None.
; Category    : 
;	Utilities, Image_display.
; Prev. Hist. : 
;	William Thompson, March 1991.
;	William Thompson, August 1992, renamed BADPIXEL to MISSING.
; Written     : 
;	William Thompson, GSFC, March 1991.
; Modified    : 
;	Version 1, William Thompson, GSFC, 13 May 1993.
;		Incorporated into CDS library.
;       Version 2, 11-May-2005, William Thompson, GSFC
;               Handle NaN values
;       Version 3, 25-Jun-2009, Zarro (ADNET)
;               Renamed to LAPLACIAN2 to avoid conflict with ITTVIS
;               version
; GD, Apr 2023: added a slightly edited version to GDL procedures.
;-
;
;
;  Check the size of the image array.
;
  S = SIZE(ARRAY)
  IF S(0) NE 2 THEN BEGIN
     PRINT,'*** Variable must be two-dimensional, name= ARRAY, ' + $
        'routine LAPLACIAN.'
     RETURN,0
  ENDIF
  NX = S(1)
  NY = S(2)
;
;  Calculate the numerical derivatives in the center of the image array.
;
  D2X = 0.*ARRAY  &  D2Y = D2X
  D2X(1,0) = 2.*ARRAY(1:NX-2,*) - ARRAY(2:NX-1,*) - ARRAY(0:NX-3,*)
  D2Y(0,1) = 2.*ARRAY(*,1:NY-2) - ARRAY(*,2:NY-1) - ARRAY(*,0:NY-3)
;
;  Set the edges equal to the next row/column.
;
  D2X(0,0) = D2X(1,*)  &  D2X(NX-1,0) = D2X(NX-2,*)
  D2Y(0,0) = D2Y(*,1)  &  D2Y(0,NY-1) = D2Y(*,NY-2)
;
;  Calculate the Laplacian as the sum.
;
  LAP = D2X + D2Y
;
;  If the missing pixel flag is set, calculate the X and Y positions of the
;  missing pixels.
;
  if n_elements(missing) gt 0 then begin
     w = where((finite(array) ne 1) or (array eq missing), count)
     IF count GT 0 THEN BEGIN
        LAP_MISSING = ARRAY[W[0]]
        LAP(W) = LAP_MISSING
        X = W MOD NX
        Y = W / NX
;
;  Set all pixels next to a missing pixel equal to the missing pixel flag
;  value.
;
        LAP( ((X-1) > 0)      + NX*Y ) = LAP_MISSING ;Left
        LAP( ((X+1) < (NX-1)) + NX*Y ) = LAP_MISSING ;Right
        LAP( X + ((Y-1) > 0)     *NX ) = LAP_MISSING ;Below
        LAP( X + ((Y+1) < (NX-1))*NX ) = LAP_MISSING ;Above
     ENDIF
  ENDIF
;
  if keyword_set(add_back) then RETURN, LAP+ARRAY
  RETURN, LAP
END


