;+
; NAME:
;   MPFIT2DFUN
;
; AUTHOR:
;   Craig B. Markwardt, NASA/GSFC Code 662, Greenbelt, MD 20770
;   craigm@lheamail.gsfc.nasa.gov
;   UPDATED VERSIONs can be found on my WEB PAGE: 
;      http://cow.physics.wisc.edu/~craigm/idl/idl.html
;
; PURPOSE:
;   Perform Levenberg-Marquardt least-squares fit to a 2-D IDL function
;
; MAJOR TOPICS:
;   Curve and Surface Fitting
;
; CALLING SEQUENCE:
;   parms = MPFIT2DFUN(MYFUNCT, X, Y, Z, ERR, start_parms, ...)
;
; DESCRIPTION:
;
;  MPFIT2DFUN fits a user-supplied model -- in the form of an IDL
;  function -- to a set of user-supplied data.  MPFIT2DFUN calls
;  MPFIT, the MINPACK-1 least-squares minimizer, to do the main
;  work.  MPFIT2DFUN is a specialized version for two-dimensional 
;  data.
;
;  Given the data and their uncertainties, MPFIT2DFUN finds the best set
;  of model parameters which match the data (in a least-squares
;  sense) and returns them in an array.
;  
;  The user must supply the following items:
;   - Two arrays of independent variable values ("X", "Y").
;   - An array of "measured" *dependent* variable values ("Z").
;   - An array of "measured" 1-sigma uncertainty values ("ERR").
;   - The name of an IDL function which computes Z given (X,Y) ("MYFUNCT").
;   - Starting guesses for all of the parameters ("START_PARAMS").
;
;  There are very few restrictions placed on X, Y, Z, or MYFUNCT.
;  Simply put, MYFUNCT must map the (X,Y) values into Z values given
;  the model parameters.  The (X,Y) values are usually the independent
;  X and Y coordinate positions in the two dimensional plane, but need
;  not be.
;
;  MPFIT2DFUN carefully avoids passing large arrays where possible to
;  improve performance.
;
;  See below for an example of usage.
;   
; USER FUNCTION
;
;  The user must define a function which returns the model value.  For
;  applications which use finite-difference derivatives -- the default
;  -- the user function should be declared in the following way:
;
;    FUNCTION MYFUNCT, X, Y, P
;     ; The independent variables are X and Y
;     ; Parameter values are passed in "P"
;     ZMOD = ... computed model values at (X,Y) ...
;     return, ZMOD
;    END
;
;  The returned array YMOD must have the same dimensions and type as
;  the "measured" Z values.
;
;  User functions may also indicate a fatal error condition
;  using the ERROR_CODE common block variable, as described
;  below under the MPFIT_ERROR common block definition.
;
;  See the discussion under "ANALYTIC DERIVATIVES" and AUTODERIVATIVE
;  in MPFIT.PRO if you wish to compute the derivatives for yourself.
;  AUTODERIVATIVE is accepted and passed directly to MPFIT.  The user
;  function must accept one additional parameter, DP, which contains
;  the derivative of the user function with respect to each parameter
;  at each data point, as described in MPFIT.PRO.
;
; CREATING APPROPRIATELY DIMENSIONED INDEPENDENT VARIABLES
;
;  The user must supply appropriate independent variables to
;  MPFIT2DFUN.  For image fitting applications, this variable should
;  be two-dimensional *arrays* describing the X and Y positions of
;  every *pixel*.  [ Thus any two dimensional sampling is permitted,
;  including irregular sampling. ]
;  
;  If the sampling is regular, then the x coordinates are the same for
;  each row, and the y coordinates are the same for each column.  Call
;  the x-row and y-column coordinates XR and YC respectively.  You can
;  then compute X and Y as follows:
;  
;      X = XR # (YC*0 + 1)             eqn. 1
;      Y = (XR*0 + 1) # YC             eqn. 2
;  
;  For example, if XR and YC have the following values:
;  
;    XR = [  1, 2, 3, 4, 5,]  ;; X positions of one row of pixels
;    YC = [ 15,16,17 ]        ;; Y positions of one column of
;                                pixels
;  
;  Then using equations 1 and 2 above will give these values to X and
;  Y:
;  
;     X :  1  2  3  4  5       ;; X positions of all pixels
;          1  2  3  4  5
;          1  2  3  4  5
;  
;     Y : 15 15 15 15 15       ;; Y positions of all pixels
;         16 16 16 16 16
;         17 17 17 17 17
;  
;  Using the above technique is suggested, but *not* required.  You
;  can do anything you wish with the X and Y values.  This technique
;  only makes it easier to compute your model function values.
;
; CONSTRAINING PARAMETER VALUES WITH THE PARINFO KEYWORD
;
;  The behavior of MPFIT can be modified with respect to each
;  parameter to be fitted.  A parameter value can be fixed; simple
;  boundary constraints can be imposed; limitations on the parameter
;  changes can be imposed; properties of the automatic derivative can
;  be modified; and parameters can be tied to one another.
;
;  These properties are governed by the PARINFO structure, which is
;  passed as a keyword parameter to MPFIT.
;
;  PARINFO should be an array of structures, one for each parameter.
;  Each parameter is associated with one element of the array, in
;  numerical order.  The structure can have the following entries
;  (none are required):
;  
;     .VALUE - the starting parameter value (but see the START_PARAMS
;              parameter for more information).
;  
;     .FIXED - a boolean value, whether the parameter is to be held
;              fixed or not.  Fixed parameters are not varied by
;              MPFIT, but are passed on to MYFUNCT for evaluation.
;  
;     .LIMITED - a two-element boolean array.  If the first/second
;                element is set, then the parameter is bounded on the
;                lower/upper side.  A parameter can be bounded on both
;                sides.  Both LIMITED and LIMITS must be given
;                together.
;  
;     .LIMITS - a two-element float or double array.  Gives the
;               parameter limits on the lower and upper sides,
;               respectively.  Zero, one or two of these values can be
;               set, depending on the values of LIMITED.  Both LIMITED
;               and LIMITS must be given together.
;  
;     .PARNAME - a string, giving the name of the parameter.  The
;                fitting code of MPFIT does not use this tag in any
;                way.  However, the default ITERPROC will print the
;                parameter name if available.
;  
;     .STEP - the step size to be used in calculating the numerical
;             derivatives.  If set to zero, then the step size is
;             computed automatically.  Ignored when AUTODERIVATIVE=0.
;             This value is superceded by the RELSTEP value.
;
;     .RELSTEP - the *relative* step size to be used in calculating
;                the numerical derivatives.  This number is the
;                fractional size of the step, compared to the
;                parameter value.  This value supercedes the STEP
;                setting.  If the parameter is zero, then a default
;                step size is chosen.
;
;     .MPSIDE - the sidedness of the finite difference when computing
;               numerical derivatives.  This field can take four
;               values:
;
;                  0 - one-sided derivative computed automatically
;                  1 - one-sided derivative (f(x+h) - f(x)  )/h
;                 -1 - one-sided derivative (f(x)   - f(x-h))/h
;                  2 - two-sided derivative (f(x+h) - f(x-h))/(2*h)
;
;              Where H is the STEP parameter described above.  The
;              "automatic" one-sided derivative method will chose a
;              direction for the finite difference which does not
;              violate any constraints.  The other methods do not
;              perform this check.  The two-sided method is in
;              principle more precise, but requires twice as many
;              function evaluations.  Default: 0.
;
;     .MPMINSTEP - the minimum change to be made in the parameter
;                  value.  During the fitting process, the parameter
;                  will be changed by multiples of this value.  The
;                  actual step is computed as:
;
;                     DELTA1 = MPMINSTEP*ROUND(DELTA0/MPMINSTEP)
;
;                  where DELTA0 and DELTA1 are the estimated parameter
;                  changes before and after this constraint is
;                  applied.  Note that this constraint should be used
;                  with care since it may cause non-converging,
;                  oscillating solutions.
;
;                  A value of 0 indicates no minimum.  Default: 0.
;
;     .MPMAXSTEP - the maximum change to be made in the parameter
;                  value.  During the fitting process, the parameter
;                  will never be changed by more than this value.
;
;                  A value of 0 indicates no maximum.  Default: 0.
;  
;     .TIED - a string expression which "ties" the parameter to other
;             free or fixed parameters.  Any expression involving
;             constants and the parameter array P are permitted.
;             Example: if parameter 2 is always to be twice parameter
;             1 then use the following: parinfo[2].tied = '2 * P[1]'.
;             Since they are totally constrained, tied parameters are
;             considered to be fixed; no errors are computed for them.
;             [ NOTE: the PARNAME can't be used in expressions. ]
;  
;  Future modifications to the PARINFO structure, if any, will involve
;  adding structure tags beginning with the two letters "MP".
;  Therefore programmers are urged to avoid using tags starting with
;  the same letters; otherwise they are free to include their own
;  fields within the PARINFO structure, and they will be ignored.
;  
;  PARINFO Example:
;  parinfo = replicate({value:0.D, fixed:0, limited:[0,0], $
;                       limits:[0.D,0]}, 5)
;  parinfo[0].fixed = 1
;  parinfo[4].limited(0) = 1
;  parinfo[4].limits(0)  = 50.D
;  parinfo[*].value = [5.7D, 2.2, 500., 1.5, 2000.]
;  
;  A total of 5 parameters, with starting values of 5.7,
;  2.2, 500, 1.5, and 2000 are given.  The first parameter
;  is fixed at a value of 5.7, and the last parameter is
;  constrained to be above 50.
;
;
; COMPATIBILITY
;
;  This function is designed to work with IDL 5.0 or greater.
;  
;  Because TIED parameters rely on the EXECUTE() function, they cannot
;  be used with the free version of the IDL Virtual Machine.
;
;
; INPUTS:
;   MYFUNCT - a string variable containing the name of an IDL
;             function.  This function computes the "model" Z values
;             given the X,Y values and model parameters, as described above.
;
;   X - Array of "X" independent variable values, as described above.
;       These values are passed directly to the fitting function
;       unmodified.
;
;   Y - Array of "Y" independent variable values, as described
;       above. X and Y should have the same data type.
;
;   Z - Array of "measured" dependent variable values.  Z should have
;       the same data type as X and Y.  The function MYFUNCT should
;       map (X,Y)->Z.
;
;   ERR - Array of "measured" 1-sigma uncertainties.  ERR should have
;         the same data type as Z.  ERR is ignored if the WEIGHTS
;         keyword is specified.
;
;   START_PARAMS - An array of starting values for each of the
;                  parameters of the model.  The number of parameters
;                  should be fewer than the number of measurements.
;                  Also, the parameters should have the same data type
;                  as the measurements (double is preferred).
;
;                  This parameter is optional if the PARINFO keyword
;                  is used (see MPFIT).  The PARINFO keyword provides
;                  a mechanism to fix or constrain individual
;                  parameters.  If both START_PARAMS and PARINFO are
;                  passed, then the starting *value* is taken from
;                  START_PARAMS, but the *constraints* are taken from
;                  PARINFO.
; 
; RETURNS:
;
;   Returns the array of best-fit parameters.
;
; KEYWORD PARAMETERS:
;
;   BESTNORM - the value of the summed, squared, weighted residuals
;              for the returned parameter values, i.e. the chi-square value.
;
;   BEST_FJAC - upon return, BEST_FJAC contains the Jacobian, or
;               partial derivative, matrix for the best-fit model.
;               The values are an array,
;               ARRAY(N_ELEMENTS(DEVIATES),NFREE) where NFREE is the
;               number of free parameters.  This array is only
;               computed if /CALC_FJAC is set, otherwise BEST_FJAC is
;               undefined.
;
;               The returned array is such that BEST_FJAC[I,J] is the
;               partial derivative of the model with respect to
;               parameter PARMS[PFREE_INDEX[J]].
;
;   BEST_RESID - upon return, an array of best-fit deviates,
;                normalized by the weights or errors.
;
;   COVAR - the covariance matrix for the set of parameters returned
;           by MPFIT.  The matrix is NxN where N is the number of
;           parameters.  The square root of the diagonal elements
;           gives the formal 1-sigma statistical errors on the
;           parameters IF errors were treated "properly" in MYFUNC.
;           Parameter errors are also returned in PERROR.
;
;           To compute the correlation matrix, PCOR, use this example:
;                  PCOR = COV * 0
;                  FOR i = 0, n-1 DO FOR j = 0, n-1 DO $
;                    PCOR[i,j] = COV[i,j]/sqrt(COV[i,i]*COV[j,j])
;           or equivalently, in vector notation,
;                  PCOR = COV / (PERROR # PERROR)
;
;           If NOCOVAR is set or MPFIT terminated abnormally, then
;           COVAR is set to a scalar with value !VALUES.D_NAN.
;
;   DOF - number of degrees of freedom, computed as
;             DOF = N_ELEMENTS(DEVIATES) - NFREE
;         Note that this doesn't account for pegged parameters (see
;         NPEGGED).
;
;   ERRMSG - a string error or warning message is returned.
;
;   FTOL - a nonnegative input variable. Termination occurs when both
;          the actual and predicted relative reductions in the sum of
;          squares are at most FTOL (and STATUS is accordingly set to
;          1 or 3).  Therefore, FTOL measures the relative error
;          desired in the sum of squares.  Default: 1D-10
;
;   FUNCTARGS - A structure which contains the parameters to be passed
;               to the user-supplied function specified by MYFUNCT via
;               the _EXTRA mechanism.  This is the way you can pass
;               additional data to your user-supplied function without
;               using common blocks.
;
;               By default, no extra parameters are passed to the
;               user-supplied function.
;
;   GTOL - a nonnegative input variable. Termination occurs when the
;          cosine of the angle between fvec and any column of the
;          jacobian is at most GTOL in absolute value (and STATUS is
;          accordingly set to 4). Therefore, GTOL measures the
;          orthogonality desired between the function vector and the
;          columns of the jacobian.  Default: 1D-10
;
;   ITERARGS - The keyword arguments to be passed to ITERPROC via the
;              _EXTRA mechanism.  This should be a structure, and is
;              similar in operation to FUNCTARGS.
;              Default: no arguments are passed.
;
;   ITERPROC - The name of a procedure to be called upon each NPRINT
;              iteration of the MPFIT routine.  It should be declared
;              in the following way:
;
;              PRO ITERPROC, MYFUNCT, p, iter, fnorm, FUNCTARGS=fcnargs, $
;                PARINFO=parinfo, QUIET=quiet, ...
;                ; perform custom iteration update
;              END
;         
;              ITERPROC must either accept all three keyword
;              parameters (FUNCTARGS, PARINFO and QUIET), or at least
;              accept them via the _EXTRA keyword.
;          
;              MYFUNCT is the user-supplied function to be minimized,
;              P is the current set of model parameters, ITER is the
;              iteration number, and FUNCTARGS are the arguments to be
;              passed to MYFUNCT.  FNORM should be the
;              chi-squared value.  QUIET is set when no textual output
;              should be printed.  See below for documentation of
;              PARINFO.
;
;              In implementation, ITERPROC can perform updates to the
;              terminal or graphical user interface, to provide
;              feedback while the fit proceeds.  If the fit is to be
;              stopped for any reason, then ITERPROC should set the
;              common block variable ERROR_CODE to negative value (see
;              MPFIT_ERROR common block below).  In principle,
;              ITERPROC should probably not modify the parameter
;              values, because it may interfere with the algorithm's
;              stability.  In practice it is allowed.
;
;              Default: an internal routine is used to print the
;                       parameter values.
;
;   MAXITER - The maximum number of iterations to perform.  If the
;             number is exceeded, then the STATUS value is set to 5
;             and MPFIT returns.
;             Default: 200 iterations
;
;   NFEV - the number of MYFUNCT function evaluations performed.
;
;   NITER - the number of iterations completed.
;
;   NOCOVAR - set this keyword to prevent the calculation of the
;             covariance matrix before returning (see COVAR)
;
;   NPRINT - The frequency with which ITERPROC is called.  A value of
;            1 indicates that ITERPROC is called with every iteration,
;            while 2 indicates every other iteration, etc.  Note that
;            several Levenberg-Marquardt attempts can be made in a
;            single iteration.
;            Default value: 1
;
;   PARINFO - Provides a mechanism for more sophisticated constraints
;             to be placed on parameter values.  When PARINFO is not
;             passed, then it is assumed that all parameters are free
;             and unconstrained.  Values in PARINFO are never 
;             modified during a call to MPFIT.
;
;             See description above for the structure of PARINFO.
;
;             Default value:  all parameters are free and unconstrained.
;
;   PERROR - The formal 1-sigma errors in each parameter, computed
;            from the covariance matrix.  If a parameter is held
;            fixed, or if it touches a boundary, then the error is
;            reported as zero.
;
;            If the fit is unweighted (i.e. no errors were given, or
;            the weights were uniformly set to unity), then PERROR
;            will probably not represent the true parameter
;            uncertainties.  *If* you can assume that the true reduced
;            chi-squared value is unity -- meaning that the fit is
;            implicitly assumed to be of good quality -- then the
;            estimated parameter uncertainties can be computed by
;            scaling PERROR by the measured chi-squared value.
;
;              DOF     = N_ELEMENTS(Z) - N_ELEMENTS(PARMS) ; deg of freedom
;              PCERROR = PERROR * SQRT(BESTNORM / DOF)   ; scaled uncertainties
;
;   PFREE_INDEX - upon return, PFREE_INDEX contains an index array
;                 which indicates which parameter were allowed to
;                 vary.  I.e. of all the parameters PARMS, only
;                 PARMS[PFREE_INDEX] were varied.
;
;   QUIET - set this keyword when no textual output should be printed
;           by MPFIT
;
;   STATUS - an integer status code is returned.  All values greater
;            than zero can represent success (however STATUS EQ 5 may
;            indicate failure to converge).  It can have one of the
;            following values:
;
;	   0  improper input parameters.
;         
;	   1  both actual and predicted relative reductions
;	      in the sum of squares are at most FTOL.
;         
;	   2  relative error between two consecutive iterates
;	      is at most XTOL
;         
;	   3  conditions for STATUS = 1 and STATUS = 2 both hold.
;         
;	   4  the cosine of the angle between fvec and any
;	      column of the jacobian is at most GTOL in
;	      absolute value.
;         
;	   5  the maximum number of iterations has been reached
;         
;	   6  FTOL is too small. no further reduction in
;	      the sum of squares is possible.
;         
;	   7  XTOL is too small. no further improvement in
;	      the approximate solution x is possible.
;         
;	   8  GTOL is too small. fvec is orthogonal to the
;	      columns of the jacobian to machine precision.
;
;   WEIGHTS - Array of weights to be used in calculating the
;             chi-squared value.  If WEIGHTS is specified then the ERR
;             parameter is ignored.  The chi-squared value is computed
;             as follows:
;
;                CHISQ = TOTAL( (Z-MYFUNCT(X,Y,P))^2 * ABS(WEIGHTS) )
;
;             Here are common values of WEIGHTS:
;
;                1D/ERR^2 - Normal weighting (ERR is the measurement error)
;                1D/Z     - Poisson weighting (counting statistics)
;                1D       - Unweighted
;
;   XTOL - a nonnegative input variable. Termination occurs when the
;          relative error between two consecutive iterates is at most
;          XTOL (and STATUS is accordingly set to 2 or 3).  Therefore,
;          XTOL measures the relative error desired in the approximate
;          solution.  Default: 1D-10
;
;   YFIT - the best-fit model function, as returned by MYFUNCT.
;
; EXAMPLE:
;
;   p  = [2.2D, -0.7D, 1.4D, 3000.D]
;   x  = (dindgen(200)*0.1 - 10.) # (dblarr(200) + 1)
;   y  = (dblarr(200) + 1) # (dindgen(200)*0.1 - 10.)
;   zi = gauss2(x, y, p)
;   sz = sqrt(zi>1)
;   z  = zi + randomn(seed, 200, 200) * sz
;
;   p0 = [0D, 0D, 1D, 10D]
;   p = mpfit2dfun('GAUSS2', x, y, z, sz, p0)
;   
;   Generates a synthetic data set with a Gaussian peak, and Poisson
;   statistical uncertainty.  Then the same function (but different
;   starting parameters) is fitted to the data to see how close we can
;   get.
;
;   It is especially worthy to notice that the X and Y values are
;   created as full images, so that a coordinate is attached to each
;   pixel independently.  This is the format that GAUSS2 accepts, and
;   the easiest for you to use in your own functions.
;
;
; COMMON BLOCKS:
;
;   COMMON MPFIT_ERROR, ERROR_CODE
;
;     User routines may stop the fitting process at any time by
;     setting an error condition.  This condition may be set in either
;     the user's model computation routine (MYFUNCT), or in the
;     iteration procedure (ITERPROC).
;
;     To stop the fitting, the above common block must be declared,
;     and ERROR_CODE must be set to a negative number.  After the user
;     procedure or function returns, MPFIT checks the value of this
;     common block variable and exits immediately if the error
;     condition has been set.  By default the value of ERROR_CODE is
;     zero, indicating a successful function/procedure call.
;
;
; REFERENCES:
;
;   MINPACK-1, Jorge More', available from netlib (www.netlib.org).
;   "Optimization Software Guide," Jorge More' and Stephen Wright, 
;     SIAM, *Frontiers in Applied Mathematics*, Number 14.
;
; MODIFICATION HISTORY:
;   Written, transformed from MPFITFUN, 26 Sep 1999, CM
;   Alphabetized documented keywords, 02 Oct 1999, CM
;   Added example, 02 Oct 1999, CM
;   Tried to clarify definitions of X and Y, 29 Oct 1999, CM
;   Added QUERY keyword and query checking of MPFIT, 29 Oct 1999, CM
;   Check to be sure that X, Y and Z are present, 02 Nov 1999, CM
;   Documented PERROR for unweighted fits, 03 Nov 1999, CM
;   Changed to ERROR_CODE for error condition, 28 Jan 2000, CM
;   Copying permission terms have been liberalized, 26 Mar 2000, CM
;   Propagated improvements from MPFIT, 17 Dec 2000, CM
;   Documented RELSTEP field of PARINFO (!!), CM, 25 Oct 2002
;   Add DOF keyword to return degrees of freedom, CM, 23 June 2003
;   Minor documentation adjustment, 03 Feb 2004, CM
;   Fix the example to prevent zero errorbars, 28 Mar 2005, CM
;   Defend against users supplying strangely dimensioned X and Y, 29
;     Jun 2005, CM
;   Convert to IDL 5 array syntax (!), 16 Jul 2006, CM
;   Move STRICTARR compile option inside each function/procedure, 9 Oct 2006
;   Add COMPATIBILITY section, CM, 13 Dec 2007
;   Add keywords BEST_RESIDS, CALC_FJAC, BEST_FJAC, PFREE_INDEX;
;     update some documentation that had become stale, CM, 2010-10-28
;   Better documentation for STATUS, CM, 2016-04-29
;
;  $Id: mpfit2dfun.pro,v 1.13 2016/05/19 16:08:49 cmarkwar Exp $
;-
; Copyright (C) 1997-2000, 2002, 2003, 2004, 2005, 2013, 2016 Craig Markwardt
; This software is provided as is without any warranty whatsoever.
; Permission to use, copy, modify, and distribute modified or
; unmodified copies is granted, provided this copyright and disclaimer
; are included unchanged.
;-

FORWARD_FUNCTION mpfit2dfun_eval, mpfit2dfun, mpfit

; This is the call-back function for MPFIT.  It evaluates the
; function, subtracts the data, and returns the residuals.
function mpfit2dfun_eval, p, dp, _EXTRA=extra

  COMPILE_OPT strictarr
  common mpfit2dfun_common, fcn, x, y, z, err, wts, f, fcnargs

  ;; The function is evaluated here.  There are four choices,
  ;; depending on whether (a) FUNCTARGS was passed to MPFIT2DFUN, which
  ;; is passed to this function as "hf"; or (b) the derivative
  ;; parameter "dp" is passed, meaning that derivatives should be
  ;; calculated analytically by the function itself.
  if n_elements(fcnargs) GT 0 then begin
      if n_params() GT 1 then f = call_function(fcn,x,y,p, dp, _EXTRA=fcnargs)$
      else                    f = call_function(fcn,x,y,p,     _EXTRA=fcnargs)
  endif else begin
      if n_params() GT 1 then f = call_function(fcn,x,y,p, dp) $
      else                    f = call_function(fcn,x,y,p)
  endelse

  ;; Compute the deviates, applying either errors or weights
  if n_elements(err) GT 0 then begin
      result = (z-f)/err
  endif else if n_elements(wts) GT 0 then begin
      result = (z-f)*wts
  endif else begin
      result = (z-f)
  endelse
      
  ;; Make sure the returned result is one-dimensional.
  result = reform(result, n_elements(result), /overwrite)
  return, result
  
end

function mpfit2dfun, fcn, x, y, z, err, p, WEIGHTS=wts, FUNCTARGS=fa, $
                   BESTNORM=bestnorm, nfev=nfev, STATUS=status, $
                   best_resid=best_resid, pfree_index=ifree, $
                   calc_fjac=calc_fjac, best_fjac=best_fjac, $
                   parinfo=parinfo, query=query, $
                   npegged=npegged, nfree=nfree, dof=dof, $
                   covar=covar, perror=perror, niter=iter, yfit=yfit, $
                   quiet=quiet, ERRMSG=errmsg, _EXTRA=extra

  COMPILE_OPT strictarr
  status = 0L
  errmsg = ''

  ;; Detect MPFIT and crash if it was not found
  catch, catcherror
  if catcherror NE 0 then begin
      MPFIT_NOTFOUND:
      catch, /cancel
      message, 'ERROR: the required function MPFIT must be in your IDL path', /info
      return, !values.d_nan
  endif
  if mpfit(/query) NE 1 then goto, MPFIT_NOTFOUND
  catch, /cancel
  if keyword_set(query) then return, 1

  if n_params() EQ 0 then begin
      message, "USAGE: PARMS = MPFIT2DFUN('MYFUNCT', X, Y, ERR, "+ $
        "START_PARAMS, ... )", /info
      return, !values.d_nan
  endif
  if n_elements(x) EQ 0 OR n_elements(y) EQ 0 OR n_elements(z) EQ 0 then begin
      message, 'ERROR: X, Y and Z must be defined', /info
      return, !values.d_nan
  endif

  ;; Use common block to pass data back and forth
  common mpfit2dfun_common, fc, xc, yc, zc, ec, wc, mc, ac
  fc = fcn & xc = x & yc = y & zc = z & mc = 0L
  ;; These optional parameters must be undefined first
  ac = 0 & dummy = size(temporary(ac))
  ec = 0 & dummy = size(temporary(ec))
  wc = 0 & dummy = size(temporary(wc))

  if n_elements(fa) GT 0 then ac = fa
  if n_elements(wts) GT 0 then begin
      wc = sqrt(abs(wts))
  endif else if n_elements(err) GT 0 then begin
      wh = where(err EQ 0, ct)
      if ct GT 0 then begin
          message, 'ERROR: ERROR value must not be zero.  Use WEIGHTS.', $
            /info
          return, !values.d_nan
      endif
      ec = err
  endif

  result = mpfit('mpfit2dfun_eval', p, $
                 parinfo=parinfo, STATUS=status, nfev=nfev, BESTNORM=bestnorm,$
                 covar=covar, perror=perror, niter=iter, $
                 best_resid=best_resid, pfree_index=ifree, $
                 calc_fjac=calc_fjac, best_fjac=best_fjac, $
                 nfree=nfree, npegged=npegged, dof=dof, $
                 ERRMSG=errmsg, quiet=quiet, _EXTRA=extra)

  ;; Retrieve the fit value
  yfit = temporary(mc)
  ;; Some cleanup
  xc = 0 & yc = 0 & zc = 0 & wc = 0 & ec = 0 & mc = 0 & ac = 0

  ;; Print error message if there is one.
  if NOT keyword_set(quiet) AND errmsg NE '' then $
    message, errmsg, /info

  return, result
end
