;+
; NAME:
;   MPFITFUN
;
; AUTHOR:
;   Craig B. Markwardt, NASA/GSFC Code 662, Greenbelt, MD 20770
;   craigm@lheamail.gsfc.nasa.gov
;   UPDATED VERSIONs can be found on my WEB PAGE: 
;      http://cow.physics.wisc.edu/~craigm/idl/idl.html
;
; PURPOSE:
;   Perform Levenberg-Marquardt least-squares fit to IDL function
;
; MAJOR TOPICS:
;   Curve and Surface Fitting
;
; CALLING SEQUENCE:
;   parms = MPFITFUN(MYFUNCT, X, Y, ERR, start_params, ...)
;
; DESCRIPTION:
;
;  MPFITFUN fits a user-supplied model -- in the form of an IDL
;  function -- to a set of user-supplied data.  MPFITFUN calls
;  MPFIT, the MINPACK-1 least-squares minimizer, to do the main
;  work.
;
;  Given the data and their uncertainties, MPFITFUN finds the best set
;  of model parameters which match the data (in a least-squares
;  sense) and returns them in an array.
;  
;  The user must supply the following items:
;   - An array of independent variable values ("X").
;   - An array of "measured" *dependent* variable values ("Y").
;   - An array of "measured" 1-sigma uncertainty values ("ERR").
;   - The name of an IDL function which computes Y given X ("MYFUNCT").
;   - Starting guesses for all of the parameters ("START_PARAMS").
;
;  There are very few restrictions placed on X, Y or MYFUNCT.  Simply
;  put, MYFUNCT must map the "X" values into "Y" values given the
;  model parameters.  The "X" values may represent any independent
;  variable (not just Cartesian X), and indeed may be multidimensional
;  themselves.  For example, in the application of image fitting, X
;  may be a 2xN array of image positions.
;
;  Data values of NaN or Infinity for "Y", "ERR" or "WEIGHTS" will be
;  ignored as missing data if the NAN keyword is set.  Otherwise, they
;  may cause the fitting loop to halt with an error message.  Note
;  that the fit will still halt if the model function, or its
;  derivatives, produces infinite or NaN values.
;
;  MPFITFUN carefully avoids passing large arrays where possible to
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
;    FUNCTION MYFUNCT, X, P
;     ; The independent variable is X
;     ; Parameter values are passed in "P"
;     YMOD = ... computed model values at X ...
;     return, YMOD
;    END
;
;  The returned array YMOD must have the same dimensions and type as
;  the "measured" Y values.
;
;  User functions may also indicate a fatal error condition
;  using the ERROR_CODE common block variable, as described
;  below under the MPFIT_ERROR common block definition.
;
;  MPFIT by default calculates derivatives numerically via a finite
;  difference approximation.  However, the user function *may*
;  calculate the derivatives if desired, but only if the model
;  function is declared with an additional position parameter, DP, as
;  described below.
;
;  To enable explicit derivatives for all parameters, set
;  AUTODERIVATIVE=0.
;
;  When AUTODERIVATIVE=0, the user function is responsible for
;  calculating the derivatives of the user function with respect to
;  each parameter.  The user function should be declared as follows:
;
;    ;
;    ; MYFUNCT - example user function
;    ;   P - input parameter values (N-element array)
;    ;   DP - upon input, an N-vector indicating which parameters
;    ;          to compute derivatives for; 
;    ;        upon output, the user function must return
;    ;          an ARRAY(M,N) of derivatives in this keyword
;    ;   (keywords) - any other keywords specified by FUNCTARGS
;    ; RETURNS - function values
;    ;
;    FUNCTION MYFUNCT, x, p, dp [, (additional keywords if desired)]
;     model = F(x, p)         ;; Model function
;     
;     if n_params() GT 2 then begin
;       ; Create derivative and compute derivative array
;       requested = dp   ; Save original value of DP
;       dp = make_array(n_elements(x), n_elements(p), value=x[0]*0)
;
;       ; Compute derivative if requested by caller
;       for i = 0, n_elements(p)-1 do if requested(i) NE 0 then $
;         dp(*,i) = FGRAD(x, p, i)
;     endif
;    
;     return, resid
;    END
;
;  where FGRAD(x, p, i) is a model function which computes the
;  derivative of the model F(x,p) with respect to parameter P(i) at X.
;
;  Derivatives should be returned in the DP array. DP should be an
;  ARRAY(m,n) array, where m is the number of data points and n is the
;  number of parameters.  DP[i,j] is the derivative of the ith
;  function value with respect to the jth parameter.
;
;  MPFIT may not always request derivatives from the user function.
;  In those cases, the parameter DP is not passed.  Therefore
;  functions can use N_PARAMS() to indicate whether they must compute
;  the derivatives or not.
;
;  For additional information about explicit derivatives, including
;  additional settings and debugging options, see the discussion under
;  "EXPLICIT DERIVATIVES" and AUTODERIVATIVE in MPFIT.PRO.
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
;             free or fixed parameters as an equality constraint.  Any
;             expression involving constants and the parameter array P
;             are permitted.
;             Example: if parameter 2 is always to be twice parameter
;             1 then use the following: parinfo[2].tied = '2 * P[1]'.
;             Since they are totally constrained, tied parameters are
;             considered to be fixed; no errors are computed for them.
;             [ NOTE: the PARNAME can't be used in a TIED expression. ]
;
;     .MPPRINT - if set to 1, then the default ITERPROC will print the
;                parameter value.  If set to 0, the parameter value
;                will not be printed.  This tag can be used to
;                selectively print only a few parameter values out of
;                many.  Default: 1 (all parameters printed)
;
;     .MPFORMAT - IDL format string to print the parameter within
;                 ITERPROC.  Default: '(G20.6)'  (An empty string will
;                 also use the default.)
;
;  Future modifications to the PARINFO structure, if any, will involve
;  adding structure tags beginning with the two letters "MP".
;  Therefore programmers are urged to avoid using tags starting with
;  "MP", but otherwise they are free to include their own fields
;  within the PARINFO structure, which will be ignored by MPFIT.
;  
;  PARINFO Example:
;  parinfo = replicate({value:0.D, fixed:0, limited:[0,0], $
;                       limits:[0.D,0]}, 5)
;  parinfo[0].fixed = 1
;  parinfo[4].limited[0] = 1
;  parinfo[4].limits[0]  = 50.D
;  parinfo[*].value = [5.7D, 2.2, 500., 1.5, 2000.]
;  
;  A total of 5 parameters, with starting values of 5.7,
;  2.2, 500, 1.5, and 2000 are given.  The first parameter
;  is fixed at a value of 5.7, and the last parameter is
;  constrained to be above 50.
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
;   MYFUNCT - a string variable containing the name of an IDL function.
;             This function computes the "model" Y values given the
;             X values and model parameters, as desribed above.
;
;   X - Array of independent variable values.
;
;   Y - Array of "measured" dependent variable values.  Y should have
;       the same data type as X.  The function MYFUNCT should map
;       X->Y.
;         NOTE: the following special cases apply:
;                * if Y is NaN or Infinite, and the NAN keyword is
;                  set, then the corresponding data point is ignored
;
;   ERR - Array of "measured" 1-sigma uncertainties.  ERR should have
;         the same data type as Y.  ERR is ignored if the WEIGHTS
;         keyword is specified.
;         NOTE: the following special cases apply:
;                * if ERR is zero, then the corresponding data point
;                  is ignored
;                * if ERR is NaN or Infinite, and the NAN keyword is
;                  set, then the corresponding data point is ignored
;                * if ERR is negative, then the absolute value of 
;                  ERR is used.
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
;
; RETURNS:
;
;   Returns the array of best-fit parameters.
;
;
; KEYWORD PARAMETERS:
;
;   BESTNORM - the value of the summed squared residuals for the
;              returned parameter values.
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
;   CASH - when set, the fit statistic is changed to a derivative of
;          the CASH statistic.  The model function must be strictly
;          positive. WARNING: this option is incomplete and untested.
;
;   DOF - number of degrees of freedom, computed as
;             DOF = N_ELEMENTS(DEVIATES) - NFREE
;         Note that this doesn't account for pegged parameters (see
;         NPEGGED).  It also does not account for data points which
;         are assigned zero weight, for example if :
;           * WEIGHTS[i] EQ 0, or
;           * ERR[i] EQ infinity, or 
;           * any of the values is "undefined" and /NAN is set.
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
;             number of calculation iterations exceeds MAXITER, then
;             the STATUS value is set to 5 and MPFIT returns.  
;
;             If MAXITER EQ 0, then MPFIT does not iterate to adjust
;             parameter values; however, the user function is evaluated
;             and parameter errors/covariance/Jacobian are estimated
;             before returning.
;             Default: 200 iterations
;
;   NAN - ignore infinite or NaN values in the Y, ERR or WEIGHTS
;         parameters.  These values will be treated as missing data.
;         However, the fit will still halt with an error condition
;         if the model function becomes infinite.
;
;   NFEV - the number of MYFUNCT function evaluations performed.
;
;   NFREE - the number of free parameters in the fit.  This includes
;           parameters which are not FIXED and not TIED, but it does
;           include parameters which are pegged at LIMITS.
;
;   NITER - the number of iterations completed.
;
;   NOCOVAR - set this keyword to prevent the calculation of the
;             covariance matrix before returning (see COVAR)
;
;   NPEGGED - the number of free parameters which are pegged at a
;             LIMIT.
;
;   NPRINT - The frequency with which ITERPROC is called.  A value of
;            1 indicates that ITERPROC is called with every iteration,
;            while 2 indicates every other iteration, etc.  Be aware
;            that several Levenberg-Marquardt attempts can be made in
;            a single iteration.  Also, the ITERPROC is *always*
;            called for the final iteration, regardless of the
;            iteration number.
;            Default value: 1
;
;   PARINFO - A one-dimensional array of structures.
;             Provides a mechanism for more sophisticated constraints
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
;            uncertainties.  
;
;            *If* you can assume that the true reduced chi-squared
;            value is unity -- meaning that the fit is implicitly
;            assumed to be of good quality -- then the estimated
;            parameter uncertainties can be computed by scaling PERROR
;            by the measured chi-squared value.
;
;              DOF     = N_ELEMENTS(X) - N_ELEMENTS(PARMS) ; deg of freedom
;              PCERROR = PERROR * SQRT(BESTNORM / DOF)   ; scaled uncertainties
;
;   PFREE_INDEX - upon return, PFREE_INDEX contains an index array
;                 which indicates which parameter were allowed to
;                 vary.  I.e. of all the parameters PARMS, only
;                 PARMS[PFREE_INDEX] were varied.
;
;   QUERY - if set, then MPFIT() will return immediately with one of
;           the following values:
;                 1 - if MIN_VERSION is not set
;                 1 - if MIN_VERSION is set and MPFIT satisfies the minimum
;                 0 - if MIN_VERSION is set and MPFIT does not satisfy it
;           Default: not set.
;
;   QUIET - set this keyword when no textual output should be printed
;           by MPFIT
;
;   STATUS - an integer status code is returned.  All values greater
;            than zero can represent success (however STATUS EQ 5 may
;            indicate failure to converge).  It can have one of the
;            following values:
;
;        -18  a fatal execution error has occurred.  More information
;             may be available in the ERRMSG string.
;
;        -16  a parameter or function value has become infinite or an
;             undefined number.  This is usually a consequence of
;             numerical overflow in the user's model function, which
;             must be avoided.
;
;        -15 to -1 
;             these are error codes that either MYFUNCT or ITERPROC
;             may return to terminate the fitting process (see
;             description of MPFIT_ERROR common below).  If either
;             MYFUNCT or ITERPROC set ERROR_CODE to a negative number,
;             then that number is returned in STATUS.  Values from -15
;             to -1 are reserved for the user functions and will not
;             clash with MPFIT.
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
;                CHISQ = TOTAL( (Y-MYFUNCT(X,P))^2 * ABS(WEIGHTS) )
;
;             Here are common values of WEIGHTS for standard weightings:
;
;                1D/ERR^2 - Normal weighting (ERR is the measurement error)
;                1D/Y     - Poisson weighting (counting statistics)
;                1D       - Unweighted
;
;         NOTE: the following special cases apply:
;                * if WEIGHTS is zero, then the corresponding data point
;                  is ignored
;                * if WEIGHTS is NaN or Infinite, and the NAN keyword is
;                  set, then the corresponding data point is ignored
;                * if WEIGHTS is negative, then the absolute value of 
;                  WEIGHTS is used.
;
;   XTOL - a nonnegative input variable. Termination occurs when the
;          relative error between two consecutive iterates is at most
;          XTOL (and STATUS is accordingly set to 2 or 3).  Therefore,
;          XTOL measures the relative error desired in the approximate
;          solution.  Default: 1D-10
;
;   YFIT - the best-fit model function, as returned by MYFUNCT.
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
;   p0 = [1.D, 1., 1000.]                   ; Initial guess (cent, width, area)
;   p = mpfitfun('GAUSS1', x, y, sy, p0)    ; Fit a function
;   print, p
;
;   Generates a synthetic data set with a Gaussian peak, and Poisson
;   statistical uncertainty.  Then the same function is fitted to the
;   data (with different starting parameters) to see how close we can
;   get.
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
;   Written, Apr-Jul 1998, CM
;   Added PERROR keyword, 04 Aug 1998, CM
;   Added COVAR keyword, 20 Aug 1998, CM
;   Added ITER output keyword, 05 Oct 1998
;      D.L Windt, Bell Labs, windt@bell-labs.com;
;   Added ability to return model function in YFIT, 09 Nov 1998
;   Analytical derivatives allowed via AUTODERIVATIVE keyword, 09 Nov 1998
;   Parameter values can be tied to others, 09 Nov 1998
;   Cosmetic documentation updates, 16 Apr 1999, CM
;   More cosmetic documentation updates, 14 May 1999, CM
;   Made sure to update STATUS, 25 Sep 1999, CM
;   Added WEIGHTS keyword, 25 Sep 1999, CM
;   Changed from handles to common blocks, 25 Sep 1999, CM
;     - commons seem much cleaner and more logical in this case.
;   Alphabetized documented keywords, 02 Oct 1999, CM
;   Added QUERY keyword and query checking of MPFIT, 29 Oct 1999, CM
;   Corrected EXAMPLE (offset of 1000), 30 Oct 1999, CM
;   Check to be sure that X and Y are present, 02 Nov 1999, CM
;   Documented PERROR for unweighted fits, 03 Nov 1999, CM
;   Changed to ERROR_CODE for error condition, 28 Jan 2000, CM
;   Corrected errors in EXAMPLE, 26 Mar 2000, CM
;   Copying permission terms have been liberalized, 26 Mar 2000, CM
;   Propagated improvements from MPFIT, 17 Dec 2000, CM
;   Added CASH statistic, 10 Jan 2001
;   Added NFREE and NPEGGED keywords, 11 Sep 2002, CM
;   Documented RELSTEP field of PARINFO (!!), CM, 25 Oct 2002
;   Add DOF keyword to return degrees of freedom, CM, 23 June 2003
;   Convert to IDL 5 array syntax (!), 16 Jul 2006, CM
;   Move STRICTARR compile option inside each function/procedure, 9
;     Oct 2006
;   Add NAN keyword, to ignore non-finite data values, 28 Oct 2006, CM
;   Clarify documentation on user-function, derivatives, and PARINFO,
;     27 May 2007
;   Fix bug in handling of explicit derivatives with errors/weights
;     (the weights were not being applied), CM, 03 Sep 2007
;   Add COMPATIBILITY section, CM, 13 Dec 2007
;   Add documentation about NAN behavior, CM, 30 Mar 2009
;   Add keywords BEST_RESIDS, CALC_FJAC, BEST_FJAC, PFREE_INDEX;
;     update some documentation that had become stale, CM, 2010-10-28
;   Documentation corrections, CM, 2011-08-26
;   Additional documentation about explicit derivatives, CM, 2012-07-23
;
;  $Id: mpfitfun.pro,v 1.19 2012/09/27 23:59:31 cmarkwar Exp $
;-
; Copyright (C) 1997-2002, 2003, 2006, 2007, 2009, 2010, 2011, 2012, Craig Markwardt
; This software is provided as is without any warranty whatsoever.
; Permission to use, copy, modify, and distribute modified or
; unmodified copies is granted, provided this copyright and disclaimer
; are included unchanged.
;-

FORWARD_FUNCTION mpfitfun_eval, mpfitfun, mpfit

; This is the call-back function for MPFIT.  It evaluates the
; function, subtracts the data, and returns the residuals.
function mpfitfun_eval, p, dp, _EXTRA=extra

  COMPILE_OPT strictarr
  common mpfitfun_common, fcn, x, y, err, wts, f, fcnargs

  ;; Save the original DP matrix for later use
  if n_params() GT 1 then if n_elements(dp) GT 0 then dp0 = dp

  ;; The function is evaluated here.  There are four choices,
  ;; depending on whether (a) FUNCTARGS was passed to MPFITFUN, which
  ;; is passed to this function as "hf"; or (b) the derivative
  ;; parameter "dp" is passed, meaning that derivatives should be
  ;; calculated analytically by the function itself.
  if n_elements(fcnargs) GT 0 then begin
      if n_params() GT 1 then f = call_function(fcn, x, p, dp, _EXTRA=fcnargs)$
      else                    f = call_function(fcn, x, p, _EXTRA=fcnargs)
  endif else begin
      if n_params() GT 1 then f = call_function(fcn, x, p, dp) $
      else                    f = call_function(fcn, x, p)
  endelse

  np = n_elements(p)
  nf = n_elements(f)

  ;; Compute the deviates, applying either errors or weights
  if n_elements(wts) GT 0 then begin
      result = (y-f)*wts
      if n_elements(dp0) GT 0 AND n_elements(dp) EQ np*nf then begin
          for j = 0L, np-1 do dp[j*nf] = dp[j*nf:j*nf+nf-1] * wts
      endif
  endif else if n_elements(err) GT 0 then begin
      result = (y-f)/err
      if n_elements(dp0) GT 0 AND n_elements(dp) EQ np*nf then begin
          for j = 0L, np-1 do dp[j*nf] = dp[j*nf:j*nf+nf-1] / err
      endif
  endif else begin
      result = (y-f)
  endelse
      
  ;; Make sure the returned result is one-dimensional.
  result = reform(result, n_elements(result), /overwrite)
  return, result
  
end

;; Implement residual and gradient scaling according to the
;; prescription of Cash (ApJ, 228, 939)
pro mpfitfun_cash, resid, dresid
  COMPILE_OPT strictarr
  common mpfitfun_common, fcn, x, y, err, wts, f, fcnargs

  sz = size(dresid)
  m = sz[1]
  n = sz[2]

  ;; Do rudimentary dimensions checks, so we don't do something stupid
  if n_elements(y) NE m OR n_elements(f) NE m OR n_elements(resid) NE m then begin
      DIM_ERROR:
      message, 'ERROR: dimensions of Y, F, RESID or DRESID are not consistent'
  endif

  ;; Scale gradient by sqrt(y)/f
  gfact = temporary(dresid) * rebin(reform(sqrt(y)/f,m,1),m,n)
  dresid = reform(dresid, m, n, /overwrite)
  
  ;; Scale residuals by 1/sqrt(y)
  resid = temporary(resid)/sqrt(y)

  return
end

function mpfitfun, fcn, x, y, err, p, WEIGHTS=wts, FUNCTARGS=fa, $
                   BESTNORM=bestnorm, nfev=nfev, STATUS=status, $
                   best_resid=best_resid, pfree_index=ifree, $
                   calc_fjac=calc_fjac, best_fjac=best_fjac, $
                   parinfo=parinfo, query=query, CASH=cash, $
                   covar=covar, perror=perror, yfit=yfit, $
                   niter=niter, nfree=nfree, npegged=npegged, dof=dof, $
                   quiet=quiet, ERRMSG=errmsg, NAN=NAN, _EXTRA=extra

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
      message, "USAGE: PARMS = MPFITFUN('MYFUNCT', X, Y, ERR, "+ $
        "START_PARAMS, ... )", /info
      return, !values.d_nan
  endif
  if n_elements(x) EQ 0 OR n_elements(y) EQ 0 then begin
      message, 'ERROR: X and Y must be defined', /info
      return, !values.d_nan
  endif

  if n_elements(err) GT 0 OR n_elements(wts) GT 0 AND keyword_set(cash) then begin
      message, 'ERROR: WEIGHTS or ERROR cannot be specified with CASH', /info
      return, !values.d_nan
  endif
  if keyword_set(cash) then begin
      scalfcn = 'mpfitfun_cash'
  endif

  ;; Use common block to pass data back and forth
  common mpfitfun_common, fc, xc, yc, ec, wc, mc, ac
  fc = fcn & xc = x & yc = y & mc = 0L
  ;; These optional parameters must be undefined first
  ac = 0 & dummy = size(temporary(ac))
  ec = 0 & dummy = size(temporary(ec))
  wc = 0 & dummy = size(temporary(wc))

  ;; FUNCTARGS
  if n_elements(fa) GT 0 then ac = fa

  ;; WEIGHTS or ERROR
  if n_elements(wts) GT 0 then begin
      wc = sqrt(abs(wts))
  endif else if n_elements(err) GT 0 then begin
      wh = where(err EQ 0, ct)
      if ct GT 0 then begin
          errmsg = 'ERROR: ERROR value must not be zero.  Use WEIGHTS instead.'
          message, errmsg, /info
          return, !values.d_nan
      endif
      ;; Appropriate weight for gaussian errors
      wc = 1/abs(err)
  endif

  ;; Check for weights/errors which do not match the dimension
  ;; of the data points
  if n_elements(wc) GT 0 AND $
    n_elements(wc) NE 1 AND $
    n_elements(wc) NE n_elements(yc) then begin
      errmsg = 'ERROR: ERROR/WEIGHTS must either be a scalar or match the number of Y values'
      message, errmsg, /info
      return, !values.d_nan
  endif

  ;; If the weights/errors are a scalar value, and not finite, then 
  ;; the fit will surely fail
  if n_elements(wc) EQ 1 then begin
      if finite(wc[0]) EQ 0 then begin
          errmsg = 'ERROR: the supplied scalar WEIGHT/ERROR value was not finite'
          message, errmsg, /info
          return, !values.d_nan
      endif
  endif

  ;; Handle the cases of non-finite data points or weights
  if keyword_set(nan) then begin
      ;; Non-finite data points
      wh = where(finite(yc) EQ 0, ct)
      if ct GT 0 then begin
          yc[wh] = 0
          ;; Careful: handle case when weights were a scalar...
          ;;   ... promote to a vector
          if n_elements(wc) EQ 1 then wc = replicate(wc[0], n_elements(yc))
          wc[wh] = 0
      endif

      ;; Non-finite weights
      wh = where(finite(wc) EQ 0, ct)
      if ct GT 0 then wc[wh] = 0
  endif

  result = mpfit('mpfitfun_eval', p, SCALE_FCN=scalfcn, $
                 parinfo=parinfo, STATUS=status, nfev=nfev, BESTNORM=bestnorm,$
                 covar=covar, perror=perror, $
                 best_resid=best_resid, pfree_index=ifree, $
                 calc_fjac=calc_fjac, best_fjac=best_fjac, $
                 niter=niter, nfree=nfree, npegged=npegged, dof=dof, $
                 ERRMSG=errmsg, quiet=quiet, _EXTRA=extra)

  ;; Retrieve the fit value
  yfit = temporary(mc)

  ;; Rescale the Jacobian according to parameter uncertainties
  if keyword_set(calc_fjac) AND nfree GT 0 AND status GT 0 then begin
      ec = 1/wc  ;; Per-data-point errors (could be INF or NAN!)
      for i = 0, nfree-1 do best_fjac[*,i] = - best_fjac[*,i] * ec
  endif

  ;; Some cleanup
  xc = 0 & yc = 0 & wc = 0 & ec = 0 & mc = 0 & ac = 0

  ;; Print error message if there is one.
  if NOT keyword_set(quiet) AND errmsg NE '' then $
    message, errmsg, /info

  return, result
end
