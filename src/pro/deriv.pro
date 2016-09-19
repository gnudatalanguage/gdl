;+
; NAME:      DERIV
;
; PURPOSE:  
; This function performs a numerical differentiation
; using a 3 points interpolation in Lagrangian way.
; The advantage of this procedure is we don't have to know the
; analytical equation of the "tabulated" data. 
; Lagrangian interpolation means we "fit" the data with a polynomial
; which cross the data at collocation points. The fact we assume we
; have a polynomial give an explicit formula for the derivative.
;
; When only Y (data) are provided, a constant step of 1 is assumed
; between x-positions. Except at the edges, the formula is very simple.
;
; When X (positions) and Y (data) are provided, a more complex computation
; is donne.
;
; See below (in Section PROCEDURE) where you can found the formulae
;
; CATEGORY:  Numerical analysis.
;
; CALLING SEQUENCE: 
;      result = DERIV, [x], y, [help=help], [test=test], [check=check]
;
; -- result = DERIV(data)              (also : d=DERIV(Y))
; -- result = DERIV(positions, data)   (also : d=DERIV(X,Y))
;
; INPUTS:
; -- When only one array (X) is provided, X contains the data to
;    be differentiated ()
; -- When two arrays are provided
;          X is the array containing the positions of data (colocations)
;          Y is the array containing the data
;
; OPTIONAL INPUTS:    none
;
; KEYWORD PARAMETERS:
; -- no_check : since we do extra checks, do not do them !
;    (DESACTIVATED ! but we keep it for compatility issue)
; -- check : you can ask for extra tests on input
;
; -- Help : give a very simple summary of procedure then exit
; -- Test : will stop just before the return statement
;    (useful for internal checking/debugging)
;
; OUTPUTS:  Return the numerical derivative (Lagrangian formalism)
;           Please note that the output type will be Float OR Double
;
; OPTIONAL OUTPUTS: none
;
; COMMON BLOCKS:    none
;
; SIDE EFFECTS:     none
;
; RESTRICTIONS:     - We do check if the length of the 2 arrays (if 2)
;                     are the same (as in IDL)
;                   - We can check if the x(n+1)-x(n) is <> 0. (not
;                     done in IDL)
;                   - We can check if the X array is well sorted (not
;                     done in IDL)
;                   - We can check if Delta_x is constant or not (not
;                     done in IDL)
;                   - Neither IDL version or GDL version manage NaN values
;
; COMPATIBILITY:  
; - this function is NOT FULLY compatible with the IDL one.
; NEVERTHELESS, it should give the same output with the same "normal" input(s).
; We DO have ADD some checks (zero steps, size of Y array, sorting,
; shortcut for constant step) which were missing in initial procedure.
; This will give clear warning when a problem is encountered.
;
; WARNING: 
; - NaN values are maybe not fully managed now
;
; PROCEDURE:  
; - in the IDL version, it is written that the procedure is described
;   in Hildebrand, Introduction to Numerical Analysis, Mc Graw Hill,
;   1956, p. 82 (see IMCCE library, Bat A, Floor 2, Paris
;   Observatory). But in that book, only the simple formulae for
;   constant steps are explicitaly written. Nothing else in Chapter 3.
;
; - for the case with constant step, the procedure is obvious,
;   except at the edges. We do use the formulae given in Hildebrand (p82)
; - when the X step is variable, a more complex computation is
;   mandatory. We use the formulae written here:
; http://sonia_madani.club.fr/Cloaque/Arithmurgistan/Derivation/lagrange.html
;
; EXAMPLE: see also the accompagning program TEST_DERIV
;          d=DERIV(REPLICATE(1.,nbp))
;          d=DERIV(x_pos, y_data)
;
; TESTABILITY: the best way is to try on a pure slope !
;
; MODIFICATION HISTORY:
;   - 17/03/2006 created by Alain Coulais (ARSC)
;   - 09/10/2012 default is to be as tolerant as possible (/check if needed) !
;
;-
; LICENCE:
; Copyright (C) 2006, 2012 Alain Coulais
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.
; 
;-
function DERIV, x, y, help=help, test=test, no_check=no_check, check=check
;
ON_ERROR, 2
;
; When GDL behavior of {MESSAGE, .skip, .Continue} will be similar to
; IDL, the following flag can be switch to 1 !
;
flag_message=1
name_proc='% DERIV: '
;
if ((N_PARAMS() EQ 0) OR (N_PARAMS() GT 2)) then begin
   if (flag_message EQ 1) then begin
      MESSAGE, 'Incorrect number of arguments'
   endif else begin
      print, '% DERIV: Incorrect number of arguments'
      help=1
   endelse
endif
;
if KEYWORD_SET(help) then begin
   print, 'function DERIV, [x,] y, $'
   print, '         [help=help, test=test, no_check=no_check, check=check]'
   return, -1
endif
;
nbp_x=N_ELEMENTS(x)
if (nbp_x LT 3) then begin
   txt='Data must have at least 3 points !'
   if (flag_message EQ 1) then MESSAGE, txt else print, name_proc+txt
   return, -1
endif
;
if (SIZE(x,/type) EQ 7) then begin
   txt='No STRING vector allowed !'
   if (flag_message EQ 1) then MESSAGE, txt else print, name_proc+txt
   return, -1
endif
;
; We check whether X and Y arrays have same size !
if (N_PARAMS() EQ 2) then begin
    nbp_y=N_ELEMENTS(y)
    if (nbp_x NE nbp_y) then begin
        txt='X and Y vectors must have same size !'
        if (flag_message EQ 1) then MESSAGE, txt else print, name_proc+txt
        return, -1
    endif
    if (SIZE(y,/type) EQ 7) then begin
        txt='No STRING vector allowed !'
        if (flag_message EQ 1) then MESSAGE, txt else print, name_proc+txt
        return, -1
    endif
endif
;
; First case : we only have Data, no Position.
; That means that the data are equally spaced.
; We use the 3 formulae given by Hildebrand
;
if (N_PARAMS() EQ 1) then begin
   ;;
   derivee=SHIFT(x,-1)-SHIFT(x,1)
   ;;
   ;; Specific computation for the 2 Edges
   ;; (if needed, the type conversion (Int/Long --> Float) will be
   ;; done here automatically)
   ;;
   derivee[0]=-3.0*x[0]+4.0*x[1]-x[2]
   derivee[nbp_x-1]=3.0*x[nbp_x-1]-4.0*x[nbp_x-2]+x[nbp_x-3]
   ;;
   derivee=derivee/2.
endif
;
; Second case : we have Positions (X) and Data (Y)
; We use the formulae expressed on Web Site
;
if (N_PARAMS() EQ 2) then begin
   if KEYWORD_SET(check) then begin
      ;; some checks : sorting, no-nul steps, no-constant steps, ...
      ;;
      dx=x-SHIFT(x,1)
      dx=dx[1:*]
      dx_min=MIN(dx)
      dx_max=MAX(dx)
      ;;
      ;; X locations must be sorted (by increasing order)
      ;;
      if (dx_min LT 0.0) then begin
         txt='X colocations MUST be sorted !'
         if (flag_message EQ 1) then MESSAGE, txt else print, name_proc+txt
         return, -1
      endif
      ;;
      ;; X locations must be strictly different (to avoid zero division)
      ;;
      if (dx_min EQ 0.0) then begin
         txt='At least 2 X colocations are EQUAL !'
         if (flag_message EQ 1) then MESSAGE, txt else print, name_proc+txt
         return, -1
      endif
      ;;
      ;; When All steps are equal, we can used "FAST" way ...
      ;;
      if (dx_min EQ dx_max) then begin
         if KEYWORD_SET(verbose) then print, name_proc+'Found constant STEP ... Algorithm changed'
         derivee=DERIV(y)/dx_min
         if KEYWORD_SET(test) then STOP 
         return, derivee
      endif
      ;; after here, X are well conditionned !
      ;; x0 < x1 < x2
   endif
   ;;
   ;;  3 useful expressions
   ;;
   x0_x1=SHIFT(x,1)-x
   x1_x2=x-SHIFT(x,-1)
   x0_x2=SHIFT(x,1)-SHIFT(x,-1)
   ;;
   derivee=SHIFT(y,1)*x1_x2/(x0_x1*x0_x2)
   derivee=derivee+y*(1./x1_x2-1./x0_x1)
   derivee=derivee-SHIFT(y,-1)*x0_x1/(x0_x2*x1_x2)
   ;;
   ;; The 2 Edges (see matrix formulae on Web Page)
   ;;
   derivee[0]=y[0]*(1./x0_x1[1]+1./x0_x2[1])
   derivee[0]=derivee[0]-y[1]*x0_x2[1]/(x0_x1[1]*x1_x2[1])
   derivee[0]=derivee[0]+y[2]*x0_x1[1]/(x0_x2[1]*x1_x2[1])
   ;;
   ;; useful index
   nm3=nbp_x-3
   nm2=nbp_x-2
   nm1=nbp_x-1
   ;;
   derivee[nm1]=-y[nm3]*x1_x2[nm2]/(x0_x1[nm2]*x0_x2[nm2])
   derivee[nm1]=derivee[nm1]+y[nm2]*x0_x2[nm2]/(x0_x1[nm2]*x1_x2[nm2])
   derivee[nm1]=derivee[nm1]-y[nm1]*(1./x0_x2[nm2]+1./x1_x2[nm2])    
endif
;
if KEYWORD_SET(test) then STOP
;
return, derivee
;
end
