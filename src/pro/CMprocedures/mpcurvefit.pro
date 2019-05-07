;+
; NAME:
;   MPCURVEFIT
;
; AUTHOR:
;   Craig B. Markwardt, NASA/GSFC Code 662, Greenbelt, MD 20770
;   craigm@lheamail.gsfc.nasa.gov
;   UPDATED VERSIONs can be found on my WEB PAGE: 
;      http://cow.physics.wisc.edu/~craigm/idl/idl.html
;
; PURPOSE:
;   Perform Levenberg-Marquardt least-squares fit (replaces CURVEFIT)
;
; MAJOR TOPICS:
;   Curve and Surface Fitting
;
; CALLING SEQUENCE:
;   YFIT = MPCURVEFIT(X, Y, WEIGHTS, P, [SIGMA,] FUNCTION_NAME=FUNC, 
;                     ITER=iter, ITMAX=itmax, 
;                     CHISQ=chisq, NFREE=nfree, DOF=dof, 
;                     NFEV=nfev, COVAR=covar, [/NOCOVAR, ] [/NODERIVATIVE, ]
;                     FUNCTARGS=functargs, PARINFO=parinfo,
;                     FTOL=ftol, XTOL=xtol, GTOL=gtol, TOL=tol,
;                     ITERPROC=iterproc, ITERARGS=iterargs,
;                     NPRINT=nprint, QUIET=quiet, 
;                     ERRMSG=errmsg, STATUS=status)
;
; DESCRIPTION:
;
;  MPCURVEFIT fits a user-supplied model -- in the form of an IDL
;  function -- to a set of user-supplied data.  MPCURVEFIT calls
;  MPFIT, the MINPACK-1 least-squares minimizer, to do the main
;  work.
;
;  Given the data and their uncertainties, MPCURVEFIT finds the best
;  set of model parameters which match the data (in a least-squares
;  sense) and returns them in the parameter P.  
;
;  MPCURVEFIT returns the best fit function.
;  
;  The user must supply the following items:
;   - An array of independent variable values ("X").
;   - An array of "measured" *dependent* variable values ("Y").
;   - An array of weighting values ("WEIGHTS").
;   - The name of an IDL function which computes Y given X ("FUNC").
;   - Starting guesses for all of the parameters ("P").
;
;  There are very few restrictions placed on X, Y or FUNCT.  Simply
;  put, FUNCT must map the "X" values into "Y" values given the
;  model parameters.  The "X" values may represent any independent
;  variable (not just Cartesian X), and indeed may be multidimensional
;  themselves.  For example, in the application of image fitting, X
;  may be a 2xN array of image positions.
;
;  MPCURVEFIT carefully avoids passing large arrays where possible to
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
;    ; MYFUNCT - example user function
;    ;   X - input independent variable (vector same size as data)
;    ;   P - input parameter values (N-element array)
;    ;   YMOD - upon return, user function values
;    ;   DP - upon return, the user function must return
;    ;          an ARRAY(M,N) of derivatives in this parameter
;    ;
;    PRO MYFUNCT, x, p, ymod, dp
;     ymod = F(x, p)         ;; Model function
;     
;     if n_params() GE 4  then begin
;       ; Create derivative and compute derivative array
;       dp = make_array(n_elements(x), n_elements(p), value=x[0]*0)
;
;       ; Compute derivative if requested by caller
;       for i = 0, n_elements(p)-1 do dp(*,i) = FGRAD(x, p, i)
;     endif
;    END
;
;  where FGRAD(x, p, i) is a model function which computes the
;  derivative of the model F(x,p) with respect to parameter P(i) at X.
;  The returned array YMOD must have the same dimensions and type as
;  the "measured" Y values.  The returned array DP[i,j] is the
;  derivative of the ith function value with respect to the jth
;  parameter.
;
;  User functions may also indicate a fatal error condition
;  using the ERROR_CODE common block variable, as described
;  below under the MPFIT_ERROR common block definition.
; 
;  If NODERIVATIVE=1, then MPCURVEFIT will never request explicit
;  derivatives from the user function, and instead will user numerical
;  estimates (i.e. by calling the user function multiple times).
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
;     .MPMAXSTEP - the maximum change to be made in the parameter
;                  value.  During the fitting process, the parameter
;                  will never be changed by more than this value in
;                  one iteration.
;
;                  A value of 0 indicates no maximum.  Default: 0.
;  
;     .TIED - a string expression which "ties" the parameter to other
;             free or fixed parameters.  Any expression involving
;             constants and the parameter array P are permitted.
;             Example: if parameter 2 is always to be twice parameter
;             1 then use the following: parinfo(2).tied = '2 * P(1)'.
;             Since they are totally constrained, tied parameters are
;             considered to be fixed; no errors are computed for them.
;             [ NOTE: the PARNAME can't be used in expressions. ]
;
;     .MPPRINT - if set to 1, then the default ITERPROC will print the
;                parameter value.  If set to 0, the parameter value
;                will not be printed.  This tag can be used to
;                selectively print only a few parameter values out of
;                many.  Default: 1 (all parameters printed)
;
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
;  parinfo(0).fixed = 1
;  parinfo(4).limited(0) = 1
;  parinfo(4).limits(0)  = 50.D
;  parinfo(*).value = [5.7D, 2.2, 500., 1.5, 2000.]
;  
;  A total of 5 parameters, with starting values of 5.7,
;  2.2, 500, 1.5, and 2000 are given.  The first parameter
;  is fixed at a value of 5.7, and the last parameter is
;  constrained to be above 50.
;
; INPUTS:
;   X - Array of independent variable values.
;
;   Y - Array of "measured" dependent variable values.  Y should have
;       the same data type as X.  The function FUNCT should map
;       X->Y.
;
;   WEIGHTS - Array of weights to be used in calculating the
;             chi-squared value.  If WEIGHTS is specified then the ERR
;             parameter is ignored.  The chi-squared value is computed
;             as follows:
;
;                CHISQ = TOTAL( (Y-FUNCT(X,P))^2 * ABS(WEIGHTS) )
;
;             Here are common values of WEIGHTS:
;
;                1D/ERR^2 - Normal weighting (ERR is the measurement error)
;                1D/Y     - Poisson weighting (counting statistics)
;                1D       - Unweighted
;
;   P - An array of starting values for each of the parameters of the
;       model.  The number of parameters should be fewer than the
;       number of measurements.  Also, the parameters should have the
;       same data type as the measurements (double is preferred).
;
;       Upon successful completion the new parameter values are
;       returned in P.
;
;       If both START_PARAMS and PARINFO are passed, then the starting
;       *value* is taken from START_PARAMS, but the *constraints* are
;       taken from PARINFO.
; 
;   SIGMA - The formal 1-sigma errors in each parameter, computed from
;           the covariance matrix.  If a parameter is held fixed, or
;           if it touches a boundary, then the error is reported as
;           zero.
;
;           If the fit is unweighted (i.e. no errors were given, or
;           the weights were uniformly set to unity), then SIGMA will
;           probably not represent the true parameter uncertainties.
;
;           *If* you can assume that the true reduced chi-squared
;           value is unity -- meaning that the fit is implicitly
;           assumed to be of good quality -- then the estimated
;           parameter uncertainties can be computed by scaling SIGMA
;           by the measured chi-squared value.
;
;              DOF     = N_ELEMENTS(X) - N_ELEMENTS(P) ; deg of freedom
;              CSIGMA  = SIGMA * SQRT(CHISQ / DOF)     ; scaled uncertainties
;
; RETURNS:
;
;   Returns the array containing the best-fitting function.
;
; KEYWORD PARAMETERS:
;
;   CHISQ - the value of the summed, squared, weighted residuals for
;           the returned parameter values, i.e. the chi-square value.
;
;   COVAR - the covariance matrix for the set of parameters returned
;           by MPFIT.  The matrix is NxN where N is the number of
;           parameters.  The square root of the diagonal elements
;           gives the formal 1-sigma statistical errors on the
;           parameters IF errors were treated "properly" in MYFUNC.
;           Parameter errors are also returned in PERROR.
;
;           To compute the correlation matrix, PCOR, use this:
;           IDL> PCOR = COV * 0
;           IDL> FOR i = 0, n-1 DO FOR j = 0, n-1 DO $
;                PCOR(i,j) = COV(i,j)/sqrt(COV(i,i)*COV(j,j))
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
;   FUNCTION_NAME - a scalar string containing the name of an IDL
;                   procedure to compute the user model values, as
;                   described above in the "USER MODEL" section.
;
;   FUNCTARGS - A structure which contains the parameters to be passed
;               to the user-supplied function specified by FUNCT via
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
;   ITER - the number of iterations completed.
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
;              PRO ITERPROC, FUNCT, p, iter, fnorm, FUNCTARGS=fcnargs, $
;                PARINFO=parinfo, QUIET=quiet, ...
;                ; perform custom iteration update
;              END
;         
;              ITERPROC must either accept all three keyword
;              parameters (FUNCTARGS, PARINFO and QUIET), or at least
;              accept them via the _EXTRA keyword.
;          
;              FUNCT is the user-supplied function to be minimized,
;              P is the current set of model parameters, ITER is the
;              iteration number, and FUNCTARGS are the arguments to be
;              passed to FUNCT.  FNORM should be the
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
;   ITMAX - The maximum number of iterations to perform.  If the
;             number is exceeded, then the STATUS value is set to 5
;             and MPFIT returns.
;             Default: 200 iterations
;
;   NFEV - the number of FUNCT function evaluations performed.
;
;   NFREE - the number of free parameters in the fit.  This includes
;           parameters which are not FIXED and not TIED, but it does
;           include parameters which are pegged at LIMITS.
;
;   NOCOVAR - set this keyword to prevent the calculation of the
;             covariance matrix before returning (see COVAR)
;
;   NODERIVATIVE - if set, then the user function will not be queried
;                  for analytical derivatives, and instead the
;                  derivatives will be computed by finite differences
;                  (and according to the PARINFO derivative settings;
;                  see above for a description).
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
;   TOL - synonym for FTOL.  Use FTOL instead.
;
;   XTOL - a nonnegative input variable. Termination occurs when the
;          relative error between two consecutive iterates is at most
;          XTOL (and STATUS is accordingly set to 2 or 3).  Therefore,
;          XTOL measures the relative error desired in the approximate
;          solution.  Default: 1D-10
;
;   YERROR - upon return, the root-mean-square variance of the
;            residuals.
;
;
; EXAMPLE:
;
;   ; First, generate some synthetic data
;   npts = 200
;   x  = dindgen(npts) * 0.1 - 10.                  ; Independent variable 
;   yi = gauss1(x, [2.2D, 1.4, 3000.])              ; "Ideal" Y variable
;   y  = yi + randomn(seed, npts) * sqrt(1000. + yi); Measured, w/ noise
;   sy = sqrt(1000.D + y)                           ; Poisson errors
;
;   ; Now fit a Gaussian to see how well we can recover
;   p0 = [1.D, 1., 1000.]                           ; Initial guess
;   yfit = mpcurvefit(x, y, 1/sy^2, p0, $           ; Fit a function
;                     FUNCTION_NAME='GAUSS1P',/autoderivative)
;   print, p
;
;   Generates a synthetic data set with a Gaussian peak, and Poisson
;   statistical uncertainty.  Then the same function is fitted to the
;   data to see how close we can get.  GAUSS1 and GAUSS1P are
;   available from the same web page.
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
; REFERENCES:
;
;   MINPACK-1, Jorge More', available from netlib (www.netlib.org).
;   "Optimization Software Guide," Jorge More' and Stephen Wright, 
;     SIAM, *Frontiers in Applied Mathematics*, Number 14.
;
; MODIFICATION HISTORY:
;   Translated from MPFITFUN, 25 Sep 1999, CM
;   Alphabetized documented keywords, 02 Oct 1999, CM
;   Added QUERY keyword and query checking of MPFIT, 29 Oct 1999, CM
;   Check to be sure that X and Y are present, 02 Nov 1999, CM
;   Documented SIGMA for unweighted fits, 03 Nov 1999, CM
;   Changed to ERROR_CODE for error condition, 28 Jan 2000, CM
;   Copying permission terms have been liberalized, 26 Mar 2000, CM
;   Propagated improvements from MPFIT, 17 Dec 2000, CM
;   Corrected behavior of NODERIVATIVE, 13 May 2002, CM
;   Documented RELSTEP field of PARINFO (!!), CM, 25 Oct 2002
;   Make more consistent with comparable IDL routines, 30 Jun 2003, CM
;   Minor documentation adjustment, 03 Feb 2004, CM
;   Fix error in documentation, 26 Aug 2005, CM
;   Convert to IDL 5 array syntax (!), 16 Jul 2006, CM
;   Move STRICTARR compile option inside each function/procedure, 9 Oct 2006
;   Fix bug in handling of explicit derivatives with errors/weights
;     (the weights were not being applied), CM, 2012-07-22
;   Add more documentation on calling interface for user function and
;     parameter derivatives, CM, 2012-07-22
;   Better documentation for STATUS, CM, 2016-04-29
;
;  $Id: mpcurvefit.pro,v 1.12 2016/05/19 16:08:49 cmarkwar Exp $
;-
; Copyright (C) 1997-2000, 2002, 2003, 2004, 2005, 2012, 2016 Craig Markwardt
; This software is provided as is without any warranty whatsoever.
; Permission to use, copy, modify, and distribute modified or
; unmodified copies is granted, provided this copyright and disclaimer
; are included unchanged.
;-

FORWARD_FUNCTION mpcurvefit_eval, mpcurvefit, mpfit

; This is the call-back function for MPFIT.  It evaluates the
; function, subtracts the data, and returns the residuals.
function mpcurvefit_eval, p, dp, _EXTRA=extra

  COMPILE_OPT strictarr
  common mpcurvefit_common, fcn, x, y, wts, f, fcnargs

  ;; The function is evaluated here.  There are four choices,
  ;; depending on whether (a) FUNCTARGS was passed to MPCURVEFIT, which
  ;; is passed to this function as "hf"; or (b) the derivative
  ;; parameter "dp" is passed, meaning that derivatives should be
  ;; calculated analytically by the function itself.
  if n_elements(fcnargs) GT 0 then begin
      if n_params() GT 1 then call_procedure, fcn, x, p, f, dp,_EXTRA=fcnargs $
      else                    call_procedure, fcn, x, p, f,    _EXTRA=fcnargs
  endif else begin
      if n_params() GT 1 then call_procedure, fcn, x, p, f, dp $
      else                    call_procedure, fcn, x, p, f
  endelse

  ;; Compute the deviates, applying the weights
  result = (y-f)*wts
  
  ;; Apply weights to derivative quantities                                     
  if n_params() GT 1 then begin
      np = n_elements(p)
      nf = n_elements(f)
      for j = 0L, np-1 do dp[j*nf] = dp[j*nf:j*nf+nf-1] * wts
  endif
      
  ;; Make sure the returned result is one-dimensional.
  result = reform(result, n_elements(result), /overwrite)
  return, result
  
end

function mpcurvefit, x, y, wts, p, perror, function_name=fcn, $
                     iter=iter, itmax=maxiter, $
                     chisq=bestnorm, nfree=nfree, dof=dof, $
                     nfev=nfev, covar=covar, nocovar=nocovar, yerror=yerror, $
                     noderivative=noderivative, tol=tol, ftol=ftol, $
                     FUNCTARGS=fa, parinfo=parinfo, $
                     errmsg=errmsg, STATUS=status, QUIET=quiet, $
                     query=query, _EXTRA=extra

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
      message, "USAGE: YFIT = MPCURVEFIT(X, Y, WTS, P, DP)", /info
      return, !values.d_nan
  endif
  if n_elements(x) EQ 0 OR n_elements(y) EQ 0 then begin
      message, 'ERROR: X and Y must be defined', /info
      return, !values.d_nan
  endif
  if n_elements(fcn) EQ 0 then fcn = 'funct'
  if n_elements(noderivative) EQ 0 then noderivative = 0

  common mpcurvefit_common, fc, xc, yc, wc, mc, ac
  fc = fcn & xc = x & yc = y & wc = sqrt(abs(wts)) & mc = 0L
  ac = 0 & dummy = size(temporary(ac))
  if n_elements(fa) GT 0 then ac = fa

  if n_elements(tol) GT 0 then ftol = tol

  result = mpfit('mpcurvefit_eval', p, maxiter=maxiter, $
                 autoderivative=noderivative, ftol=ftol, $
                 parinfo=parinfo, STATUS=status, nfev=nfev, BESTNORM=bestnorm,$
                 covar=covar, perror=perror, niter=iter, nfree=nfree, dof=dof,$
                 ERRMSG=errmsg, quiet=quiet, _EXTRA=extra)

  ;; Retrieve the fit value
  yfit = temporary(mc)
  ;; Now do some clean-up
  xc = 0 & yc = 0 & wc = 0 & mc = 0 & ac = 0

  if NOT keyword_set(quiet) AND errmsg NE '' then $
    message, errmsg, /info $
  else $
    p = result

  yerror = p[0]*0
  if n_elements(dof) GT 0 AND dof[0] GT 0 then begin
      yerror[0] = sqrt( total( (y-yfit)^2 ) / dof[0] )
  endif

  return, yfit
end
