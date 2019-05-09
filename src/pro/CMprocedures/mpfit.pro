;+
; NAME:
;   MPFIT
;
; AUTHOR:
;   Craig B. Markwardt, NASA/GSFC Code 662, Greenbelt, MD 20770
;   craigm@lheamail.gsfc.nasa.gov
;   UPDATED VERSIONs can be found on my WEB PAGE: 
;      http://cow.physics.wisc.edu/~craigm/idl/idl.html
;
; PURPOSE:
;   Perform Levenberg-Marquardt least-squares minimization (MINPACK-1)
;
; MAJOR TOPICS:
;   Curve and Surface Fitting
;
; CALLING SEQUENCE:
;   parms = MPFIT(MYFUNCT, start_parms, FUNCTARGS=fcnargs, NFEV=nfev,
;                 MAXITER=maxiter, ERRMSG=errmsg, NPRINT=nprint, QUIET=quiet, 
;                 FTOL=ftol, XTOL=xtol, GTOL=gtol, NITER=niter, 
;                 STATUS=status, ITERPROC=iterproc, ITERARGS=iterargs,
;                 COVAR=covar, PERROR=perror, BESTNORM=bestnorm,
;                 PARINFO=parinfo)
;
; DESCRIPTION:
;
;  MPFIT uses the Levenberg-Marquardt technique to solve the
;  least-squares problem.  In its typical use, MPFIT will be used to
;  fit a user-supplied function (the "model") to user-supplied data
;  points (the "data") by adjusting a set of parameters.  MPFIT is
;  based upon MINPACK-1 (LMDIF.F) by More' and collaborators.
;
;  For example, a researcher may think that a set of observed data
;  points is best modelled with a Gaussian curve.  A Gaussian curve is
;  parameterized by its mean, standard deviation and normalization.
;  MPFIT will, within certain constraints, find the set of parameters
;  which best fits the data.  The fit is "best" in the least-squares
;  sense; that is, the sum of the weighted squared differences between
;  the model and data is minimized.
;
;  The Levenberg-Marquardt technique is a particular strategy for
;  iteratively searching for the best fit.  This particular
;  implementation is drawn from MINPACK-1 (see NETLIB), and seems to
;  be more robust than routines provided with IDL.  This version
;  allows upper and lower bounding constraints to be placed on each
;  parameter, or the parameter can be held fixed.
;
;  The IDL user-supplied function should return an array of weighted
;  deviations between model and data.  In a typical scientific problem
;  the residuals should be weighted so that each deviate has a
;  gaussian sigma of 1.0.  If X represents values of the independent
;  variable, Y represents a measurement for each value of X, and ERR
;  represents the error in the measurements, then the deviates could
;  be calculated as follows:
;
;    DEVIATES = (Y - F(X)) / ERR
;
;  where F is the function representing the model.  You are
;  recommended to use the convenience functions MPFITFUN and
;  MPFITEXPR, which are driver functions that calculate the deviates
;  for you.  If ERR are the 1-sigma uncertainties in Y, then
;
;    TOTAL( DEVIATES^2 ) 
;
;  will be the total chi-squared value.  MPFIT will minimize the
;  chi-square value.  The values of X, Y and ERR are passed through
;  MPFIT to the user-supplied function via the FUNCTARGS keyword.
;
;  Simple constraints can be placed on parameter values by using the
;  PARINFO keyword to MPFIT.  See below for a description of this
;  keyword.
;
;  MPFIT does not perform more general optimization tasks.  See TNMIN
;  instead.  MPFIT is customized, based on MINPACK-1, to the
;  least-squares minimization problem.
;
; USER FUNCTION
;
;  The user must define a function which returns the appropriate
;  values as specified above.  The function should return the weighted
;  deviations between the model and the data.  For applications which
;  use finite-difference derivatives -- the default -- the user
;  function should be declared in the following way:
;
;    FUNCTION MYFUNCT, p, X=x, Y=y, ERR=err
;     ; Parameter values are passed in "p"
;     model = F(x, p)
;     return, (y-model)/err
;    END
;
;  See below for applications with explicit derivatives.
;
;  The keyword parameters X, Y, and ERR in the example above are
;  suggestive but not required.  Any parameters can be passed to
;  MYFUNCT by using the FUNCTARGS keyword to MPFIT.  Use MPFITFUN and
;  MPFITEXPR if you need ideas on how to do that.  The function *must*
;  accept a parameter list, P.
;  
;  In general there are no restrictions on the number of dimensions in
;  X, Y or ERR.  However the deviates *must* be returned in a
;  one-dimensional array, and must have the same type (float or
;  double) as the input arrays.
;
;  See below for error reporting mechanisms.
;
;
; CHECKING STATUS AND HANNDLING ERRORS
;
;  Upon return, MPFIT will report the status of the fitting operation
;  in the STATUS and ERRMSG keywords.  The STATUS keyword will contain
;  a numerical code which indicates the success or failure status.
;  Generally speaking, any value 1 or greater indicates success, while
;  a value of 0 or less indicates a possible failure.  The ERRMSG
;  keyword will contain a text string which should describe the error
;  condition more fully.
;
;  By default, MPFIT will trap fatal errors and report them to the
;  caller gracefully.  However, during the debugging process, it is
;  often useful to halt execution where the error occurred.  When you
;  set the NOCATCH keyword, MPFIT will not do any special error
;  trapping, and execution will stop whereever the error occurred.
;
;  MPFIT does not explicitly change the !ERROR_STATE variable
;  (although it may be changed implicitly if MPFIT calls MESSAGE).  It
;  is the caller's responsibility to call MESSAGE, /RESET to ensure
;  that the error state is initialized before calling MPFIT.
;
;  User functions may also indicate non-fatal error conditions using
;  the ERROR_CODE common block variable, as described below under the
;  MPFIT_ERROR common block definition (by setting ERROR_CODE to a
;  number between -15 and -1).  When the user function sets an error
;  condition via ERROR_CODE, MPFIT will gracefully exit immediately
;  and report this condition to the caller.  The ERROR_CODE is
;  returned in the STATUS keyword in that case.
;
;
; EXPLICIT DERIVATIVES
; 
;  In the search for the best-fit solution, MPFIT by default
;  calculates derivatives numerically via a finite difference
;  approximation.  The user-supplied function need not calculate the
;  derivatives explicitly.  However, the user function *may* calculate
;  the derivatives if desired, but only if the model function is
;  declared with an additional position parameter, DP, as described
;  below.  If the user function does not accept this additional
;  parameter, MPFIT will report an error.  As a practical matter, it
;  is often sufficient and even faster to allow MPFIT to calculate the
;  derivatives numerically, but this option is available for users who
;  wish more control over the fitting process.
;
;  There are two ways to enable explicit derivatives.  First, the user
;  can set the keyword AUTODERIVATIVE=0, which is a global switch for
;  all parameters.  In this case, MPFIT will request explicit
;  derivatives for every free parameter.  
;
;  Second, the user may request explicit derivatives for specifically
;  selected parameters using the PARINFO.MPSIDE=3 (see "CONSTRAINING
;  PARAMETER VALUES WITH THE PARINFO KEYWORD" below).  In this
;  strategy, the user picks and chooses which parameter derivatives
;  are computed explicitly versus numerically.  When PARINFO[i].MPSIDE
;  EQ 3, then the ith parameter derivative is computed explicitly.
;
;  The keyword setting AUTODERIVATIVE=0 always globally overrides the
;  individual values of PARINFO.MPSIDE.  Setting AUTODERIVATIVE=0 is
;  equivalent to resetting PARINFO.MPSIDE=3 for all parameters.
;
;  Even if the user requests explicit derivatives for some or all
;  parameters, MPFIT will not always request explicit derivatives on
;  every user function call.
;
; EXPLICIT DERIVATIVES - CALLING INTERFACE
;
;  When AUTODERIVATIVE=0, the user function is responsible for
;  calculating the derivatives of the *residuals* with respect to each
;  parameter.  The user function should be declared as follows:
;
;    ;
;    ; MYFUNCT - example user function
;    ;   P - input parameter values (N-element array)
;    ;   DP - upon input, an N-vector indicating which parameters
;    ;          to compute derivatives for; 
;    ;        upon output, the user function must return
;    ;          an ARRAY(M,N) of derivatives in this keyword
;    ;   (keywords) - any other keywords specified by FUNCTARGS
;    ; RETURNS - residual values
;    ;
;    FUNCTION MYFUNCT, p, dp, X=x, Y=y, ERR=err
;     model = F(x, p)         ;; Model function
;     resid = (y - model)/err ;; Residual calculation (for example)
;     
;     if n_params() GT 1 then begin
;       ; Create derivative and compute derivative array
;       requested = dp   ; Save original value of DP
;       dp = make_array(n_elements(x), n_elements(p), value=x[0]*0)
;
;       ; Compute derivative if requested by caller
;       for i = 0, n_elements(p)-1 do if requested(i) NE 0 then $
;         dp(*,i) = FGRAD(x, p, i) / err
;     endif
;    
;     return, resid
;    END
;
;  where FGRAD(x, p, i) is a model function which computes the
;  derivative of the model F(x,p) with respect to parameter P(i) at X.
;
;  A quirk in the implementation leaves a stray negative sign in the
;  definition of DP.  The derivative of the *residual* should be
;  "-FGRAD(x,p,i) / err" because of how the residual is defined
;  ("resid = (data - model) / err").  **HOWEVER** because of the
;  implementation quirk, MPFIT expects FGRAD(x,p,i)/err instead,
;  i.e. the opposite sign of the gradient of RESID.
;
;  Derivatives should be returned in the DP array. DP should be an
;  ARRAY(m,n) array, where m is the number of data points and n is the
;  number of parameters.  -DP[i,j] is the derivative of the ith
;  residual with respect to the jth parameter (note the minus sign
;  due to the quirk described above).
;
;  As noted above, MPFIT may not always request derivatives from the
;  user function.  In those cases, the parameter DP is not passed.
;  Therefore functions can use N_PARAMS() to indicate whether they
;  must compute the derivatives or not.
;  
;  The derivatives with respect to fixed parameters are ignored; zero
;  is an appropriate value to insert for those derivatives.  Upon
;  input to the user function, DP is set to a vector with the same
;  length as P, with a value of 1 for a parameter which is free, and a
;  value of zero for a parameter which is fixed (and hence no
;  derivative needs to be calculated).  This input vector may be
;  overwritten as needed.  In the example above, the original DP
;  vector is saved to a variable called REQUESTED, and used as a mask
;  to calculate only those derivatives that are required.
;
;  If the data is higher than one dimensional, then the *last*
;  dimension should be the parameter dimension.  Example: fitting a
;  50x50 image, "dp" should be 50x50xNPAR.
;
; EXPLICIT DERIVATIVES - TESTING and DEBUGGING
;
;  For reasonably complicated user functions, the calculation of
;  explicit derivatives of the correct sign and magnitude can be
;  difficult to get right.  A simple sign error can cause MPFIT to be
;  confused.  MPFIT has a derivative debugging mode which will compute
;  the derivatives *both* numerically and explicitly, and compare the
;  results.
;
;  It is expected that during production usage, derivative debugging
;  should be disabled for all parameters.
;
;  In order to enable derivative debugging mode, set the following
;  PARINFO members for the ith parameter.
;      PARINFO[i].MPSIDE = 3          ; Enable explicit derivatives
;      PARINFO[i].MPDERIV_DEBUG = 1   ; Enable derivative debugging mode
;      PARINFO[i].MPDERIV_RELTOL = ?? ; Relative tolerance for comparison
;      PARINFO[i].MPDERIV_ABSTOL = ?? ; Absolute tolerance for comparison
;  Note that these settings are maintained on a parameter-by-parameter
;  basis using PARINFO, so the user can choose which parameters
;  derivatives will be tested.
;
;  When .MPDERIV_DEBUG is set, then MPFIT first computes the
;  derivative explicitly by requesting them from the user function.
;  Then, it computes the derivatives numerically via finite
;  differencing, and compares the two values.  If the difference
;  exceeds a tolerance threshold, then the values are printed out to 
;  alert the user.  The tolerance level threshold contains both a
;  relative and an absolute component, and is expressed as,
;
;     ABS(DERIV_U - DERIV_N) GE (ABSTOL + RELTOL*ABS(DERIV_U))
;
;  where DERIV_U and DERIV_N are the derivatives computed explicitly
;  and numerically, respectively.  Appropriate values
;  for most users will be: 
;
;      PARINFO[i].MPDERIV_RELTOL = 1d-3 ;; Suggested relative tolerance 
;      PARINFO[i].MPDERIV_ABSTOL = 1d-7 ;; Suggested absolute tolerance
;
;  although these thresholds may have to be adjusted for a particular
;  problem.  When the threshold is exceeded, users can expect to see a
;  tabular report like this one:
;
;    FJAC DEBUG BEGIN
;    #        IPNT       FUNC    DERIV_U    DERIV_N   DIFF_ABS   DIFF_REL
;    FJAC PARM 2
;               80    -0.7308    0.04233    0.04233 -5.543E-07 -1.309E-05
;               99      1.370    0.01417    0.01417 -5.518E-07 -3.895E-05
;              118    0.07187   -0.01400   -0.01400 -5.566E-07  3.977E-05
;              137      1.844   -0.04216   -0.04216 -5.589E-07  1.326E-05
;    FJAC DEBUG END
;
;  The report will be bracketed by FJAC DEBUG BEGIN/END statements.
;  Each parameter will be delimited by the statement FJAC PARM n,
;  where n is the parameter number.  The columns are,
;
;      IPNT - data point number  (0 ... M-1)
;      FUNC - function value at that point
;      DERIV_U - explicit derivative value at that point
;      DERIV_N - numerical derivative estimate at that point
;      DIFF_ABS - absolute difference = (DERIV_U - DERIV_N)
;      DIFF_REL - relative difference = (DIFF_ABS)/(DERIV_U)
;
;  When prints appear in this report, it is most important to check
;  that the derivatives computed in two different ways have the same
;  numerical sign and the same order of magnitude, since these are the
;  most common programming mistakes.
;
;  A line of this form may also appear 
;
;   # FJAC_MASK = 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 
;
;  This line indicates for which parameters explicit derivatives are
;  expected.  A list of all-1s indicates all explicit derivatives for
;  all parameters are requested from the user function.
;    
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
;     .MPSIDE - selector for type of derivative calculation. This
;               field can take one of five possible values:
;
;                  0 - one-sided derivative computed automatically
;                  1 - one-sided derivative (f(x+h) - f(x)  )/h
;                 -1 - one-sided derivative (f(x)   - f(x-h))/h
;                  2 - two-sided derivative (f(x+h) - f(x-h))/(2*h)
;                  3 - explicit derivative used for this parameter
;
;              In the first four cases, the derivative is approximated
;              numerically by finite difference, with step size
;              H=STEP, where the STEP parameter is defined above.  The
;              last case, MPSIDE=3, indicates to allow the user
;              function to compute the derivative explicitly (see
;              section on "EXPLICIT DERIVATIVES").  AUTODERIVATIVE=0
;              overrides this setting for all parameters, and is
;              equivalent to MPSIDE=3 for all parameters.  For
;              MPSIDE=0, the "automatic" one-sided derivative method
;              will chose a direction for the finite difference which
;              does not violate any constraints.  The other methods
;              (MPSIDE=-1 or MPSIDE=1) do not perform this check.  The
;              two-sided method is in principle more precise, but
;              requires twice as many function evaluations.  Default:
;              0.
;
;     .MPDERIV_DEBUG - set this value to 1 to enable debugging of
;              user-supplied explicit derivatives (see "TESTING and
;              DEBUGGING" section above).  In addition, the
;              user must enable calculation of explicit derivatives by
;              either setting AUTODERIVATIVE=0, or MPSIDE=3 for the
;              desired parameters.  When this option is enabled, a
;              report may be printed to the console, depending on the
;              MPDERIV_ABSTOL and MPDERIV_RELTOL settings.
;              Default: 0 (no debugging)
;
;     
;     .MPDERIV_ABSTOL, .MPDERIV_RELTOL - tolerance settings for
;              print-out of debugging information, for each parameter
;              where debugging is enabled.  See "TESTING and
;              DEBUGGING" section above for the meanings of these two
;              fields.
;
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
;             considered to be fixed; no errors are computed for them,
;             and any LIMITS are not obeyed.
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
;
; RECURSION
;
;  Generally, recursion is not allowed.  As of version 1.77, MPFIT has
;  recursion protection which does not allow a model function to
;  itself call MPFIT.  Users who wish to perform multi-level
;  optimization should investigate the 'EXTERNAL' function evaluation
;  methods described below for hard-to-evaluate functions.  That
;  method places more control in the user's hands.  The user can
;  design a "recursive" application by taking care.
;
;  In most cases the recursion protection should be well-behaved.
;  However, if the user is doing debugging, it is possible for the
;  protection system to get "stuck."  In order to reset it, run the
;  procedure:
;     MPFIT_RESET_RECURSION
;  and the protection system should get "unstuck."  It is save to call
;  this procedure at any time.
;
;
; COMPATIBILITY
;
;  This function is designed to work with IDL 5.0 or greater.
;  
;  Because TIED parameters and the "(EXTERNAL)" user-model feature use
;  the EXECUTE() function, they cannot be used with the free version
;  of the IDL Virtual Machine.
;
;
; DETERMINING THE VERSION OF MPFIT
;
;  MPFIT is a changing library.  Users of MPFIT may also depend on a
;  specific version of the library being present.  As of version 1.70
;  of MPFIT, a VERSION keyword has been added which allows the user to
;  query which version is present.  The keyword works like this:
;
;    RESULT = MPFIT(/query, VERSION=version)
;
;  This call uses the /QUERY keyword to query the version number
;  without performing any computations.  Users of MPFIT can call this
;  method to determine which version is in the IDL path before
;  actually using MPFIT to do any numerical work.  Upon return, the
;  VERSION keyword contains the version number of MPFIT, expressed as
;  a string of the form 'X.Y' where X and Y are integers.
;
;  Users can perform their own version checking, or use the built-in
;  error checking of MPFIT.  The MIN_VERSION keyword enforces the
;  requested minimum version number.  For example,
;
;    RESULT = MPFIT(/query, VERSION=version, MIN_VERSION='1.70')
;
;  will check whether the accessed version is 1.70 or greater, without
;  performing any numerical processing.
;
;  The VERSION and MIN_VERSION keywords were added in MPFIT
;  version 1.70 and later.  If the caller attempts to use the VERSION
;  or MIN_VERSION keywords, and an *older* version of the code is
;  present in the caller's path, then IDL will throw an 'unknown
;  keyword' error.  Therefore, in order to be robust, the caller, must
;  use exception handling.  Here is an example demanding at least
;  version 1.70.
;
;    MPFIT_OK = 0  & VERSION = '<unknown>'
;    CATCH, CATCHERR
;    IF CATCHERR EQ 0 THEN MPFIT_OK = MPFIT(/query, VERSION=version, $
;                                         MIN_VERSION='1.70')
;    CATCH, /CANCEL
;
;    IF NOT MPFIT_OK THEN $
;      MESSAGE, 'ERROR: you must have MPFIT version 1.70 or higher in '+$
;             'your path (found version '+version+')'
;
;  Of course, the caller can also do its own version number
;  requirements checking.
;
;
; HARD-TO-COMPUTE FUNCTIONS: "EXTERNAL" EVALUATION
;
;  The normal mode of operation for MPFIT is for the user to pass a
;  function name, and MPFIT will call the user function multiple times
;  as it iterates toward a solution.
;
;  Some user functions are particularly hard to compute using the
;  standard model of MPFIT.  Usually these are functions that depend
;  on a large amount of external data, and so it is not feasible, or
;  at least highly impractical, to have MPFIT call it.  In those cases
;  it may be possible to use the "(EXTERNAL)" evaluation option.
;
;  In this case the user is responsible for making all function *and
;  derivative* evaluations.  The function and Jacobian data are passed
;  in through the EXTERNAL_FVEC and EXTERNAL_FJAC keywords,
;  respectively.  The user indicates the selection of this option by
;  specifying a function name (MYFUNCT) of "(EXTERNAL)".  No
;  user-function calls are made when EXTERNAL evaluation is being
;  used.
;
;  ** SPECIAL NOTE ** For the "(EXTERNAL)" case, the quirk noted above
;     does not apply.  The gradient matrix, EXTERNAL_FJAC, should be
;     comparable to "-FGRAD(x,p)/err", which is the *opposite* sign of
;     the DP matrix described above.  In other words, EXTERNAL_FJAC
;     has the same sign as the derivative of EXTERNAL_FVEC, and the
;     opposite sign of FGRAD.
;
;  At the end of each iteration, control returns to the user, who must
;  reevaluate the function at its new parameter values.  Users should
;  check the return value of the STATUS keyword, where a value of 9
;  indicates the user should supply more data for the next iteration,
;  and re-call MPFIT.  The user may refrain from calling MPFIT
;  further; as usual, STATUS will indicate when the solution has
;  converged and no more iterations are required.
;
;  Because MPFIT must maintain its own data structures between calls,
;  the user must also pass a named variable to the EXTERNAL_STATE
;  keyword.  This variable must be maintained by the user, but not
;  changed, throughout the fitting process.  When no more iterations
;  are desired, the named variable may be discarded.
;
;
; INPUTS:
;   MYFUNCT - a string variable containing the name of the function to
;             be minimized.  The function should return the weighted
;             deviations between the model and the data, as described
;             above.
;
;             For EXTERNAL evaluation of functions, this parameter
;             should be set to a value of "(EXTERNAL)".
;
;   START_PARAMS - An one-dimensional array of starting values for each of the
;                  parameters of the model.  The number of parameters
;                  should be fewer than the number of measurements.
;                  Also, the parameters should have the same data type
;                  as the measurements (double is preferred).
;
;                  This parameter is optional if the PARINFO keyword
;                  is used (but see PARINFO).  The PARINFO keyword
;                  provides a mechanism to fix or constrain individual
;                  parameters.  If both START_PARAMS and PARINFO are
;                  passed, then the starting *value* is taken from
;                  START_PARAMS, but the *constraints* are taken from
;                  PARINFO.
; 
; RETURNS:
;
;   Returns the array of best-fit parameters.
;   Exceptions: 
;      * if /QUERY is set (see QUERY).
;
;
; KEYWORD PARAMETERS:
;
;   AUTODERIVATIVE - If this is set, derivatives of the function will
;                    be computed automatically via a finite
;                    differencing procedure.  If not set, then MYFUNCT
;                    must provide the explicit derivatives.
;                    Default: set (=1) 
;                    NOTE: to supply your own explicit derivatives,
;                      explicitly pass AUTODERIVATIVE=0
;
;   BESTNORM - upon return, the value of the summed squared weighted
;              residuals for the returned parameter values,
;              i.e. TOTAL(DEVIATES^2).
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
;               partial derivative of DEVIATES[I] with respect to
;               parameter PARMS[PFREE_INDEX[J]].  Note that since
;               deviates are (data-model)*weight, the Jacobian of the
;               *deviates* will have the opposite sign from the
;               Jacobian of the *model*, and may be scaled by a
;               factor.
;
;   BEST_RESID - upon return, an array of best-fit deviates.
;
;   CALC_FJAC - if set, then calculate the Jacobian and return it in
;               BEST_FJAC.  If not set, then the return value of
;               BEST_FJAC is undefined.
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
;         NPEGGED).  It also does not account for data points which
;         are assigned zero weight by the user function.
;
;   ERRMSG - a string error or warning message is returned.
;
;   EXTERNAL_FVEC - upon input, the function values, evaluated at
;                   START_PARAMS.  This should be an M-vector, where M
;                   is the number of data points.
;
;   EXTERNAL_FJAC - upon input, the Jacobian array of partial
;                   derivative values.  This should be a M x N array,
;                   where M is the number of data points and N is the
;                   number of parameters.  NOTE: that all FIXED or
;                   TIED parameters must *not* be included in this
;                   array.
;
;   EXTERNAL_STATE - a named variable to store MPFIT-related state
;                    information between iterations (used in input and
;                    output to MPFIT).  The user must not manipulate
;                    or discard this data until the final iteration is
;                    performed.
;
;   FASTNORM - set this keyword to select a faster algorithm to
;              compute sum-of-square values internally.  For systems
;              with large numbers of data points, the standard
;              algorithm can become prohibitively slow because it
;              cannot be vectorized well.  By setting this keyword,
;              MPFIT will run faster, but it will be more prone to
;              floating point overflows and underflows.  Thus, setting
;              this keyword may sacrifice some stability in the
;              fitting process.
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
;               Consider the following example:
;                if FUNCTARGS = { XVAL:[1.D,2,3], YVAL:[1.D,4,9],
;                                 ERRVAL:[1.D,1,1] }
;                then the user supplied function should be declared
;                like this:
;                FUNCTION MYFUNCT, P, XVAL=x, YVAL=y, ERRVAL=err
;
;               By default, no extra parameters are passed to the
;               user-supplied function, but your function should
;               accept *at least* one keyword parameter.  [ This is to
;               accomodate a limitation in IDL's _EXTRA
;               parameter-passing mechanism. ]
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
;   ITERPRINT - The name of an IDL procedure, equivalent to PRINT,
;               that ITERPROC will use to render output.  ITERPRINT
;               should be able to accept at least four positional
;               arguments.  In addition, it should be able to accept
;               the standard FORMAT keyword for output formatting; and
;               the UNIT keyword, to redirect output to a logical file
;               unit (default should be UNIT=1, standard output).
;               These keywords are passed using the ITERARGS keyword
;               above.  The ITERPRINT procedure must accept the _EXTRA
;               keyword.  
;               NOTE: that much formatting can be handled with the 
;                     MPPRINT and MPFORMAT tags.
;               Default: 'MPFIT_DEFPRINT' (default internal formatter)
;
;   ITERPROC - The name of a procedure to be called upon each NPRINT
;              iteration of the MPFIT routine.  ITERPROC is always
;              called in the final iteration.  It should be declared
;              in the following way:
;
;              PRO ITERPROC, MYFUNCT, p, iter, fnorm, FUNCTARGS=fcnargs, $
;                PARINFO=parinfo, QUIET=quiet, DOF=dof, PFORMAT=pformat, $
;                UNIT=unit, ...
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
;              passed to MYFUNCT.  FNORM should be the chi-squared
;              value.  QUIET is set when no textual output should be
;              printed.  DOF is the number of degrees of freedom,
;              normally the number of points less the number of free
;              parameters.  See below for documentation of PARINFO.
;              PFORMAT is the default parameter value format.  UNIT is
;              passed on to the ITERPRINT procedure, and should
;              indicate the file unit where log output will be sent
;              (default: standard output).
;
;              In implementation, ITERPROC can perform updates to the
;              terminal or graphical user interface, to provide
;              feedback while the fit proceeds.  If the fit is to be
;              stopped for any reason, then ITERPROC should set the
;              common block variable ERROR_CODE to negative value
;              between -15 and -1 (see MPFIT_ERROR common block
;              below).  In principle, ITERPROC should probably not
;              modify the parameter values, because it may interfere
;              with the algorithm's stability.  In practice it is
;              allowed.
;
;              Default: an internal routine is used to print the
;                       parameter values.
;
;   ITERSTOP - Set this keyword if you wish to be able to stop the
;              fitting by hitting the predefined ITERKEYSTOP key on
;              the keyboard.  This only works if you use the default
;              ITERPROC.
;
;   ITERKEYSTOP - A keyboard key which will halt the fit (and if
;                 ITERSTOP is set and the default ITERPROC is used).
;                 ITERSTOPKEY may either be a one-character string
;                 with the desired key, or a scalar integer giving the
;                 ASCII code of the desired key.  
;                 Default: 7b (control-g)
;
;                 NOTE: the default value of ASCI 7 (control-G) cannot
;                 be read in some windowing environments, so you must
;                 change to a printable character like 'q'.
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
;   MIN_VERSION - The minimum requested version number.  This must be
;                 a scalar string of the form returned by the VERSION
;                 keyword.  If the current version of MPFIT does not
;                 satisfy the minimum requested version number, then,
;                    MPFIT(/query, min_version='...') returns 0
;                    MPFIT(...) returns NAN
;                 Default: no version number check
;                 NOTE: MIN_VERSION was added in MPFIT version 1.70
;
;   NFEV - the number of MYFUNCT function evaluations performed.
;
;   NFREE - the number of free parameters in the fit.  This includes
;           parameters which are not FIXED and not TIED, but it does
;           include parameters which are pegged at LIMITS.
;
;   NITER - the number of iterations completed.
;
;   NOCATCH - if set, then MPFIT will not perform any error trapping.
;             By default (not set), MPFIT will trap errors and report
;             them to the caller.  This keyword will typically be used
;             for debugging.
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
;           The VERSION output keyword is always set upon return.
;           Default: not set.
;
;   QUIET - set this keyword when no textual output should be printed
;           by MPFIT
;
;   RESDAMP - a scalar number, indicating the cut-off value of
;             residuals where "damping" will occur.  Residuals with
;             magnitudes greater than this number will be replaced by
;             their logarithm.  This partially mitigates the so-called
;             large residual problem inherent in least-squares solvers
;             (as for the test problem CURVI, http://www.maxthis.com/-
;             curviex.htm).  A value of 0 indicates no damping.
;             Default: 0
;
;             Note: RESDAMP doesn't work with AUTODERIV=0
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
;          9  A successful single iteration has been completed, and
;             the user must supply another "EXTERNAL" evaluation of
;             the function and its derivatives.  This status indicator
;             is neither an error nor a convergence indicator.
;
;   VERSION - upon return, VERSION will be set to the MPFIT internal
;             version number.  The version number will be a string of
;             the form "X.Y" where X is a major revision number and Y
;             is a minor revision number.
;             NOTE: the VERSION keyword was not present before 
;               MPFIT version number 1.70, therefore, callers must 
;               use exception handling when using this keyword.
;
;   XTOL - a nonnegative input variable. Termination occurs when the
;          relative error between two consecutive iterates is at most
;          XTOL (and STATUS is accordingly set to 2 or 3).  Therefore,
;          XTOL measures the relative error desired in the approximate
;          solution.  Default: 1D-10
;
;
; EXAMPLE:
;
;   p0 = [5.7D, 2.2, 500., 1.5, 2000.]
;   fa = {X:x, Y:y, ERR:err}
;   p = mpfit('MYFUNCT', p0, functargs=fa)
;
;   Minimizes sum of squares of MYFUNCT.  MYFUNCT is called with the X,
;   Y, and ERR keyword parameters that are given by FUNCTARGS.  The
;   resulting parameter values are returned in p.
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
;     condition has been set.  This value is also returned in the
;     STATUS keyword: values of -1 through -15 are reserved error
;     codes for the user routines.  By default the value of ERROR_CODE
;     is zero, indicating a successful function/procedure call.
;
;   COMMON MPFIT_PROFILE
;   COMMON MPFIT_MACHAR
;   COMMON MPFIT_CONFIG
;
;     These are undocumented common blocks are used internally by
;     MPFIT and may change in future implementations.
;
; THEORY OF OPERATION:
;
;   There are many specific strategies for function minimization.  One
;   very popular technique is to use function gradient information to
;   realize the local structure of the function.  Near a local minimum
;   the function value can be taylor expanded about x0 as follows:
;
;      f(x) = f(x0) + f'(x0) . (x-x0) + (1/2) (x-x0) . f''(x0) . (x-x0)
;             -----   ---------------   -------------------------------  (1)
;     Order    0th          1st                      2nd
;
;   Here f'(x) is the gradient vector of f at x, and f''(x) is the
;   Hessian matrix of second derivatives of f at x.  The vector x is
;   the set of function parameters, not the measured data vector.  One
;   can find the minimum of f, f(xm) using Newton's method, and
;   arrives at the following linear equation:
;
;      f''(x0) . (xm-x0) = - f'(x0)                            (2)
;
;   If an inverse can be found for f''(x0) then one can solve for
;   (xm-x0), the step vector from the current position x0 to the new
;   projected minimum.  Here the problem has been linearized (ie, the
;   gradient information is known to first order).  f''(x0) is
;   symmetric n x n matrix, and should be positive definite.
;
;   The Levenberg - Marquardt technique is a variation on this theme.
;   It adds an additional diagonal term to the equation which may aid the
;   convergence properties:
;
;      (f''(x0) + nu I) . (xm-x0) = -f'(x0)                  (2a)
;
;   where I is the identity matrix.  When nu is large, the overall
;   matrix is diagonally dominant, and the iterations follow steepest
;   descent.  When nu is small, the iterations are quadratically
;   convergent.
;
;   In principle, if f''(x0) and f'(x0) are known then xm-x0 can be
;   determined.  However the Hessian matrix is often difficult or
;   impossible to compute.  The gradient f'(x0) may be easier to
;   compute, if even by finite difference techniques.  So-called
;   quasi-Newton techniques attempt to successively estimate f''(x0)
;   by building up gradient information as the iterations proceed.
;
;   In the least squares problem there are further simplifications
;   which assist in solving eqn (2).  The function to be minimized is
;   a sum of squares:
;
;       f = Sum(hi^2)                                         (3)
;
;   where hi is the ith residual out of m residuals as described
;   above.  This can be substituted back into eqn (2) after computing
;   the derivatives:
;
;       f'  = 2 Sum(hi  hi')     
;       f'' = 2 Sum(hi' hj') + 2 Sum(hi hi'')                (4)
;
;   If one assumes that the parameters are already close enough to a
;   minimum, then one typically finds that the second term in f'' is
;   negligible [or, in any case, is too difficult to compute].  Thus,
;   equation (2) can be solved, at least approximately, using only
;   gradient information.
;
;   In matrix notation, the combination of eqns (2) and (4) becomes:
;
;        hT' . h' . dx = - hT' . h                          (5)
;
;   Where h is the residual vector (length m), hT is its transpose, h'
;   is the Jacobian matrix (dimensions n x m), and dx is (xm-x0).  The
;   user function supplies the residual vector h, and in some cases h'
;   when it is not found by finite differences (see MPFIT_FDJAC2,
;   which finds h and hT').  Even if dx is not the best absolute step
;   to take, it does provide a good estimate of the best *direction*,
;   so often a line minimization will occur along the dx vector
;   direction.
;
;   The method of solution employed by MINPACK is to form the Q . R
;   factorization of h', where Q is an orthogonal matrix such that QT .
;   Q = I, and R is upper right triangular.  Using h' = Q . R and the
;   ortogonality of Q, eqn (5) becomes
;
;        (RT . QT) . (Q . R) . dx = - (RT . QT) . h
;                     RT . R . dx = - RT . QT . h         (6)
;                          R . dx = - QT . h
;
;   where the last statement follows because R is upper triangular.
;   Here, R, QT and h are known so this is a matter of solving for dx.
;   The routine MPFIT_QRFAC provides the QR factorization of h, with
;   pivoting, and MPFIT_QRSOL;V provides the solution for dx.
;   
; REFERENCES:
;
;   Markwardt, C. B. 2008, "Non-Linear Least Squares Fitting in IDL
;     with MPFIT," in proc. Astronomical Data Analysis Software and
;     Systems XVIII, Quebec, Canada, ASP Conference Series, Vol. XXX, eds.
;     D. Bohlender, P. Dowler & D. Durand (Astronomical Society of the
;     Pacific: San Francisco), p. 251-254 (ISBN: 978-1-58381-702-5)
;       http://arxiv.org/abs/0902.2850
;       Link to NASA ADS: http://adsabs.harvard.edu/abs/2009ASPC..411..251M
;       Link to ASP: http://aspbooks.org/a/volumes/table_of_contents/411
;
;   Refer to the MPFIT website as:
;       http://purl.com/net/mpfit
;
;   MINPACK-1 software, by Jorge More' et al, available from netlib.
;     http://www.netlib.org/
;
;   "Optimization Software Guide," Jorge More' and Stephen Wright, 
;     SIAM, *Frontiers in Applied Mathematics*, Number 14.
;     (ISBN: 978-0-898713-22-0)
;
;   More', J. 1978, "The Levenberg-Marquardt Algorithm: Implementation
;     and Theory," in Numerical Analysis, vol. 630, ed. G. A. Watson
;     (Springer-Verlag: Berlin), p. 105 (DOI: 10.1007/BFb0067690 )
;
; MODIFICATION HISTORY:
;   Translated from MINPACK-1 in FORTRAN, Apr-Jul 1998, CM
;   Fixed bug in parameter limits (x vs xnew), 04 Aug 1998, CM
;   Added PERROR keyword, 04 Aug 1998, CM
;   Added COVAR keyword, 20 Aug 1998, CM
;   Added NITER output keyword, 05 Oct 1998
;      D.L Windt, Bell Labs, windt@bell-labs.com;
;   Made each PARINFO component optional, 05 Oct 1998 CM
;   Analytical derivatives allowed via AUTODERIVATIVE keyword, 09 Nov 1998
;   Parameter values can be tied to others, 09 Nov 1998
;   Fixed small bugs (Wayne Landsman), 24 Nov 1998
;   Added better exception error reporting, 24 Nov 1998 CM
;   Cosmetic documentation changes, 02 Jan 1999 CM
;   Changed definition of ITERPROC to be consistent with TNMIN, 19 Jan 1999 CM
;   Fixed bug when AUTDERIVATIVE=0.  Incorrect sign, 02 Feb 1999 CM
;   Added keyboard stop to MPFIT_DEFITER, 28 Feb 1999 CM
;   Cosmetic documentation changes, 14 May 1999 CM
;   IDL optimizations for speed & FASTNORM keyword, 15 May 1999 CM
;   Tried a faster version of mpfit_enorm, 30 May 1999 CM
;   Changed web address to cow.physics.wisc.edu, 14 Jun 1999 CM
;   Found malformation of FDJAC in MPFIT for 1 parm, 03 Aug 1999 CM
;   Factored out user-function call into MPFIT_CALL.  It is possible,
;     but currently disabled, to call procedures.  The calling format
;     is similar to CURVEFIT, 25 Sep 1999, CM
;   Slightly changed mpfit_tie to be less intrusive, 25 Sep 1999, CM
;   Fixed some bugs associated with tied parameters in mpfit_fdjac, 25
;     Sep 1999, CM
;   Reordered documentation; now alphabetical, 02 Oct 1999, CM
;   Added QUERY keyword for more robust error detection in drivers, 29
;     Oct 1999, CM
;   Documented PERROR for unweighted fits, 03 Nov 1999, CM
;   Split out MPFIT_RESETPROF to aid in profiling, 03 Nov 1999, CM
;   Some profiling and speed optimization, 03 Nov 1999, CM
;     Worst offenders, in order: fdjac2, qrfac, qrsolv, enorm.
;     fdjac2 depends on user function, qrfac and enorm seem to be
;     fully optimized.  qrsolv probably could be tweaked a little, but
;     is still <10% of total compute time.
;   Made sure that !err was set to 0 in MPFIT_DEFITER, 10 Jan 2000, CM
;   Fixed small inconsistency in setting of QANYLIM, 28 Jan 2000, CM
;   Added PARINFO field RELSTEP, 28 Jan 2000, CM
;   Converted to MPFIT_ERROR common block for indicating error
;     conditions, 28 Jan 2000, CM
;   Corrected scope of MPFIT_ERROR common block, CM, 07 Mar 2000
;   Minor speed improvement in MPFIT_ENORM, CM 26 Mar 2000
;   Corrected case where ITERPROC changed parameter values and
;     parameter values were TIED, CM 26 Mar 2000
;   Changed MPFIT_CALL to modify NFEV automatically, and to support
;     user procedures more, CM 26 Mar 2000
;   Copying permission terms have been liberalized, 26 Mar 2000, CM
;   Catch zero value of zero a(j,lj) in MPFIT_QRFAC, 20 Jul 2000, CM
;      (thanks to David Schlegel <schlegel@astro.princeton.edu>)
;   MPFIT_SETMACHAR is called only once at init; only one common block
;     is created (MPFIT_MACHAR); it is now a structure; removed almost
;     all CHECK_MATH calls for compatibility with IDL5 and !EXCEPT;
;     profiling data is now in a structure too; noted some
;     mathematical discrepancies in Linux IDL5.0, 17 Nov 2000, CM
;   Some significant changes.  New PARINFO fields: MPSIDE, MPMINSTEP,
;     MPMAXSTEP.  Improved documentation.  Now PTIED constraints are
;     maintained in the MPCONFIG common block.  A new procedure to
;     parse PARINFO fields.  FDJAC2 now computes a larger variety of
;     one-sided and two-sided finite difference derivatives.  NFEV is
;     stored in the MPCONFIG common now.  17 Dec 2000, CM
;   Added check that PARINFO and XALL have same size, 29 Dec 2000 CM
;   Don't call function in TERMINATE when there is an error, 05 Jan
;     2000
;   Check for float vs. double discrepancies; corrected implementation
;     of MIN/MAXSTEP, which I still am not sure of, but now at least
;     the correct behavior occurs *without* it, CM 08 Jan 2001
;   Added SCALE_FCN keyword, to allow for scaling, as for the CASH
;     statistic; added documentation about the theory of operation,
;     and under the QR factorization; slowly I'm beginning to
;     understand the bowels of this algorithm, CM 10 Jan 2001
;   Remove MPMINSTEP field of PARINFO, for now at least, CM 11 Jan
;     2001
;   Added RESDAMP keyword, CM, 14 Jan 2001
;   Tried to improve the DAMP handling a little, CM, 13 Mar 2001
;   Corrected .PARNAME behavior in _DEFITER, CM, 19 Mar 2001
;   Added checks for parameter and function overflow; a new STATUS
;     value to reflect this; STATUS values of -15 to -1 are reserved
;     for user function errors, CM, 03 Apr 2001
;   DAMP keyword is now a TANH, CM, 03 Apr 2001
;   Added more error checking of float vs. double, CM, 07 Apr 2001
;   Fixed bug in handling of parameter lower limits; moved overflow
;     checking to end of loop, CM, 20 Apr 2001
;   Failure using GOTO, TERMINATE more graceful if FNORM1 not defined,
;     CM, 13 Aug 2001
;   Add MPPRINT tag to PARINFO, CM, 19 Nov 2001
;   Add DOF keyword to DEFITER procedure, and print degrees of
;     freedom, CM, 28 Nov 2001
;   Add check to be sure MYFUNCT is a scalar string, CM, 14 Jan 2002
;   Addition of EXTERNAL_FJAC, EXTERNAL_FVEC keywords; ability to save
;     fitter's state from one call to the next; allow '(EXTERNAL)'
;     function name, which implies that user will supply function and
;     Jacobian at each iteration, CM, 10 Mar 2002
;   Documented EXTERNAL evaluation code, CM, 10 Mar 2002
;   Corrected signficant bug in the way that the STEP parameter, and
;     FIXED parameters interacted (Thanks Andrew Steffl), CM, 02 Apr
;     2002
;   Allow COVAR and PERROR keywords to be computed, even in case of
;     '(EXTERNAL)' function, 26 May 2002
;   Add NFREE and NPEGGED keywords; compute NPEGGED; compute DOF using
;     NFREE instead of n_elements(X), thanks to Kristian Kjaer, CM 11
;     Sep 2002
;   Hopefully PERROR is all positive now, CM 13 Sep 2002
;   Documented RELSTEP field of PARINFO (!!), CM, 25 Oct 2002
;   Error checking to detect missing start pars, CM 12 Apr 2003
;   Add DOF keyword to return degrees of freedom, CM, 30 June 2003
;   Always call ITERPROC in the final iteration; add ITERKEYSTOP
;     keyword, CM, 30 June 2003
;   Correct bug in MPFIT_LMPAR of singularity handling, which might
;     likely be fatal for one-parameter fits, CM, 21 Nov 2003
;     (with thanks to Peter Tuthill for the proper test case)
;   Minor documentation adjustment, 03 Feb 2004, CM
;   Correct small error in QR factorization when pivoting; document
;     the return values of QRFAC when pivoting, 21 May 2004, CM
;   Add MPFORMAT field to PARINFO, and correct behavior of interaction
;     between MPPRINT and PARNAME in MPFIT_DEFITERPROC (thanks to Tim
;     Robishaw), 23 May 2004, CM
;   Add the ITERPRINT keyword to allow redirecting output, 26 Sep
;     2004, CM
;   Correct MAXSTEP behavior in case of a negative parameter, 26 Sep
;     2004, CM
;   Fix bug in the parsing of MINSTEP/MAXSTEP, 10 Apr 2005, CM
;   Fix bug in the handling of upper/lower limits when the limit was
;     negative (the fitting code would never "stick" to the lower
;     limit), 29 Jun 2005, CM
;   Small documentation update for the TIED field, 05 Sep 2005, CM
;   Convert to IDL 5 array syntax (!), 16 Jul 2006, CM
;   If MAXITER equals zero, then do the basic parameter checking and
;     uncertainty analysis, but do not adjust the parameters, 15 Aug
;     2006, CM
;   Added documentation, 18 Sep 2006, CM
;   A few more IDL 5 array syntax changes, 25 Sep 2006, CM
;   Move STRICTARR compile option inside each function/procedure, 9 Oct 2006
;   Bug fix for case of MPMAXSTEP and fixed parameters, thanks
;     to Huib Intema (who found it from the Python translation!), 05 Feb 2007
;   Similar fix for MPFIT_FDJAC2 and the MPSIDE sidedness of
;     derivatives, also thanks to Huib Intema, 07 Feb 2007
;   Clarify documentation on user-function, derivatives, and PARINFO,
;     27 May 2007
;   Change the wording of "Analytic Derivatives" to "Explicit 
;     Derivatives" in the documentation, CM, 03 Sep 2007
;   Further documentation tweaks, CM, 13 Dec 2007
;   Add COMPATIBILITY section and add credits to copyright, CM, 13 Dec
;      2007
;   Document and enforce that START_PARMS and PARINFO are 1-d arrays,
;      CM, 29 Mar 2008
;   Previous change for 1-D arrays wasn't correct for
;      PARINFO.LIMITED/.LIMITS; now fixed, CM, 03 May 2008
;   Documentation adjustments, CM, 20 Aug 2008
;   Change some minor FOR-loop variables to type-long, CM, 03 Sep 2008
;   Change error handling slightly, document NOCATCH keyword,
;      document error handling in general, CM, 01 Oct 2008
;   Special case: when either LIMITS is zero, and a parameter pushes
;      against that limit, the coded that 'pegged' it there would not
;      work since it was a relative condition; now zero is handled
;      properly, CM, 08 Nov 2008
;   Documentation of how TIED interacts with LIMITS, CM, 21 Dec 2008
;   Better documentation of references, CM, 27 Feb 2009
;   If MAXITER=0, then be sure to set STATUS=5, which permits the
;      the covariance matrix to be computed, CM, 14 Apr 2009
;   Avoid numerical underflow while solving for the LM parameter,
;      (thanks to Sergey Koposov) CM, 14 Apr 2009
;   Use individual functions for all possible MPFIT_CALL permutations,
;      (and make sure the syntax is right) CM, 01 Sep 2009
;   Correct behavior of MPMAXSTEP when some parameters are frozen,
;      thanks to Josh Destree, CM, 22 Nov 2009
;   Update the references section, CM, 22 Nov 2009
;   1.70 - Add the VERSION and MIN_VERSION keywords, CM, 22 Nov 2009
;   1.71 - Store pre-calculated revision in common, CM, 23 Nov 2009
;   1.72-1.74 - Documented alternate method to compute correlation matrix,
;          CM, 05 Feb 2010
;   1.75 - Enforce TIED constraints when preparing to terminate the
;          routine, CM, 2010-06-22
;   1.76 - Documented input keywords now are not modified upon output,
;          CM, 2010-07-13
;   1.77 - Upon user request (/CALC_FJAC), compute Jacobian matrix and
;          return in BEST_FJAC; also return best residuals in
;          BEST_RESID; also return an index list of free parameters as
;          PFREE_INDEX; add a fencepost to prevent recursion
;          CM, 2010-10-27
;   1.79 - Documentation corrections.  CM, 2011-08-26
;   1.81 - Fix bug in interaction of AUTODERIVATIVE=0 and .MPSIDE=3;
;          Document FJAC_MASK. CM, 2012-05-08
;   1.83 - Trap more overflow conditions (ref. Nirmal Iyer), CM 2013-12-23
;   1.84 - More robust handling of FNORM, CM 2016-05-19
;   1.85 - Add MPFORMAT_PARNAME for explicit formatting of printed
;          parms, CM, 2017-01-03
;
;  $Id: mpfit.pro,v 1.85 2017/01/03 19:08:14 cmarkwar Exp $
;-
; Original MINPACK by More' Garbow and Hillstrom, translated with permission
; Modifications and enhancements are:
; Copyright (C) 1997-2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2017 Craig Markwardt
; This software is provided as is without any warranty whatsoever.
; Permission to use, copy, modify, and distribute modified or
; unmodified copies is granted, provided this copyright and disclaimer
; are included unchanged.
;-

pro mpfit_dummy
  ;; Enclose in a procedure so these are not defined in the main level
  COMPILE_OPT strictarr
  FORWARD_FUNCTION mpfit_fdjac2, mpfit_enorm, mpfit_lmpar, mpfit_covar, $
    mpfit, mpfit_call

  COMMON mpfit_error, error_code  ;; For error passing to user function
  COMMON mpfit_config, mpconfig   ;; For internal error configrations
end

;; Reset profiling registers for another run.  By default, and when
;; uncommented, the profiling registers simply accumulate.

pro mpfit_resetprof
  COMPILE_OPT strictarr
  common mpfit_profile, mpfit_profile_vals

  mpfit_profile_vals = { status: 1L, fdjac2: 0D, lmpar: 0D, mpfit: 0D, $
                         qrfac: 0D,  qrsolv: 0D, enorm: 0D}
  return
end

;; Following are machine constants that can be loaded once.  I have
;; found that bizarre underflow messages can be produced in each call
;; to MACHAR(), so this structure minimizes the number of calls to
;; one.

pro mpfit_setmachar, double=isdouble
  COMPILE_OPT strictarr
  common mpfit_profile, profvals
  if n_elements(profvals) EQ 0 then mpfit_resetprof

  common mpfit_machar, mpfit_machar_vals

  ;; In earlier versions of IDL, MACHAR itself could produce a load of
  ;; error messages.  We try to mask some of that out here.
  if (!version.release) LT 5 then dummy = check_math(1, 1)

  mch = 0.
  mch = machar(double=keyword_set(isdouble))
  dmachep = mch.eps
  dmaxnum = mch.xmax
  dminnum = mch.xmin
  dmaxlog = alog(mch.xmax)
  dminlog = alog(mch.xmin)
  if keyword_set(isdouble) then $
    dmaxgam = 171.624376956302725D $
  else $
    dmaxgam = 171.624376956302725
  drdwarf = sqrt(dminnum*1.5) * 10
  drgiant = sqrt(dmaxnum) * 0.1

  mpfit_machar_vals = {machep: dmachep, maxnum: dmaxnum, minnum: dminnum, $
                       maxlog: dmaxlog, minlog: dminlog, maxgam: dmaxgam, $
                       rdwarf: drdwarf, rgiant: drgiant}

  if (!version.release) LT 5 then dummy = check_math(0, 0)

  return
end


; Call user function with no _EXTRA parameters
function mpfit_call_func_noextra, fcn, x, fjac, _EXTRA=extra
  if n_params() EQ 2 then begin
     return, call_function(fcn, x)
  endif else begin
     return, call_function(fcn, x, fjac)
  endelse
end

; Call user function with _EXTRA parameters
function mpfit_call_func_extra, fcn, x, fjac, _EXTRA=extra
  if n_params() EQ 2 then begin
     return, call_function(fcn, x, _EXTRA=extra)
  endif else begin
     return, call_function(fcn, x, fjac, _EXTRA=extra)
  endelse
end

; Call user procedure with no _EXTRA parameters
function mpfit_call_pro_noextra, fcn, x, fjac, _EXTRA=extra
  if n_params() EQ 2 then begin
     call_procedure, fcn, x, f
  endif else begin
     call_procedure, fcn, x, f, fjac
  endelse
  return, f
end

; Call user procedure with _EXTRA parameters
function mpfit_call_pro_extra, fcn, x, fjac, _EXTRA=extra
  if n_params() EQ 2 then begin
     call_procedure, fcn, x, f, _EXTRA=extra
  endif else begin
     call_procedure, fcn, x, f, fjac, _EXTRA=extra
  endelse
  return, f
end


;; Call user function or procedure, with _EXTRA or not, with
;; derivatives or not.
function mpfit_call, fcn, x, fjac, _EXTRA=extra

  COMPILE_OPT strictarr
  common mpfit_config, mpconfig

  if keyword_set(mpconfig.qanytied) then mpfit_tie, x, mpconfig.ptied

  ;; Decide whether we are calling a procedure or function, and 
  ;; with/without FUNCTARGS
  proname = 'MPFIT_CALL'
  proname = proname + ((mpconfig.proc) ? '_PRO' : '_FUNC')
  proname = proname + ((n_elements(extra) GT 0) ? '_EXTRA' : '_NOEXTRA')

  if n_params() EQ 2 then begin
     f = call_function(proname, fcn, x, _EXTRA=extra)
  endif else begin
     f = call_function(proname, fcn, x, fjac, _EXTRA=extra)
  endelse
  mpconfig.nfev = mpconfig.nfev + 1

  if n_params() EQ 2 AND mpconfig.damp GT 0 then begin
      damp = mpconfig.damp[0]
      
      ;; Apply the damping if requested.  This replaces the residuals
      ;; with their hyperbolic tangent.  Thus residuals larger than
      ;; DAMP are essentially clipped.
      f = tanh(f/damp)
  endif

  return, f
end

function mpfit_fdjac2, fcn, x, fvec, step, ulimited, ulimit, dside, $
                 iflag=iflag, epsfcn=epsfcn, autoderiv=autoderiv, $
                 FUNCTARGS=fcnargs, xall=xall, ifree=ifree, dstep=dstep, $
                 deriv_debug=ddebug, deriv_reltol=ddrtol, deriv_abstol=ddatol

  COMPILE_OPT strictarr
  common mpfit_machar, machvals
  common mpfit_profile, profvals
  common mpfit_error, mperr

;  prof_start = systime(1)
  MACHEP0 = machvals.machep
  DWARF   = machvals.minnum

  if n_elements(epsfcn) EQ 0 then epsfcn = MACHEP0
  if n_elements(xall)   EQ 0 then xall = x
  if n_elements(ifree)  EQ 0 then ifree = lindgen(n_elements(xall))
  if n_elements(step)   EQ 0 then step = x * 0.
  if n_elements(ddebug) EQ 0 then ddebug = intarr(n_elements(xall))
  if n_elements(ddrtol) EQ 0 then ddrtol = x * 0.
  if n_elements(ddatol) EQ 0 then ddatol = x * 0.
  has_debug_deriv = max(ddebug)

  if keyword_set(has_debug_deriv) then begin
      ;; Header for debugging
      print, 'FJAC DEBUG BEGIN'
      print, "IPNT", "FUNC", "DERIV_U", "DERIV_N", "DIFF_ABS", "DIFF_REL", $
        format='("#  ",A10," ",A10," ",A10," ",A10," ",A10," ",A10)'
  endif

  nall = n_elements(xall)

  eps = sqrt(max([epsfcn, MACHEP0]));
  m = n_elements(fvec)
  n = n_elements(x)

  ;; Compute analytical derivative if requested
  ;; Two ways to enable computation of explicit derivatives:
  ;;   1. AUTODERIVATIVE=0
  ;;   2. AUTODERIVATIVE=1, but P[i].MPSIDE EQ 3

  if keyword_set(autoderiv) EQ 0 OR max(dside[ifree] EQ 3) EQ 1 then begin
      fjac_mask = intarr(nall)
      ;; Specify which parameters need derivatives
      ;;                  ---- Case 2 ------     ----- Case 1 -----
      fjac_mask[ifree] = (dside[ifree] EQ 3) OR (keyword_set(autoderiv) EQ 0)
      if has_debug_deriv then $
        print, fjac_mask, format='("# FJAC_MASK = ",100000(I0," ",:))'

      fjac = fjac_mask  ;; Pass the mask to the calling function as FJAC
      mperr = 0
      fp = mpfit_call(fcn, xall, fjac, _EXTRA=fcnargs)
      iflag = mperr

      if n_elements(fjac) NE m*nall then begin
          message, /cont, 'ERROR: Derivative matrix was not computed properly.'
          iflag = 1
;          profvals.fdjac2 = profvals.fdjac2 + (systime(1) - prof_start)
          return, 0
      endif

      ;; This definition is consistent with CURVEFIT (WRONG, see below)
      ;; Sign error found (thanks Jesus Fernandez <fernande@irm.chu-caen.fr>)

      ;; ... and now I regret doing this sign flip since it's not
      ;; strictly correct.  The definition should be RESID =
      ;; (Y-F)/SIGMA, so d(RESID)/dP should be -dF/dP.  My response to
      ;; Fernandez was unfounded because he was trying to supply
      ;; dF/dP.  Sigh. (CM 31 Aug 2007)

      fjac = reform(-temporary(fjac), m, nall, /overwrite)

      ;; Select only the free parameters
      if n_elements(ifree) LT nall then $
        fjac = reform(fjac[*,ifree], m, n, /overwrite)
      
      ;; Are we done computing derivatives?  The answer is, YES, if we
      ;; computed explicit derivatives for all free parameters, EXCEPT
      ;; when we are going on to compute debugging derivatives.
      if min(fjac_mask[ifree]) EQ 1 AND NOT has_debug_deriv then begin
          return, fjac
      endif
  endif

  ;; Final output array, if it was not already created above
  if n_elements(fjac) EQ 0 then begin
      fjac = make_array(m, n, value=fvec[0]*0.)
      fjac = reform(fjac, m, n, /overwrite)
  endif

  h = eps * abs(x)

  ;; if STEP is given, use that
  ;; STEP includes the fixed parameters
  if n_elements(step) GT 0 then begin
      stepi = step[ifree]
      wh = where(stepi GT 0, ct)
      if ct GT 0 then h[wh] = stepi[wh]
  endif

  ;; if relative step is given, use that
  ;; DSTEP includes the fixed parameters
  if n_elements(dstep) GT 0 then begin
      dstepi = dstep[ifree]
      wh = where(dstepi GT 0, ct)
      if ct GT 0 then h[wh] = abs(dstepi[wh]*x[wh])
  endif

  ;; In case any of the step values are zero
  wh = where(h EQ 0, ct)
  if ct GT 0 then h[wh] = eps

  ;; Reverse the sign of the step if we are up against the parameter
  ;; limit, or if the user requested it.
  ;; DSIDE includes the fixed parameters (ULIMITED/ULIMIT have only
  ;; varying ones)
  mask = dside[ifree] EQ -1
  if n_elements(ulimited) GT 0 AND n_elements(ulimit) GT 0 then $
    mask = mask OR (ulimited AND (x GT ulimit-h))
  wh = where(mask, ct)
  if ct GT 0 then h[wh] = -h[wh]

  ;; Loop through parameters, computing the derivative for each
  for j=0L, n-1 do begin
      dsidej = dside[ifree[j]]
      ddebugj = ddebug[ifree[j]]

      ;; Skip this parameter if we already computed its derivative
      ;; explicitly, and we are not debugging.
      if (dsidej EQ 3) AND (ddebugj EQ 0) then continue
      if (dsidej EQ 3) AND (ddebugj EQ 1) then $
        print, ifree[j], format='("FJAC PARM ",I0)'

      xp = xall
      xp[ifree[j]] = xp[ifree[j]] + h[j]
      
      mperr = 0
      fp = mpfit_call(fcn, xp, _EXTRA=fcnargs)
      
      iflag = mperr
      if iflag LT 0 then return, !values.d_nan

      if ((dsidej GE -1) AND (dsidej LE 1)) OR (dsidej EQ 3) then begin
          ;; COMPUTE THE ONE-SIDED DERIVATIVE
          ;; Note optimization fjac(0:*,j)
          fjacj = (fp-fvec)/h[j]

      endif else begin
          ;; COMPUTE THE TWO-SIDED DERIVATIVE
          xp[ifree[j]] = xall[ifree[j]] - h[j]

          mperr = 0
          fm = mpfit_call(fcn, xp, _EXTRA=fcnargs)
          
          iflag = mperr
          if iflag LT 0 then return, !values.d_nan
          
          ;; Note optimization fjac(0:*,j)
          fjacj = (fp-fm)/(2*h[j])
      endelse          
      
      ;; Debugging of explicit derivatives
      if (dsidej EQ 3) AND (ddebugj EQ 1) then begin
          ;; Relative and absolute tolerances
          dr = ddrtol[ifree[j]] & da = ddatol[ifree[j]]

          ;; Explicitly calculated
          fjaco = fjac[*,j]
          
          ;; If tolerances are zero, then any value for deriv triggers print...
          if (da EQ 0 AND dr EQ 0) then $
            diffj = (fjaco NE 0 OR fjacj NE 0)
          ;; ... otherwise the difference must be a greater than tolerance
          if (da NE 0 OR dr NE 0) then $
            diffj = (abs(fjaco-fjacj) GT (da+abs(fjaco)*dr))

          for k = 0L, m-1 do if diffj[k] then begin
              print, k, fvec[k], fjaco[k], fjacj[k], fjaco[k]-fjacj[k], $
                (fjaco[k] EQ 0)?(0):((fjaco[k]-fjacj[k])/fjaco[k]), $
                format='("   ",I10," ",G10.4," ",G10.4," ",G10.4," ",G10.4," ",G10.4)'
          endif
      endif

      ;; Store final results in output array
      fjac[0,j] = fjacj
          
  endfor

  if has_debug_deriv then print, 'FJAC DEBUG END'

;  profvals.fdjac2 = profvals.fdjac2 + (systime(1) - prof_start)
  return, fjac
end

function mpfit_enorm, vec

  COMPILE_OPT strictarr
  ;; NOTE: it turns out that, for systems that have a lot of data
  ;; points, this routine is a big computing bottleneck.  The extended
  ;; computations that need to be done cannot be effectively
  ;; vectorized.  The introduction of the FASTNORM configuration
  ;; parameter allows the user to select a faster routine, which is 
  ;; based on TOTAL() alone.
  common mpfit_profile, profvals
;  prof_start = systime(1)

  common mpfit_config, mpconfig
; Very simple-minded sum-of-squares
  if n_elements(mpconfig) GT 0 then if mpconfig.fastnorm then begin
      ans = sqrt(total(vec^2))
      goto, TERMINATE
  endif

  common mpfit_machar, machvals

  agiant = machvals.rgiant / n_elements(vec)
  adwarf = machvals.rdwarf * n_elements(vec)

  ;; This is hopefully a compromise between speed and robustness.
  ;; Need to do this because of the possibility of over- or underflow.
  mx = max(vec, min=mn)
  mx = max(abs([mx,mn]))
  if mx EQ 0 then return, vec[0]*0.

  if mx GT agiant OR mx LT adwarf then ans = mx * sqrt(total((vec/mx)^2))$
  else                                 ans = sqrt( total(vec^2) )

  TERMINATE:
;  profvals.enorm = profvals.enorm + (systime(1) - prof_start)
  return, ans
end

;     **********
;
;     subroutine qrfac
;
;     this subroutine uses householder transformations with column
;     pivoting (optional) to compute a qr factorization of the
;     m by n matrix a. that is, qrfac determines an orthogonal
;     matrix q, a permutation matrix p, and an upper trapezoidal
;     matrix r with diagonal elements of nonincreasing magnitude,
;     such that a*p = q*r. the householder transformation for
;     column k, k = 1,2,...,min(m,n), is of the form
;
;			    t
;	    i - (1/u(k))*u*u
;
;     where u has zeros in the first k-1 positions. the form of
;     this transformation and the method of pivoting first
;     appeared in the corresponding linpack subroutine.
;
;     the subroutine statement is
;
;	subroutine qrfac(m,n,a,lda,pivot,ipvt,lipvt,rdiag,acnorm,wa)
;
;     where
;
;	m is a positive integer input variable set to the number
;	  of rows of a.
;
;	n is a positive integer input variable set to the number
;	  of columns of a.
;
;	a is an m by n array. on input a contains the matrix for
;	  which the qr factorization is to be computed. on output
;	  the strict upper trapezoidal part of a contains the strict
;	  upper trapezoidal part of r, and the lower trapezoidal
;	  part of a contains a factored form of q (the non-trivial
;	  elements of the u vectors described above).
;
;	lda is a positive integer input variable not less than m
;	  which specifies the leading dimension of the array a.
;
;	pivot is a logical input variable. if pivot is set true,
;	  then column pivoting is enforced. if pivot is set false,
;	  then no column pivoting is done.
;
;	ipvt is an integer output array of length lipvt. ipvt
;	  defines the permutation matrix p such that a*p = q*r.
;	  column j of p is column ipvt(j) of the identity matrix.
;	  if pivot is false, ipvt is not referenced.
;
;	lipvt is a positive integer input variable. if pivot is false,
;	  then lipvt may be as small as 1. if pivot is true, then
;	  lipvt must be at least n.
;
;	rdiag is an output array of length n which contains the
;	  diagonal elements of r.
;
;	acnorm is an output array of length n which contains the
;	  norms of the corresponding columns of the input matrix a.
;	  if this information is not needed, then acnorm can coincide
;	  with rdiag.
;
;	wa is a work array of length n. if pivot is false, then wa
;	  can coincide with rdiag.
;
;     subprograms called
;
;	minpack-supplied ... dpmpar,enorm
;
;	fortran-supplied ... dmax1,dsqrt,min0
;
;     argonne national laboratory. minpack project. march 1980.
;     burton s. garbow, kenneth e. hillstrom, jorge j. more
;
;     **********
;
; PIVOTING / PERMUTING:
;
; Upon return, A(*,*) is in standard parameter order, A(*,IPVT) is in
; permuted order.
;
; RDIAG is in permuted order.
;
; ACNORM is in standard parameter order.
;
; NOTE: in IDL the factors appear slightly differently than described
; above.  The matrix A is still m x n where m >= n.  
;
; The "upper" triangular matrix R is actually stored in the strict
; lower left triangle of A under the standard notation of IDL.
;
; The reflectors that generate Q are in the upper trapezoid of A upon
; output.
;
;  EXAMPLE:  decompose the matrix [[9.,2.,6.],[4.,8.,7.]]
;    aa = [[9.,2.,6.],[4.,8.,7.]]
;    mpfit_qrfac, aa, aapvt, rdiag, aanorm
;     IDL> print, aa
;          1.81818*     0.181818*     0.545455*
;         -8.54545+      1.90160*     0.432573*
;     IDL> print, rdiag
;         -11.0000+     -7.48166+
;
; The components marked with a * are the components of the
; reflectors, and those marked with a + are components of R.
;
; To reconstruct Q and R we proceed as follows.  First R.
;    r = fltarr(m, n)
;    for i = 0, n-1 do r(0:i,i) = aa(0:i,i)  ; fill in lower diag
;    r(lindgen(n)*(m+1)) = rdiag
;
; Next, Q, which are composed from the reflectors.  Each reflector v
; is taken from the upper trapezoid of aa, and converted to a matrix
; via (I - 2 vT . v / (v . vT)).
;
;   hh = ident                                    ;; identity matrix
;   for i = 0, n-1 do begin
;    v = aa(*,i) & if i GT 0 then v(0:i-1) = 0    ;; extract reflector
;    hh = hh ## (ident - 2*(v # v)/total(v * v))  ;; generate matrix
;   endfor
;
; Test the result:
;    IDL> print, hh ## transpose(r)
;          9.00000      4.00000
;          2.00000      8.00000
;          6.00000      7.00000
;
; Note that it is usually never necessary to form the Q matrix
; explicitly, and MPFIT does not.

pro mpfit_qrfac, a, ipvt, rdiag, acnorm, pivot=pivot

  COMPILE_OPT strictarr
  sz = size(a)
  m = sz[1]
  n = sz[2]

  common mpfit_machar, machvals
  common mpfit_profile, profvals
;  prof_start = systime(1)

  MACHEP0 = machvals.machep
  DWARF   = machvals.minnum
  
  ;; Compute the initial column norms and initialize arrays
  acnorm = make_array(n, value=a[0]*0.)
  for j = 0L, n-1 do $
    acnorm[j] = mpfit_enorm(a[*,j])
  rdiag = acnorm
  wa = rdiag
  ipvt = lindgen(n)

  ;; Reduce a to r with householder transformations
  minmn = min([m,n])
  for j = 0L, minmn-1 do begin
      if NOT keyword_set(pivot) then goto, HOUSE1
      
      ;; Bring the column of largest norm into the pivot position
      rmax = max(rdiag[j:*])
      kmax = where(rdiag[j:*] EQ rmax, ct) + j
      if ct LE 0 then goto, HOUSE1
      kmax = kmax[0]
      
      ;; Exchange rows via the pivot only.  Avoid actually exchanging
      ;; the rows, in case there is lots of memory transfer.  The
      ;; exchange occurs later, within the body of MPFIT, after the
      ;; extraneous columns of the matrix have been shed.
      if kmax NE j then begin
          temp     = ipvt[j]   & ipvt[j]    = ipvt[kmax] & ipvt[kmax]  = temp
          rdiag[kmax] = rdiag[j]
          wa[kmax]    = wa[j]
      endif
      
      HOUSE1:

      ;; Compute the householder transformation to reduce the jth
      ;; column of A to a multiple of the jth unit vector
      lj     = ipvt[j]
      ajj    = a[j:*,lj]
      ajnorm = mpfit_enorm(ajj)
      if ajnorm EQ 0 then goto, NEXT_ROW
      if a[j,lj] LT 0 then ajnorm = -ajnorm
      
      ajj     = ajj / ajnorm
      ajj[0]  = ajj[0] + 1
      ;; *** Note optimization a(j:*,j)
      a[j,lj] = ajj
      
      ;; Apply the transformation to the remaining columns
      ;; and update the norms

      ;; NOTE to SELF: tried to optimize this by removing the loop,
      ;; but it actually got slower.  Reverted to "for" loop to keep
      ;; it simple.
      if j+1 LT n then begin
          for k=j+1, n-1 do begin
              lk = ipvt[k]
              ajk = a[j:*,lk]
              ;; *** Note optimization a(j:*,lk) 
              ;; (corrected 20 Jul 2000)
              if a[j,lj] NE 0 then $
                a[j,lk] = ajk - ajj * total(ajk*ajj)/a[j,lj]

              if keyword_set(pivot) AND rdiag[k] NE 0 then begin
                  temp = a[j,lk]/rdiag[k]
                  rdiag[k] = rdiag[k] * sqrt((1.-temp^2) > 0)
                  temp = rdiag[k]/wa[k]
                  if 0.05D*temp*temp LE MACHEP0 then begin
                      rdiag[k] = mpfit_enorm(a[j+1:*,lk])
                      wa[k] = rdiag[k]
                  endif
              endif
          endfor
      endif

      NEXT_ROW:
      rdiag[j] = -ajnorm
  endfor

;  profvals.qrfac = profvals.qrfac + (systime(1) - prof_start)
  return
end

;     **********
;
;     subroutine qrsolv
;
;     given an m by n matrix a, an n by n diagonal matrix d,
;     and an m-vector b, the problem is to determine an x which
;     solves the system
;
;           a*x = b ,     d*x = 0 ,
;
;     in the least squares sense.
;
;     this subroutine completes the solution of the problem
;     if it is provided with the necessary information from the
;     qr factorization, with column pivoting, of a. that is, if
;     a*p = q*r, where p is a permutation matrix, q has orthogonal
;     columns, and r is an upper triangular matrix with diagonal
;     elements of nonincreasing magnitude, then qrsolv expects
;     the full upper triangle of r, the permutation matrix p,
;     and the first n components of (q transpose)*b. the system
;     a*x = b, d*x = 0, is then equivalent to
;
;                  t       t
;           r*z = q *b ,  p *d*p*z = 0 ,
;
;     where x = p*z. if this system does not have full rank,
;     then a least squares solution is obtained. on output qrsolv
;     also provides an upper triangular matrix s such that
;
;            t   t               t
;           p *(a *a + d*d)*p = s *s .
;
;     s is computed within qrsolv and may be of separate interest.
;
;     the subroutine statement is
;
;       subroutine qrsolv(n,r,ldr,ipvt,diag,qtb,x,sdiag,wa)
;
;     where
;
;       n is a positive integer input variable set to the order of r.
;
;       r is an n by n array. on input the full upper triangle
;         must contain the full upper triangle of the matrix r.
;         on output the full upper triangle is unaltered, and the
;         strict lower triangle contains the strict upper triangle
;         (transposed) of the upper triangular matrix s.
;
;       ldr is a positive integer input variable not less than n
;         which specifies the leading dimension of the array r.
;
;       ipvt is an integer input array of length n which defines the
;         permutation matrix p such that a*p = q*r. column j of p
;         is column ipvt(j) of the identity matrix.
;
;       diag is an input array of length n which must contain the
;         diagonal elements of the matrix d.
;
;       qtb is an input array of length n which must contain the first
;         n elements of the vector (q transpose)*b.
;
;       x is an output array of length n which contains the least
;         squares solution of the system a*x = b, d*x = 0.
;
;       sdiag is an output array of length n which contains the
;         diagonal elements of the upper triangular matrix s.
;
;       wa is a work array of length n.
;
;     subprograms called
;
;       fortran-supplied ... dabs,dsqrt
;
;     argonne national laboratory. minpack project. march 1980.
;     burton s. garbow, kenneth e. hillstrom, jorge j. more
;
pro mpfit_qrsolv, r, ipvt, diag, qtb, x, sdiag

  COMPILE_OPT strictarr
  sz = size(r)
  m = sz[1]
  n = sz[2]
  delm = lindgen(n) * (m+1) ;; Diagonal elements of r

  common mpfit_profile, profvals
;  prof_start = systime(1)

  ;; copy r and (q transpose)*b to preserve input and initialize s.
  ;; in particular, save the diagonal elements of r in x.

  for j = 0L, n-1 do $
    r[j:n-1,j] = r[j,j:n-1]
  x = r[delm]
  wa = qtb
  ;; Below may look strange, but it's so we can keep the right precision
  zero = qtb[0]*0.
  half = zero + 0.5
  quart = zero + 0.25

  ;; Eliminate the diagonal matrix d using a givens rotation
  for j = 0L, n-1 do begin
      l = ipvt[j]
      if diag[l] EQ 0 then goto, STORE_RESTORE
      sdiag[j:*] = 0
      sdiag[j] = diag[l]

      ;; The transformations to eliminate the row of d modify only a
      ;; single element of (q transpose)*b beyond the first n, which
      ;; is initially zero.

      qtbpj = zero
      for k = j, n-1 do begin
          if sdiag[k] EQ 0 then goto, ELIM_NEXT_LOOP
          if abs(r[k,k]) LT abs(sdiag[k]) then begin
              cotan  = r[k,k]/sdiag[k]
              sine   = half/sqrt(quart + quart*cotan*cotan)
              cosine = sine*cotan
          endif else begin
              tang   = sdiag[k]/r[k,k]
              cosine = half/sqrt(quart + quart*tang*tang)
              sine   = cosine*tang
          endelse
          
          ;; Compute the modified diagonal element of r and the
          ;; modified element of ((q transpose)*b,0).
          r[k,k] = cosine*r[k,k] + sine*sdiag[k]
          temp = cosine*wa[k] + sine*qtbpj
          qtbpj = -sine*wa[k] + cosine*qtbpj
          wa[k] = temp

          ;; Accumulate the transformation in the row of s
          if n GT k+1 then begin
              temp = cosine*r[k+1:n-1,k] + sine*sdiag[k+1:n-1]
              sdiag[k+1:n-1] = -sine*r[k+1:n-1,k] + cosine*sdiag[k+1:n-1]
              r[k+1:n-1,k] = temp
          endif
ELIM_NEXT_LOOP:
      endfor

STORE_RESTORE:
      sdiag[j] = r[j,j]
      r[j,j] = x[j]
  endfor

  ;; Solve the triangular system for z.  If the system is singular
  ;; then obtain a least squares solution
  nsing = n
  wh = where(sdiag EQ 0, ct)
  if ct GT 0 then begin
      nsing = wh[0]
      wa[nsing:*] = 0
  endif

  if nsing GE 1 then begin
      wa[nsing-1] = wa[nsing-1]/sdiag[nsing-1] ;; Degenerate case
      ;; *** Reverse loop ***
      for j=nsing-2,0,-1 do begin  
          sum = total(r[j+1:nsing-1,j]*wa[j+1:nsing-1])
          wa[j] = (wa[j]-sum)/sdiag[j]
      endfor
  endif

  ;; Permute the components of z back to components of x
  x[ipvt] = wa

;  profvals.qrsolv = profvals.qrsolv + (systime(1) - prof_start)
  return
end
      
  
;
;     subroutine lmpar
;
;     given an m by n matrix a, an n by n nonsingular diagonal
;     matrix d, an m-vector b, and a positive number delta,
;     the problem is to determine a value for the parameter
;     par such that if x solves the system
;
;	    a*x = b ,	  sqrt(par)*d*x = 0 ,
;
;     in the least squares sense, and dxnorm is the euclidean
;     norm of d*x, then either par is zero and
;
;	    (dxnorm-delta) .le. 0.1*delta ,
;
;     or par is positive and
;
;	    abs(dxnorm-delta) .le. 0.1*delta .
;
;     this subroutine completes the solution of the problem
;     if it is provided with the necessary information from the
;     qr factorization, with column pivoting, of a. that is, if
;     a*p = q*r, where p is a permutation matrix, q has orthogonal
;     columns, and r is an upper triangular matrix with diagonal
;     elements of nonincreasing magnitude, then lmpar expects
;     the full upper triangle of r, the permutation matrix p,
;     and the first n components of (q transpose)*b. on output
;     lmpar also provides an upper triangular matrix s such that
;
;	     t	 t		     t
;	    p *(a *a + par*d*d)*p = s *s .
;
;     s is employed within lmpar and may be of separate interest.
;
;     only a few iterations are generally needed for convergence
;     of the algorithm. if, however, the limit of 10 iterations
;     is reached, then the output par will contain the best
;     value obtained so far.
;
;     the subroutine statement is
;
;	subroutine lmpar(n,r,ldr,ipvt,diag,qtb,delta,par,x,sdiag,
;			 wa1,wa2)
;
;     where
;
;	n is a positive integer input variable set to the order of r.
;
;	r is an n by n array. on input the full upper triangle
;	  must contain the full upper triangle of the matrix r.
;	  on output the full upper triangle is unaltered, and the
;	  strict lower triangle contains the strict upper triangle
;	  (transposed) of the upper triangular matrix s.
;
;	ldr is a positive integer input variable not less than n
;	  which specifies the leading dimension of the array r.
;
;	ipvt is an integer input array of length n which defines the
;	  permutation matrix p such that a*p = q*r. column j of p
;	  is column ipvt(j) of the identity matrix.
;
;	diag is an input array of length n which must contain the
;	  diagonal elements of the matrix d.
;
;	qtb is an input array of length n which must contain the first
;	  n elements of the vector (q transpose)*b.
;
;	delta is a positive input variable which specifies an upper
;	  bound on the euclidean norm of d*x.
;
;	par is a nonnegative variable. on input par contains an
;	  initial estimate of the levenberg-marquardt parameter.
;	  on output par contains the final estimate.
;
;	x is an output array of length n which contains the least
;	  squares solution of the system a*x = b, sqrt(par)*d*x = 0,
;	  for the output par.
;
;	sdiag is an output array of length n which contains the
;	  diagonal elements of the upper triangular matrix s.
;
;	wa1 and wa2 are work arrays of length n.
;
;     subprograms called
;
;	minpack-supplied ... dpmpar,enorm,qrsolv
;
;	fortran-supplied ... dabs,dmax1,dmin1,dsqrt
;
;     argonne national laboratory. minpack project. march 1980.
;     burton s. garbow, kenneth e. hillstrom, jorge j. more
;
function mpfit_lmpar, r, ipvt, diag, qtb, delta, x, sdiag, par=par

  COMPILE_OPT strictarr
  common mpfit_machar, machvals
  common mpfit_profile, profvals
;  prof_start = systime(1)

  MACHEP0 = machvals.machep
  DWARF   = machvals.minnum

  sz = size(r)
  m = sz[1]
  n = sz[2]
  delm = lindgen(n) * (m+1) ;; Diagonal elements of r

  ;; Compute and store in x the gauss-newton direction.  If the
  ;; jacobian is rank-deficient, obtain a least-squares solution
  nsing = n
  wa1 = qtb
  rthresh = max(abs(r[delm]))*MACHEP0
  wh = where(abs(r[delm]) LT rthresh, ct)
  if ct GT 0 then begin
      nsing = wh[0]
      wa1[wh[0]:*] = 0
  endif

  if nsing GE 1 then begin
      ;; *** Reverse loop ***
      for j=nsing-1,0,-1 do begin  
          wa1[j] = wa1[j]/r[j,j]
          if (j-1 GE 0) then $
            wa1[0:(j-1)] = wa1[0:(j-1)] - r[0:(j-1),j]*wa1[j]
      endfor
  endif

  ;; Note: ipvt here is a permutation array
  x[ipvt] = wa1

  ;; Initialize the iteration counter.  Evaluate the function at the
  ;; origin, and test for acceptance of the gauss-newton direction
  iter = 0L
  wa2 = diag * x
  dxnorm = mpfit_enorm(wa2)
  fp = dxnorm - delta
  if fp LE 0.1*delta then goto, TERMINATE

  ;; If the jacobian is not rank deficient, the newton step provides a
  ;; lower bound, parl, for the zero of the function.  Otherwise set
  ;; this bound to zero.
  
  zero = wa2[0]*0.
  parl = zero
  if nsing GE n then begin
      wa1 = diag[ipvt]*wa2[ipvt]/dxnorm

      wa1[0] = wa1[0] / r[0,0] ;; Degenerate case 
      for j=1L, n-1 do begin   ;; Note "1" here, not zero
          sum = total(r[0:(j-1),j]*wa1[0:(j-1)])
          wa1[j] = (wa1[j] - sum)/r[j,j]
      endfor

      temp = mpfit_enorm(wa1)
      parl = ((fp/delta)/temp)/temp
  endif

  ;; Calculate an upper bound, paru, for the zero of the function
  for j=0L, n-1 do begin
      sum = total(r[0:j,j]*qtb[0:j])
      wa1[j] = sum/diag[ipvt[j]]
  endfor
  gnorm = mpfit_enorm(wa1)
  paru  = gnorm/delta
  if paru EQ 0 then paru = DWARF/min([delta,0.1])

  ;; If the input par lies outside of the interval (parl,paru), set
  ;; par to the closer endpoint

  par = max([par,parl])
  par = min([par,paru])
  if par EQ 0 then par = gnorm/dxnorm

  ;; Beginning of an interation
  ITERATION:
  iter = iter + 1
  
  ;; Evaluate the function at the current value of par
  if par EQ 0 then par = max([DWARF, paru*0.001])
  temp = sqrt(par)
  wa1 = temp * diag
  mpfit_qrsolv, r, ipvt, wa1, qtb, x, sdiag
  wa2 = diag*x
  dxnorm = mpfit_enorm(wa2)
  temp = fp
  fp = dxnorm - delta

  if (abs(fp) LE 0.1D*delta) $
    OR ((parl EQ 0) AND (fp LE temp) AND (temp LT 0)) $
    OR (iter EQ 10) then goto, TERMINATE

  ;; Compute the newton correction
  wa1 = diag[ipvt]*wa2[ipvt]/dxnorm

  for j=0L,n-2 do begin
      wa1[j] = wa1[j]/sdiag[j]
      wa1[j+1:n-1] = wa1[j+1:n-1] - r[j+1:n-1,j]*wa1[j]
  endfor
  wa1[n-1] = wa1[n-1]/sdiag[n-1] ;; Degenerate case

  temp = mpfit_enorm(wa1)
  parc = ((fp/delta)/temp)/temp

  ;; Depending on the sign of the function, update parl or paru
  if fp GT 0 then parl = max([parl,par])
  if fp LT 0 then paru = min([paru,par])

  ;; Compute an improved estimate for par
  par = max([parl, par+parc])

  ;; End of an iteration
  goto, ITERATION
  
TERMINATE:
  ;; Termination
;  profvals.lmpar = profvals.lmpar + (systime(1) - prof_start)
  if iter EQ 0 then return, par[0]*0.
  return, par
end

;; Procedure to tie one parameter to another.
pro mpfit_tie, p, _ptied
  COMPILE_OPT strictarr
  if n_elements(_ptied) EQ 0 then return
  if n_elements(_ptied) EQ 1 then if _ptied[0] EQ '' then return
  for _i = 0L, n_elements(_ptied)-1 do begin
      if _ptied[_i] EQ '' then goto, NEXT_TIE
      _cmd = 'p['+strtrim(_i,2)+'] = '+_ptied[_i]
      _err = execute(_cmd)
      if _err EQ 0 then begin
          message, 'ERROR: Tied expression "'+_cmd+'" failed.'
          return
      endif
      NEXT_TIE:
  endfor
end

;; Default print procedure
pro mpfit_defprint, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, $
                    p11, p12, p13, p14, p15, p16, p17, p18, $
                    format=format, unit=unit0, _EXTRA=extra

  COMPILE_OPT strictarr
  if n_elements(unit0) EQ 0 then unit = -1 else unit = round(unit0[0])
  if n_params() EQ 0 then printf, unit, '' $
  else if n_params() EQ 1 then printf, unit, p1, format=format $
  else if n_params() EQ 2 then printf, unit, p1, p2, format=format $
  else if n_params() EQ 3 then printf, unit, p1, p2, p3, format=format $
  else if n_params() EQ 4 then printf, unit, p1, p2, p4, format=format 

  return
end


;; Default procedure to be called every iteration.  It simply prints
;; the parameter values.
pro mpfit_defiter, fcn, x, iter, fnorm, FUNCTARGS=fcnargs, $
                   quiet=quiet, iterstop=iterstop, iterkeybyte=iterkeybyte, $
                   parinfo=parinfo, iterprint=iterprint0, $
                   format=fmt, pformat=pformat, dof=dof0, $
                   iterparnameformat=iterparnameformat0, $
                   _EXTRA=iterargs

  COMPILE_OPT strictarr
  common mpfit_error, mperr
  mperr = 0

  if keyword_set(quiet) then goto, DO_ITERSTOP
  if n_params() EQ 3 then begin
      fvec = mpfit_call(fcn, x, _EXTRA=fcnargs)
      fnorm = mpfit_enorm(fvec)^2
  endif

  ;; Determine which parameters to print
  nprint = n_elements(x)
  iprint = lindgen(nprint)

  if n_elements(iterprint0) EQ 0 then iterprint = 'MPFIT_DEFPRINT' $
  else iterprint = strtrim(iterprint0[0],2)
  if n_elements(iterparnameformat0) EQ 0 then iterparnameformat = 'A25' $
  else iterparnameformat = iterparnameformat0

  if n_elements(dof0) EQ 0 then dof = 1L else dof = floor(dof0[0])
  call_procedure, iterprint, iter, fnorm, dof, $
    format='("Iter ",I6,"   CHI-SQUARE = ",G15.8,"          DOF = ",I0)', $
    _EXTRA=iterargs
  if n_elements(fmt) GT 0 then begin
      call_procedure, iterprint, x, format=fmt, _EXTRA=iterargs
  endif else begin
      if n_elements(pformat) EQ 0 then pformat = '(G20.6)'
      parname = 'P('+strtrim(iprint,2)+')'
      parnamefmt = strarr(nprint) + iterparnameformat
      pformats = strarr(nprint) + pformat
      pardesc = strarr(nprint)

      if n_elements(parinfo) GT 0 then begin
          parinfo_tags = tag_names(parinfo)
          wh = where(parinfo_tags EQ 'PARNAME', ct)
          if ct EQ 1 then begin
              wh = where(parinfo.parname NE '', ct)
              if ct GT 0 then $
                parname[wh] = parinfo[wh].parname
          endif
          wh = where(parinfo_tags EQ 'MPPRINT', ct)
          if ct EQ 1 then begin
              iprint = where(parinfo.mpprint EQ 1, nprint)
              if nprint EQ 0 then goto, DO_ITERSTOP
          endif
          wh = where(parinfo_tags EQ 'MPFORMAT', ct)
          if ct EQ 1 then begin
              wh = where(parinfo.mpformat NE '', ct)
              if ct GT 0 then pformats[wh] = parinfo[wh].mpformat
          endif
          wh = where(parinfo_tags EQ 'MPFORMAT_PARNAME', ct)
          if ct EQ 1 then begin
              wh = where(parinfo.mpformat_parname NE '', ct)
              if ct GT 0 then parnamefmt[wh] = parinfo[wh].mpformat_parname
          endif
      endif

      for i = 0L, nprint-1 do begin
          call_procedure, iterprint, parname[iprint[i]], x[iprint[i]], $
            format='("    ",'+parnamefmt[i]+'," = ",'+pformats[iprint[i]]+')', $
            _EXTRA=iterargs
      endfor
  endelse

  DO_ITERSTOP:
  if n_elements(iterkeybyte) EQ 0 then iterkeybyte = 7b
  if keyword_set(iterstop) then begin
      k = get_kbrd(0)
      if k EQ string(iterkeybyte[0]) then begin
          message, 'WARNING: minimization not complete', /info
          print, 'Do you want to terminate this procedure? (y/n)', $
            format='(A,$)'
          k = ''
          read, k
          if strupcase(strmid(k,0,1)) EQ 'Y' then begin
              message, 'WARNING: Procedure is terminating.', /info
              mperr = -1
          endif
      endif
  endif

  return
end

;; Procedure to parse the parameter values in PARINFO
pro mpfit_parinfo, parinfo, tnames, tag, values, default=def, status=status, $
                   n_param=n

  COMPILE_OPT strictarr
  status = 0
  if n_elements(n) EQ 0 then n = n_elements(parinfo)

  if n EQ 0 then begin
      if n_elements(def) EQ 0 then return
      values = def
      status = 1
      return
  endif

  if n_elements(parinfo) EQ 0 then goto, DO_DEFAULT
  if n_elements(tnames) EQ 0 then tnames = tag_names(parinfo)
  wh = where(tnames EQ tag, ct)

  if ct EQ 0 then begin
      DO_DEFAULT:
      if n_elements(def) EQ 0 then return
      values = make_array(n, value=def[0])
      values[0] = def
  endif else begin
      values = parinfo.(wh[0])
      np = n_elements(parinfo)
      nv = n_elements(values)
      values = reform(values[*], nv/np, np)
  endelse

  status = 1
  return
end


;     **********
;
;     subroutine covar
;
;     given an m by n matrix a, the problem is to determine
;     the covariance matrix corresponding to a, defined as
;
;                    t
;           inverse(a *a) .
;
;     this subroutine completes the solution of the problem
;     if it is provided with the necessary information from the
;     qr factorization, with column pivoting, of a. that is, if
;     a*p = q*r, where p is a permutation matrix, q has orthogonal
;     columns, and r is an upper triangular matrix with diagonal
;     elements of nonincreasing magnitude, then covar expects
;     the full upper triangle of r and the permutation matrix p.
;     the covariance matrix is then computed as
;
;                      t     t
;           p*inverse(r *r)*p  .
;
;     if a is nearly rank deficient, it may be desirable to compute
;     the covariance matrix corresponding to the linearly independent
;     columns of a. to define the numerical rank of a, covar uses
;     the tolerance tol. if l is the largest integer such that
;
;           abs(r(l,l)) .gt. tol*abs(r(1,1)) ,
;
;     then covar computes the covariance matrix corresponding to
;     the first l columns of r. for k greater than l, column
;     and row ipvt(k) of the covariance matrix are set to zero.
;
;     the subroutine statement is
;
;       subroutine covar(n,r,ldr,ipvt,tol,wa)
;
;     where
;
;       n is a positive integer input variable set to the order of r.
;
;       r is an n by n array. on input the full upper triangle must
;         contain the full upper triangle of the matrix r. on output
;         r contains the square symmetric covariance matrix.
;
;       ldr is a positive integer input variable not less than n
;         which specifies the leading dimension of the array r.
;
;       ipvt is an integer input array of length n which defines the
;         permutation matrix p such that a*p = q*r. column j of p
;         is column ipvt(j) of the identity matrix.
;
;       tol is a nonnegative input variable used to define the
;         numerical rank of a in the manner described above.
;
;       wa is a work array of length n.
;
;     subprograms called
;
;       fortran-supplied ... dabs
;
;     argonne national laboratory. minpack project. august 1980.
;     burton s. garbow, kenneth e. hillstrom, jorge j. more
;
;     **********
function mpfit_covar, rr, ipvt, tol=tol

  COMPILE_OPT strictarr
  sz = size(rr)
  if sz[0] NE 2 then begin
      message, 'ERROR: r must be a two-dimensional matrix'
      return, -1L
  endif
  n = sz[1]
  if n NE sz[2] then begin
      message, 'ERROR: r must be a square matrix'
      return, -1L
  endif

  zero = rr[0] * 0.
  one  = zero  + 1.
  if n_elements(ipvt) EQ 0 then ipvt = lindgen(n)
  r = rr
  r = reform(rr, n, n, /overwrite)
  
  ;; Form the inverse of r in the full upper triangle of r
  l = -1L
  if n_elements(tol) EQ 0 then tol = one*1.E-14
  tolr = tol * abs(r[0,0])
  for k = 0L, n-1 do begin
      if abs(r[k,k]) LE tolr then goto, INV_END_LOOP
      r[k,k] = one/r[k,k]
      for j = 0L, k-1 do begin
          temp = r[k,k] * r[j,k]
          r[j,k] = zero
          r[0,k] = r[0:j,k] - temp*r[0:j,j]
      endfor
      l = k
  endfor
  INV_END_LOOP:

  ;; Form the full upper triangle of the inverse of (r transpose)*r
  ;; in the full upper triangle of r
  if l GE 0 then $
    for k = 0L, l do begin
      for j = 0L, k-1 do begin
          temp = r[j,k]
          r[0,j] = r[0:j,j] + temp*r[0:j,k]
      endfor
      temp = r[k,k]
      r[0,k] = temp * r[0:k,k]
  endfor

  ;; Form the full lower triangle of the covariance matrix
  ;; in the strict lower triangle of r and in wa
  wa = replicate(r[0,0], n)
  for j = 0L, n-1 do begin
      jj = ipvt[j]
      sing = j GT l
      for i = 0L, j do begin
          if sing then r[i,j] = zero
          ii = ipvt[i]
          if ii GT jj then r[ii,jj] = r[i,j]
          if ii LT jj then r[jj,ii] = r[i,j]
      endfor
      wa[jj] = r[j,j]
  endfor

  ;; Symmetrize the covariance matrix in r
  for j = 0L, n-1 do begin
      r[0:j,j] = r[j,0:j]
      r[j,j] = wa[j]
  endfor

  return, r
end

;; Parse the RCSID revision number
function mpfit_revision
  ;; NOTE: this string is changed every time an RCS check-in occurs
  revision = '$Revision: 1.85 $'

  ;; Parse just the version number portion
  revision = stregex(revision,'\$'+'Revision: *([0-9.]+) *'+'\$',/extract,/sub)
  revision = revision[1]
  return, revision
end

;; Parse version numbers of the form 'X.Y'
function mpfit_parse_version, version
  sz = size(version)
  if sz[sz[0]+1] NE 7 then return, 0

  s = stregex(version[0], '^([0-9]+)\.([0-9]+)$', /extract,/sub) 
  if s[0] NE version[0] then return, 0
  return, long(s[1:2])
end

;; Enforce a minimum version number
function mpfit_min_version, version, min_version
  mv = mpfit_parse_version(min_version)
  if mv[0] EQ 0 then return, 0
  v  = mpfit_parse_version(version)

  ;; Compare version components
  if v[0] LT mv[0] then return, 0
  if v[1] LT mv[1] then return, 0
  return, 1
end

; Manually reset recursion fencepost if the user gets in trouble
pro mpfit_reset_recursion
  common mpfit_fencepost, mpfit_fencepost_active
  mpfit_fencepost_active = 0
end

;     **********
;
;     subroutine lmdif
;
;     the purpose of lmdif is to minimize the sum of the squares of
;     m nonlinear functions in n variables by a modification of
;     the levenberg-marquardt algorithm. the user must provide a
;     subroutine which calculates the functions. the jacobian is
;     then calculated by a forward-difference approximation.
;
;     the subroutine statement is
;
;	subroutine lmdif(fcn,m,n,x,fvec,ftol,xtol,gtol,maxfev,epsfcn,
;			 diag,mode,factor,nprint,info,nfev,fjac,
;			 ldfjac,ipvt,qtf,wa1,wa2,wa3,wa4)
;
;     where
;
;	fcn is the name of the user-supplied subroutine which
;	  calculates the functions. fcn must be declared
;	  in an external statement in the user calling
;	  program, and should be written as follows.
;
;	  subroutine fcn(m,n,x,fvec,iflag)
;	  integer m,n,iflag
;	  double precision x(n),fvec(m)
;	  ----------
;	  calculate the functions at x and
;	  return this vector in fvec.
;	  ----------
;	  return
;	  end
;
;	  the value of iflag should not be changed by fcn unless
;	  the user wants to terminate execution of lmdif.
;	  in this case set iflag to a negative integer.
;
;	m is a positive integer input variable set to the number
;	  of functions.
;
;	n is a positive integer input variable set to the number
;	  of variables. n must not exceed m.
;
;	x is an array of length n. on input x must contain
;	  an initial estimate of the solution vector. on output x
;	  contains the final estimate of the solution vector.
;
;	fvec is an output array of length m which contains
;	  the functions evaluated at the output x.
;
;	ftol is a nonnegative input variable. termination
;	  occurs when both the actual and predicted relative
;	  reductions in the sum of squares are at most ftol.
;	  therefore, ftol measures the relative error desired
;	  in the sum of squares.
;
;	xtol is a nonnegative input variable. termination
;	  occurs when the relative error between two consecutive
;	  iterates is at most xtol. therefore, xtol measures the
;	  relative error desired in the approximate solution.
;
;	gtol is a nonnegative input variable. termination
;	  occurs when the cosine of the angle between fvec and
;	  any column of the jacobian is at most gtol in absolute
;	  value. therefore, gtol measures the orthogonality
;	  desired between the function vector and the columns
;	  of the jacobian.
;
;	maxfev is a positive integer input variable. termination
;	  occurs when the number of calls to fcn is at least
;	  maxfev by the end of an iteration.
;
;	epsfcn is an input variable used in determining a suitable
;	  step length for the forward-difference approximation. this
;	  approximation assumes that the relative errors in the
;	  functions are of the order of epsfcn. if epsfcn is less
;	  than the machine precision, it is assumed that the relative
;	  errors in the functions are of the order of the machine
;	  precision.
;
;	diag is an array of length n. if mode = 1 (see
;	  below), diag is internally set. if mode = 2, diag
;	  must contain positive entries that serve as
;	  multiplicative scale factors for the variables.
;
;	mode is an integer input variable. if mode = 1, the
;	  variables will be scaled internally. if mode = 2,
;	  the scaling is specified by the input diag. other
;	  values of mode are equivalent to mode = 1.
;
;	factor is a positive input variable used in determining the
;	  initial step bound. this bound is set to the product of
;	  factor and the euclidean norm of diag*x if nonzero, or else
;	  to factor itself. in most cases factor should lie in the
;	  interval (.1,100.). 100. is a generally recommended value.
;
;	nprint is an integer input variable that enables controlled
;	  printing of iterates if it is positive. in this case,
;	  fcn is called with iflag = 0 at the beginning of the first
;	  iteration and every nprint iterations thereafter and
;	  immediately prior to return, with x and fvec available
;	  for printing. if nprint is not positive, no special calls
;	  of fcn with iflag = 0 are made.
;
;	info is an integer output variable. if the user has
;	  terminated execution, info is set to the (negative)
;	  value of iflag. see description of fcn. otherwise,
;	  info is set as follows.
;
;	  info = 0  improper input parameters.
;
;	  info = 1  both actual and predicted relative reductions
;		    in the sum of squares are at most ftol.
;
;	  info = 2  relative error between two consecutive iterates
;		    is at most xtol.
;
;	  info = 3  conditions for info = 1 and info = 2 both hold.
;
;	  info = 4  the cosine of the angle between fvec and any
;		    column of the jacobian is at most gtol in
;		    absolute value.
;
;	  info = 5  number of calls to fcn has reached or
;		    exceeded maxfev.
;
;	  info = 6  ftol is too small. no further reduction in
;		    the sum of squares is possible.
;
;	  info = 7  xtol is too small. no further improvement in
;		    the approximate solution x is possible.
;
;	  info = 8  gtol is too small. fvec is orthogonal to the
;		    columns of the jacobian to machine precision.
;
;	nfev is an integer output variable set to the number of
;	  calls to fcn.
;
;	fjac is an output m by n array. the upper n by n submatrix
;	  of fjac contains an upper triangular matrix r with
;	  diagonal elements of nonincreasing magnitude such that
;
;		 t     t	   t
;		p *(jac *jac)*p = r *r,
;
;	  where p is a permutation matrix and jac is the final
;	  calculated jacobian. column j of p is column ipvt(j)
;	  (see below) of the identity matrix. the lower trapezoidal
;	  part of fjac contains information generated during
;	  the computation of r.
;
;	ldfjac is a positive integer input variable not less than m
;	  which specifies the leading dimension of the array fjac.
;
;	ipvt is an integer output array of length n. ipvt
;	  defines a permutation matrix p such that jac*p = q*r,
;	  where jac is the final calculated jacobian, q is
;	  orthogonal (not stored), and r is upper triangular
;	  with diagonal elements of nonincreasing magnitude.
;	  column j of p is column ipvt(j) of the identity matrix.
;
;	qtf is an output array of length n which contains
;	  the first n elements of the vector (q transpose)*fvec.
;
;	wa1, wa2, and wa3 are work arrays of length n.
;
;	wa4 is a work array of length m.
;
;     subprograms called
;
;	user-supplied ...... fcn
;
;	minpack-supplied ... dpmpar,enorm,fdjac2,lmpar,qrfac
;
;	fortran-supplied ... dabs,dmax1,dmin1,dsqrt,mod
;
;     argonne national laboratory. minpack project. march 1980.
;     burton s. garbow, kenneth e. hillstrom, jorge j. more
;
;     **********
function mpfit, fcn, xall, FUNCTARGS=fcnargs, SCALE_FCN=scalfcn, $
                ftol=ftol0, xtol=xtol0, gtol=gtol0, epsfcn=epsfcn, $
                resdamp=damp0, $
                nfev=nfev, maxiter=maxiter, errmsg=errmsg, $
                factor=factor0, nprint=nprint0, STATUS=info, $
                iterproc=iterproc0, iterargs=iterargs, iterstop=ss,$
                iterkeystop=iterkeystop, $
                niter=iter, nfree=nfree, npegged=npegged, dof=dof, $
                diag=diag, rescale=rescale, autoderivative=autoderiv0, $
                pfree_index=ifree, $
                perror=perror, covar=covar, nocovar=nocovar, $
                bestnorm=fnorm, best_resid=fvec, $
                best_fjac=output_fjac, calc_fjac=calc_fjac, $
                parinfo=parinfo, quiet=quiet, nocatch=nocatch, $
                fastnorm=fastnorm0, proc=proc, query=query, $
                external_state=state, external_init=extinit, $
                external_fvec=efvec, external_fjac=efjac, $
                version=version, min_version=min_version0

  COMPILE_OPT strictarr
  info = 0L
  errmsg = ''

  ;; Compute the revision number, to be returned in the VERSION and
  ;; QUERY keywords.
  common mpfit_revision_common, mpfit_revision_str
  if n_elements(mpfit_revision_str) EQ 0 then $
     mpfit_revision_str = mpfit_revision()
  version = mpfit_revision_str

  if keyword_set(query) then begin
     if n_elements(min_version0) GT 0 then $
        if mpfit_min_version(version, min_version0[0]) EQ 0 then $
           return, 0
     return, 1
  endif

  if n_elements(min_version0) GT 0 then $
     if mpfit_min_version(version, min_version0[0]) EQ 0 then begin
     message, 'ERROR: minimum required version '+min_version0[0]+' not satisfied', /info
     return, !values.d_nan
  endif

  if n_params() EQ 0 then begin
      message, "USAGE: PARMS = MPFIT('MYFUNCT', START_PARAMS, ... )", /info
      return, !values.d_nan
  endif
  
  ;; Use of double here not a problem since f/x/gtol are all only used
  ;; in comparisons
  if n_elements(ftol0) EQ 0 then ftol = 1.D-10 else ftol = ftol0[0]
  if n_elements(xtol0) EQ 0 then xtol = 1.D-10 else xtol = xtol0[0]
  if n_elements(gtol0) EQ 0 then gtol = 1.D-10 else gtol = gtol0[0]
  if n_elements(factor0) EQ 0 then factor = 100. else factor = factor0[0]
  if n_elements(nprint0) EQ 0 then nprint = 1 else nprint = nprint0[0]
  if n_elements(iterproc0) EQ 0 then iterproc = 'MPFIT_DEFITER' else iterproc = iterproc0[0]
  if n_elements(autoderiv0) EQ 0 then autoderiv = 1 else autoderiv = autoderiv0[0]
  if n_elements(fastnorm0) EQ 0 then fastnorm = 0 else fastnorm = fastnorm0[0]
  if n_elements(damp0) EQ 0 then damp = 0 else damp = damp0[0]

  ;; These are special configuration parameters that can't be easily
  ;; passed by MPFIT directly.
  ;;  FASTNORM - decide on which sum-of-squares technique to use (1)
  ;;             is fast, (0) is slower
  ;;  PROC - user routine is a procedure (1) or function (0)
  ;;  QANYTIED - set to 1 if any parameters are TIED, zero if none
  ;;  PTIED - array of strings, one for each parameter
  common mpfit_config, mpconfig
  mpconfig = {fastnorm: keyword_set(fastnorm), proc: 0, nfev: 0L, damp: damp}
  common mpfit_machar, machvals

  iflag = 0L
  catch_msg = 'in MPFIT'
  nfree = 0L
  npegged = 0L
  dof = 0L
  output_fjac = 0L

  ;; Set up a persistent fencepost that prevents recursive calls
  common mpfit_fencepost, mpfit_fencepost_active
  if n_elements(mpfit_fencepost_active) EQ 0 then mpfit_fencepost_active = 0
  if mpfit_fencepost_active then begin
      errmsg = 'ERROR: recursion detected; you cannot run MPFIT recursively'
      goto, TERMINATE
  endif
  ;; Only activate the fencepost if we are not in debugging mode
  if NOT keyword_set(nocatch) then mpfit_fencepost_active = 1


  ;; Parameter damping doesn't work when user is providing their own
  ;; gradients.
  if damp NE 0 AND NOT keyword_set(autoderiv) then begin
      errmsg = 'ERROR: keywords DAMP and AUTODERIV are mutually exclusive'
      goto, TERMINATE
  endif      
  
  ;; Process the ITERSTOP and ITERKEYSTOP keywords, and turn this into
  ;; a set of keywords to pass to MPFIT_DEFITER.
  if strupcase(iterproc) EQ 'MPFIT_DEFITER' AND n_elements(iterargs) EQ 0 $
    AND keyword_set(ss) then begin
      if n_elements(iterkeystop) GT 0 then begin
          sz = size(iterkeystop)
          tp = sz[sz[0]+1]
          if tp EQ 7 then begin
              ;; String - convert first char to byte
              iterkeybyte = (byte(iterkeystop[0]))[0]
          endif
          if (tp GE 1 AND tp LE 3) OR (tp GE 12 AND tp LE 15) then begin
              ;; Integer - convert to byte
              iterkeybyte = byte(iterkeystop[0])
          endif
          if n_elements(iterkeybyte) EQ 0 then begin
              errmsg = 'ERROR: ITERKEYSTOP must be either a BYTE or STRING'
              goto, TERMINATE
          endif

          iterargs = {iterstop: 1, iterkeybyte: iterkeybyte}
      endif else begin
          iterargs = {iterstop: 1, iterkeybyte: 7b}
      endelse
  endif


  ;; Handle error conditions gracefully
  if NOT keyword_set(nocatch) then begin
      catch, catcherror
      if catcherror NE 0 then begin  ;; An error occurred!!!
          catch, /cancel
          mpfit_fencepost_active = 0
          err_string = ''+!error_state.msg
          message, /cont, 'Error detected while '+catch_msg+':'
          message, /cont,    err_string
          message, /cont, 'Error condition detected. Returning to MAIN level.'
          if errmsg EQ '' then $
            errmsg = 'Error detected while '+catch_msg+': '+err_string
          if info EQ 0 then info = -18
          return, !values.d_nan
      endif
  endif
  mpconfig = create_struct(mpconfig, 'NOCATCH', keyword_set(nocatch))

  ;; Parse FCN function name - be sure it is a scalar string
  sz = size(fcn)
  if sz[0] NE 0 then begin
      FCN_NAME:
      errmsg = 'ERROR: MYFUNCT must be a scalar string'
      goto, TERMINATE
  endif
  if sz[sz[0]+1] NE 7 then goto, FCN_NAME

  isext = 0
  if fcn EQ '(EXTERNAL)' then begin
      if n_elements(efvec) EQ 0 OR n_elements(efjac) EQ 0 then begin
          errmsg = 'ERROR: when using EXTERNAL function, EXTERNAL_FVEC '+$
            'and EXTERNAL_FJAC must be defined'
          goto, TERMINATE
      endif
      nv = n_elements(efvec)
      nj = n_elements(efjac)
      if (nj MOD nv) NE 0 then begin
          errmsg = 'ERROR: the number of values in EXTERNAL_FJAC must be '+ $
            'a multiple of the number of values in EXTERNAL_FVEC'
          goto, TERMINATE
      endif
      isext = 1
  endif

  ;; Parinfo:
  ;; --------------- STARTING/CONFIG INFO (passed in to routine, not changed)
  ;; .value   - starting value for parameter
  ;; .fixed   - parameter is fixed
  ;; .limited - a two-element array, if parameter is bounded on
  ;;            lower/upper side
  ;; .limits - a two-element array, lower/upper parameter bounds, if
  ;;           limited vale is set
  ;; .step   - step size in Jacobian calc, if greater than zero

  catch_msg = 'parsing input parameters'
  ;; Parameters can either be stored in parinfo, or x.  Parinfo takes
  ;; precedence if it exists.
  if n_elements(xall) EQ 0 AND n_elements(parinfo) EQ 0 then begin
      errmsg = 'ERROR: must pass parameters in P or PARINFO'
      goto, TERMINATE
  endif

  ;; Be sure that PARINFO is of the right type
  if n_elements(parinfo) GT 0 then begin
      ;; Make sure the array is 1-D
      parinfo = parinfo[*]
      parinfo_size = size(parinfo)
      if parinfo_size[parinfo_size[0]+1] NE 8 then begin
          errmsg = 'ERROR: PARINFO must be a structure.'
          goto, TERMINATE
      endif
      if n_elements(xall) GT 0 AND n_elements(xall) NE n_elements(parinfo) $
        then begin
          errmsg = 'ERROR: number of elements in PARINFO and P must agree'
          goto, TERMINATE
      endif
  endif

  ;; If the parameters were not specified at the command line, then
  ;; extract them from PARINFO
  if n_elements(xall) EQ 0 then begin
      mpfit_parinfo, parinfo, tagnames, 'VALUE', xall, status=status
      if status EQ 0 then begin
          errmsg = 'ERROR: either P or PARINFO[*].VALUE must be supplied.'
          goto, TERMINATE
      endif

      sz = size(xall)
      ;; Convert to double if parameters are not float or double
      if sz[sz[0]+1] NE 4 AND sz[sz[0]+1] NE 5 then $
        xall = double(xall)
  endif
  xall = xall[*]   ;; Make sure the array is 1-D
  npar = n_elements(xall)
  zero = xall[0] * 0.
  one  = zero    + 1.
  fnorm  = -one
  fnorm1 = -one

  ;; TIED parameters?
  mpfit_parinfo, parinfo, tagnames, 'TIED', ptied, default='', n=npar
  ptied = strtrim(ptied, 2)
  wh = where(ptied NE '', qanytied) 
  qanytied = qanytied GT 0
  mpconfig = create_struct(mpconfig, 'QANYTIED', qanytied, 'PTIED', ptied)

  ;; FIXED parameters ?
  mpfit_parinfo, parinfo, tagnames, 'FIXED', pfixed, default=0, n=npar
  pfixed = pfixed EQ 1
  pfixed = pfixed OR (ptied NE '');; Tied parameters are also effectively fixed
  
  ;; Finite differencing step, absolute and relative, and sidedness of deriv.
  mpfit_parinfo, parinfo, tagnames, 'STEP',     step, default=zero, n=npar
  mpfit_parinfo, parinfo, tagnames, 'RELSTEP', dstep, default=zero, n=npar
  mpfit_parinfo, parinfo, tagnames, 'MPSIDE',  dside, default=0,    n=npar
  ;; Debugging parameters
  mpfit_parinfo, parinfo, tagnames, 'MPDERIV_DEBUG',  ddebug, default=0, n=npar
  mpfit_parinfo, parinfo, tagnames, 'MPDERIV_RELTOL', ddrtol, default=zero, n=npar
  mpfit_parinfo, parinfo, tagnames, 'MPDERIV_ABSTOL', ddatol, default=zero, n=npar

  ;; Maximum and minimum steps allowed to be taken in one iteration
  mpfit_parinfo, parinfo, tagnames, 'MPMAXSTEP', maxstep, default=zero, n=npar
  mpfit_parinfo, parinfo, tagnames, 'MPMINSTEP', minstep, default=zero, n=npar
  qmin = minstep *  0  ;; Remove minstep for now!!
  qmax = maxstep NE 0
  wh = where(qmin AND qmax AND maxstep LT minstep, ct)
  if ct GT 0 then begin
      errmsg = 'ERROR: MPMINSTEP is greater than MPMAXSTEP'
      goto, TERMINATE
  endif

  ;; Finish up the free parameters
  ifree = where(pfixed NE 1, nfree)
  if nfree EQ 0 then begin
      errmsg = 'ERROR: no free parameters'
      goto, TERMINATE
  endif

  ;; An external Jacobian must be checked against the number of
  ;; parameters
  if isext then begin
      if (nj/nv) NE nfree then begin
          errmsg = string(nv, nfree, nfree, $
           format=('("ERROR: EXTERNAL_FJAC must be a ",I0," x ",I0,' + $
                   '" array, where ",I0," is the number of free parameters")'))
          goto, TERMINATE
      endif
  endif

  ;; Compose only VARYING parameters
  xnew = xall      ;; xnew is the set of parameters to be returned
  x = xnew[ifree]  ;; x is the set of free parameters
  ; Same for min/max step diagnostics
  qmin = qmin[ifree]  & minstep = minstep[ifree]
  qmax = qmax[ifree]  & maxstep = maxstep[ifree]
  wh = where(qmin OR qmax, ct)
  qminmax = ct GT 0


  ;; LIMITED parameters ?
  mpfit_parinfo, parinfo, tagnames, 'LIMITED', limited, status=st1
  mpfit_parinfo, parinfo, tagnames, 'LIMITS',  limits,  status=st2
  if st1 EQ 1 AND st2 EQ 1 then begin

      ;; Error checking on limits in parinfo
      wh = where((limited[0,*] AND xall LT limits[0,*]) OR $
                 (limited[1,*] AND xall GT limits[1,*]), ct)
      if ct GT 0 then begin
          errmsg = 'ERROR: parameters are not within PARINFO limits'
          goto, TERMINATE
      endif
      wh = where(limited[0,*] AND limited[1,*] AND $
                 limits[0,*] GE limits[1,*] AND $
                 pfixed EQ 0, ct)
      if ct GT 0 then begin
          errmsg = 'ERROR: PARINFO parameter limits are not consistent'
          goto, TERMINATE
      endif
      

      ;; Transfer structure values to local variables
      qulim = limited[1, ifree]
      ulim  = limits [1, ifree]
      qllim = limited[0, ifree]
      llim  = limits [0, ifree]

      wh = where(qulim OR qllim, ct)
      if ct GT 0 then qanylim = 1 else qanylim = 0

  endif else begin

      ;; Fill in local variables with dummy values
      qulim = lonarr(nfree)
      ulim  = x * 0.
      qllim = qulim
      llim  = x * 0.
      qanylim = 0

  endelse

  ;; Initialize the number of parameters pegged at a hard limit value
  wh = where((qulim AND (x EQ ulim)) OR (qllim AND (x EQ llim)), npegged)

  n = n_elements(x)
  if n_elements(maxiter) EQ 0 then maxiter = 200L

  ;; Check input parameters for errors
  if (n LE 0) OR (ftol LE 0) OR (xtol LE 0) OR (gtol LE 0) $
    OR (maxiter LT 0) OR (factor LE 0) then begin
      errmsg = 'ERROR: input keywords are inconsistent'
      goto, TERMINATE
  endif

  if keyword_set(rescale) then begin
      errmsg = 'ERROR: DIAG parameter scales are inconsistent'
      if n_elements(diag) LT n then goto, TERMINATE
      wh = where(diag LE 0, ct)
      if ct GT 0 then goto, TERMINATE
      errmsg = ''
  endif

  if n_elements(state) NE 0 AND NOT keyword_set(extinit) then begin
      szst = size(state)
      if szst[szst[0]+1] NE 8  then begin
          errmsg = 'EXTERNAL_STATE keyword was not preserved'
          status = 0
          goto, TERMINATE
      endif
      if nfree NE n_elements(state.ifree) then begin
          BAD_IFREE:
          errmsg = 'Number of free parameters must not change from one '+$
            'external iteration to the next'
          status = 0
          goto, TERMINATE
      endif
      wh = where(ifree NE state.ifree, ct)
      if ct GT 0 then goto, BAD_IFREE

      tnames = tag_names(state)
      for i = 0L, n_elements(tnames)-1 do begin
          dummy = execute(tnames[i]+' = state.'+tnames[i])
      endfor
      wa4 = reform(efvec, n_elements(efvec))

      goto, RESUME_FIT
  endif

  common mpfit_error, mperr

  if NOT isext then begin
      mperr = 0
      catch_msg = 'calling '+fcn
      fvec = mpfit_call(fcn, xnew, _EXTRA=fcnargs)
      iflag = mperr
      if iflag LT 0 then begin
          errmsg = 'ERROR: first call to "'+fcn+'" failed'
          goto, TERMINATE
      endif
  endif else begin
      fvec = reform(efvec, n_elements(efvec))
  endelse

  catch_msg = 'calling MPFIT_SETMACHAR'
  sz = size(fvec[0])
  isdouble = (sz[sz[0]+1] EQ 5)
  
  mpfit_setmachar, double=isdouble

  common mpfit_profile, profvals
;  prof_start = systime(1)

  MACHEP0 = machvals.machep
  DWARF   = machvals.minnum

  szx = size(x)
  ;; The parameters and the squared deviations should have the same
  ;; type.  Otherwise the MACHAR-based evaluation will fail.
  catch_msg = 'checking parameter data'
  tp = szx[szx[0]+1]
  if tp NE 4 AND tp NE 5 then begin
      if NOT keyword_set(quiet) then begin
          message, 'WARNING: input parameters must be at least FLOAT', /info
          message, '         (converting parameters to FLOAT)', /info
      endif
      x = float(x)
      xnew = float(x)
      szx = size(x)
  endif
  if isdouble AND tp NE 5 then begin
      if NOT keyword_set(quiet) then begin
          message, 'WARNING: data is DOUBLE but parameters are FLOAT', /info
          message, '         (converting parameters to DOUBLE)', /info
      endif
      x = double(x)
      xnew = double(xnew)
  endif

  m = n_elements(fvec)
  if (m LT n) then begin
      errmsg = 'ERROR: number of parameters must not exceed data'
      goto, TERMINATE
  endif

  fnorm = mpfit_enorm(fvec)
  if finite(fnorm) EQ 0 then goto, FAIL_OVERFLOW

  ;; Initialize Levelberg-Marquardt parameter and iteration counter

  par = zero
  iter = 1L
  qtf = x * 0.

  ;; Beginning of the outer loop
  
  OUTER_LOOP:

  ;; If requested, call fcn to enable printing of iterates
  xnew[ifree] = x
  if qanytied then mpfit_tie, xnew, ptied
  dof = (n_elements(fvec) - nfree) > 1L

  if nprint GT 0 AND iterproc NE '' then begin
      catch_msg = 'calling '+iterproc
      iflag = 0L
      if (iter-1) MOD nprint EQ 0 then begin
          mperr = 0
          xnew0 = xnew

          call_procedure, iterproc, fcn, xnew, iter, fnorm^2, $
            FUNCTARGS=fcnargs, parinfo=parinfo, quiet=quiet, $
            dof=dof, _EXTRA=iterargs
          iflag = mperr

          ;; Check for user termination
          if iflag LT 0 then begin  
              errmsg = 'WARNING: premature termination by "'+iterproc+'"'
              goto, TERMINATE
          endif

          ;; If parameters were changed (grrr..) then re-tie
          if max(abs(xnew0-xnew)) GT 0 then begin
              if qanytied then mpfit_tie, xnew, ptied
              x = xnew[ifree]
          endif

      endif
  endif

  ;; Calculate the jacobian matrix
  iflag = 2
  if NOT isext then begin
      catch_msg = 'calling MPFIT_FDJAC2'
      ;; NOTE!  If you change this call then change the one during
      ;; clean-up as well!
      fjac = mpfit_fdjac2(fcn, x, fvec, step, qulim, ulim, dside, $
                          iflag=iflag, epsfcn=epsfcn, $
                          autoderiv=autoderiv, dstep=dstep, $
                          FUNCTARGS=fcnargs, ifree=ifree, xall=xnew, $
                          deriv_debug=ddebug, deriv_reltol=ddrtol, deriv_abstol=ddatol)
      if iflag LT 0 then begin
          errmsg = 'WARNING: premature termination by FDJAC2'
          goto, TERMINATE
      endif
  endif else begin
      fjac = reform(efjac,n_elements(fvec),npar, /overwrite)
  endelse

  ;; Rescale the residuals and gradient, for use with "alternative"
  ;; statistics such as the Cash statistic.
  catch_msg = 'prescaling residuals and gradient'
  if n_elements(scalfcn) GT 0 then begin
      call_procedure, strtrim(scalfcn[0],2), fvec, fjac
  endif

  ;; Determine if any of the parameters are pegged at the limits
  npegged = 0L
  if qanylim then begin
      catch_msg = 'zeroing derivatives of pegged parameters'
      whlpeg = where(qllim AND (x EQ llim), nlpeg)
      whupeg = where(qulim AND (x EQ ulim), nupeg)
      npegged = nlpeg + nupeg
      
      ;; See if any "pegged" values should keep their derivatives
      if (nlpeg GT 0) then begin
          ;; Total derivative of sum wrt lower pegged parameters
          ;;   Note: total(fvec*fjac[*,i]) is d(CHI^2)/dX[i]
          for i = 0L, nlpeg-1 do begin
              sum = total(fvec * fjac[*,whlpeg[i]])
              if sum GT 0 then fjac[*,whlpeg[i]] = 0
          endfor
      endif
      if (nupeg GT 0) then begin
          ;; Total derivative of sum wrt upper pegged parameters
          for i = 0L, nupeg-1 do begin
              sum = total(fvec * fjac[*,whupeg[i]])
              if sum LT 0 then fjac[*,whupeg[i]] = 0
          endfor
      endif
  endif

  ;; Save a copy of the Jacobian if the user requests it...
  if keyword_set(calc_fjac) then output_fjac = fjac

  ;; =====================
  ;; Compute the QR factorization of the jacobian
  catch_msg = 'calling MPFIT_QRFAC'
  ;;  IN:      Jacobian        
  ;; OUT:      Hh Vects  Permutation  RDIAG  ACNORM
  mpfit_qrfac, fjac,     ipvt,        wa1,   wa2, /pivot
  ;; Jacobian - jacobian matrix computed by mpfit_fdjac2
  ;; Hh vects - house holder vectors from QR factorization & R matrix
  ;; Permutation - permutation vector for pivoting
  ;; RDIAG - diagonal elements of R matrix
  ;; ACNORM - norms of input Jacobian matrix before factoring

  ;; =====================
  ;; On the first iteration if "diag" is unspecified, scale
  ;; according to the norms of the columns of the initial jacobian
  catch_msg = 'rescaling diagonal elements'
  if (iter EQ 1) then begin
      ;; Input: WA2 = root sum of squares of original Jacobian matrix
      ;;        DIAG = user-requested diagonal (not documented!)
      ;;        FACTOR = user-requested norm factor (not documented!)
      ;; Output: DIAG = Diagonal scaling values
      ;;         XNORM = sum of squared scaled residuals
      ;;         DELTA = rescaled XNORM

      if NOT keyword_set(rescale) OR (n_elements(diag) LT n) then begin
          diag = wa2 ;; Calculated from original Jacobian
          wh = where (diag EQ 0, ct) ;; Handle zero values
          if ct GT 0 then diag[wh] = one
      endif
      
      ;; On the first iteration, calculate the norm of the scaled x
      ;; and initialize the step bound delta 
      wa3 = diag * x           ;; WA3 is temp variable
      xnorm = mpfit_enorm(wa3)
      delta = factor*xnorm
      if delta EQ zero then delta = zero + factor
  endif

  ;; Form (q transpose)*fvec and store the first n components in qtf
  catch_msg = 'forming (q transpose)*fvec'
  wa4 = fvec
  for j=0L, n-1 do begin
      lj = ipvt[j]
      temp3 = fjac[j,lj]
      if temp3 NE 0 then begin
          fj = fjac[j:*,lj]
          wj = wa4[j:*]
          ;; *** optimization wa4(j:*)
          wa4[j] = wj - fj * total(fj*wj) / temp3  
      endif
      fjac[j,lj] = wa1[j]
      qtf[j] = wa4[j]
  endfor
  ;; From this point on, only the square matrix, consisting of the
  ;; triangle of R, is needed.
  fjac = fjac[0:n-1, 0:n-1]
  fjac = reform(fjac, n, n, /overwrite)
  fjac = fjac[*, ipvt]                    ;; Convert to permuted order
  fjac = reform(fjac, n, n, /overwrite)

  ;; Check for overflow.  This should be a cheap test here since FJAC
  ;; has been reduced to a (small) square matrix, and the test is
  ;; O(N^2).
  wh = where(finite(fjac) EQ 0, ct)
  if ct GT 0 then goto, FAIL_OVERFLOW

  ;; Compute the norm of the scaled gradient
  catch_msg = 'computing the scaled gradient'
  gnorm = zero
  if fnorm NE 0 then begin
      for j=0L, n-1 do begin
          l = ipvt[j]
          if wa2[l] NE 0 then begin
              sum = total(fjac[0:j,j]*qtf[0:j])/fnorm
              gnorm = max([gnorm,abs(sum/wa2[l])])
          endif
      endfor
  endif

  ;; Test for convergence of the gradient norm
  if gnorm LE gtol then info = 4
  if info NE 0 then goto, TERMINATE
  if maxiter EQ 0 then begin
     info = 5
     goto, TERMINATE
  endif

  ;; Rescale if necessary
  if NOT keyword_set(rescale) then $
    diag = diag > wa2

  ;; Beginning of the inner loop
  INNER_LOOP:
  
  ;; Determine the levenberg-marquardt parameter
  catch_msg = 'calculating LM parameter (MPFIT_LMPAR)'
  par = mpfit_lmpar(fjac, ipvt, diag, qtf, delta, wa1, wa2, par=par)

  ;; Store the direction p and x+p. Calculate the norm of p
  wa1 = -wa1

  if qanylim EQ 0 AND qminmax EQ 0 then begin
      ;; No parameter limits, so just move to new position WA2
      alpha = one
      wa2 = x + wa1

  endif else begin
      
      ;; Respect the limits.  If a step were to go out of bounds, then
      ;; we should take a step in the same direction but shorter distance.
      ;; The step should take us right to the limit in that case.
      alpha = one

      if qanylim EQ 1 then begin
          ;; Do not allow any steps out of bounds
          catch_msg = 'checking for a step out of bounds'
          if nlpeg GT 0 then wa1[whlpeg] = wa1[whlpeg] > 0
          if nupeg GT 0 then wa1[whupeg] = wa1[whupeg] < 0

          dwa1 = abs(wa1) GT MACHEP0
          whl = where(dwa1 AND qllim AND (x + wa1 LT llim), lct)
          if lct GT 0 then $
            alpha = min([alpha, (llim[whl]-x[whl])/wa1[whl]])
          whu = where(dwa1 AND qulim AND (x + wa1 GT ulim), uct)
          if uct GT 0 then $
            alpha = min([alpha, (ulim[whu]-x[whu])/wa1[whu]])
      endif

      ;; Obey any max step values.

      if qminmax EQ 1 then begin
          nwa1 = wa1 * alpha
          whmax = where(qmax AND maxstep GT 0, ct)
          if ct GT 0 then begin
              mrat = max(abs(nwa1[whmax])/abs(maxstep[whmax]))
              if mrat GT 1 then alpha = alpha / mrat
          endif
      endif          

      ;; Scale the resulting vector
      wa1 = wa1 * alpha
      wa2 = x + wa1

      ;; Adjust the final output values.  If the step put us exactly
      ;; on a boundary, make sure we peg it there.
      sgnu = (ulim GE 0)*2d - 1d
      sgnl = (llim GE 0)*2d - 1d

      ;; Handles case of 
      ;;      ... nonzero *LIM ...     ... zero *LIM ...
      ulim1 = ulim*(1-sgnu*MACHEP0) - (ulim EQ 0)*MACHEP0
      llim1 = llim*(1+sgnl*MACHEP0) + (llim EQ 0)*MACHEP0

      wh = where(qulim AND (wa2 GE ulim1), ct)
      if ct GT 0 then wa2[wh] = ulim[wh]

      wh = where(qllim AND (wa2 LE llim1), ct)
      if ct GT 0 then wa2[wh] = llim[wh]
  endelse

  wa3 = diag * wa1
  pnorm = mpfit_enorm(wa3)

  ;; On the first iteration, adjust the initial step bound
  if iter EQ 1 then delta = min([delta,pnorm])

  xnew[ifree] = wa2
  if isext then goto, SAVE_STATE

  ;; Evaluate the function at x+p and calculate its norm
  mperr = 0
  catch_msg = 'calling '+fcn
  wa4 = mpfit_call(fcn, xnew, _EXTRA=fcnargs)
  iflag = mperr
  if iflag LT 0 then begin
      errmsg = 'WARNING: premature termination by "'+fcn+'"'
      goto, TERMINATE
  endif
  RESUME_FIT:
  fnorm1 = mpfit_enorm(wa4)
  if finite(fnorm1) EQ 0 then goto, FAIL_OVERFLOW
  
  ;; Compute the scaled actual reduction
  catch_msg = 'computing convergence criteria'
  actred = -one
  if 0.1D * fnorm1 LT fnorm then actred = - (fnorm1/fnorm)^2 + 1.

  ;; Compute the scaled predicted reduction and the scaled directional
  ;; derivative
  for j = 0L, n-1 do begin
      wa3[j] = 0
      wa3[0:j] = wa3[0:j] + fjac[0:j,j]*wa1[ipvt[j]]
  endfor

  ;; Remember, alpha is the fraction of the full LM step actually
  ;; taken
  temp1 = mpfit_enorm(alpha*wa3)/fnorm
  temp2 = (sqrt(alpha*par)*pnorm)/fnorm
  half  = zero + 0.5
  prered = temp1*temp1 + (temp2*temp2)/half
  dirder = -(temp1*temp1 + temp2*temp2)

  ;; Compute the ratio of the actual to the predicted reduction.
  ratio = zero
  tenth = zero + 0.1
  if prered NE 0 then ratio = actred/prered

  ;; Update the step bound
  if ratio LE 0.25D then begin
      if actred GE 0 then temp = half $
      else temp = half*dirder/(dirder + half*actred)
      if ((0.1D*fnorm1) GE fnorm) OR (temp LT 0.1D) then temp = tenth
      delta = temp*min([delta,pnorm/tenth])
      par = par/temp
  endif else begin
      if (par EQ 0) OR (ratio GE 0.75) then begin
          delta = pnorm/half
          par = half*par
      endif
  endelse

  ;; Test for successful iteration
  if ratio GE 0.0001 then begin
      ;; Successful iteration.  Update x, fvec, and their norms
      x = wa2
      wa2 = diag * x

      fvec = wa4
      xnorm = mpfit_enorm(wa2)
      fnorm = fnorm1
      iter = iter + 1
  endif

  ;; Tests for convergence
  if (abs(actred) LE ftol) AND (prered LE ftol) $
    AND  (0.5D * ratio LE 1) then info = 1
  if delta LE xtol*xnorm then info = 2
  if (abs(actred) LE ftol) AND (prered LE ftol) $
    AND (0.5D * ratio LE 1) AND (info EQ 2) then info = 3
  if info NE 0 then goto, TERMINATE

  ;; Tests for termination and stringent tolerances
  if iter GE maxiter then info = 5
  if (abs(actred) LE MACHEP0) AND (prered LE MACHEP0) $
    AND (0.5*ratio LE 1) then info = 6
  if delta LE MACHEP0*xnorm then info = 7
  if gnorm LE MACHEP0 then info = 8
  if info NE 0 then goto, TERMINATE

  ;; End of inner loop. Repeat if iteration unsuccessful
  if ratio LT 0.0001 then begin
      goto, INNER_LOOP
  endif

  ;; Check for over/underflow
  wh = where(finite(wa1) EQ 0 OR finite(wa2) EQ 0 OR finite(x) EQ 0, ct)
  if ct GT 0 OR finite(ratio) EQ 0 then begin
      FAIL_OVERFLOW:
      errmsg = ('ERROR: parameter or function value(s) have become '+$
                'infinite; check model function for over- '+$
                'and underflow')
      info = -16
      goto, TERMINATE
  endif

  ;; End of outer loop.
  goto, OUTER_LOOP

TERMINATE:
  catch_msg = 'in the termination phase'
  ;; Termination, either normal or user imposed.
  if iflag LT 0 then info = iflag
  iflag = 0
  if n_elements(xnew) EQ 0 then goto, FINAL_RETURN
  if nfree EQ 0 then xnew = xall else xnew[ifree] = x
  if n_elements(qanytied) GT 0 then if qanytied then mpfit_tie, xnew, ptied
  dof = n_elements(fvec) - nfree


  ;; Call the ITERPROC at the end of the fit, if the fit status is
  ;; okay.  Don't call it if the fit failed for some reason.
  if info GT 0 then begin
      
      mperr = 0
      xnew0 = xnew
      
      call_procedure, iterproc, fcn, xnew, iter, fnorm^2, $
        FUNCTARGS=fcnargs, parinfo=parinfo, quiet=quiet, $
        dof=dof, _EXTRA=iterargs
      iflag = mperr

      if iflag LT 0 then begin  
          errmsg = 'WARNING: premature termination by "'+iterproc+'"'
      endif else begin
          ;; If parameters were changed (grrr..) then re-tie
          if max(abs(xnew0-xnew)) GT 0 then begin
              if qanytied then mpfit_tie, xnew, ptied
              x = xnew[ifree]
          endif
      endelse

  endif

  ;; Initialize the number of parameters pegged at a hard limit value
  npegged = 0L
  if n_elements(qanylim) GT 0 then if qanylim then begin
      wh = where((qulim AND (x EQ ulim)) OR $
                 (qllim AND (x EQ llim)), npegged)
  endif

  ;; Calculate final function value (FNORM) and residuals (FVEC)
  if isext EQ 0 AND nprint GT 0 AND info GT 0 then begin
      catch_msg = 'calling '+fcn
      fvec = mpfit_call(fcn, xnew, _EXTRA=fcnargs)
      catch_msg = 'in the termination phase'
      fnorm = mpfit_enorm(fvec)
  endif

  if n_elements(fnorm) EQ 0 AND n_elements(fnorm1) GT 0 then fnorm = fnorm1
  if n_elements(fnorm) GT 0 then fnorm = fnorm^2.

  covar = !values.d_nan
  ;; (very carefully) set the covariance matrix COVAR
  if info GT 0 AND NOT keyword_set(nocovar) $
    AND n_elements(n) GT 0 $
    AND n_elements(fjac) GT 0 AND n_elements(ipvt) GT 0 then begin
      sz = size(fjac)
      if n GT 0 AND sz[0] GT 1 AND sz[1] GE n AND sz[2] GE n $
        AND n_elements(ipvt) GE n then begin
          catch_msg = 'computing the covariance matrix'
          if n EQ 1 then $
            cv = mpfit_covar(reform([fjac[0,0]],1,1), ipvt[0]) $
          else $
            cv = mpfit_covar(fjac[0:n-1,0:n-1], ipvt[0:n-1])
          cv = reform(cv, n, n, /overwrite)
          nn = n_elements(xall)
          
          ;; Fill in actual covariance matrix, accounting for fixed
          ;; parameters.
          covar = replicate(zero, nn, nn)
          for i = 0L, n-1 do begin
              covar[ifree, ifree[i]] = cv[*,i]
          end
          
          ;; Compute errors in parameters
          catch_msg = 'computing parameter errors'
          i = lindgen(nn)
          perror = replicate(abs(covar[0])*0., nn)
          wh = where(covar[i,i] GE 0, ct)
          if ct GT 0 then $
            perror[wh] = sqrt(covar[wh, wh])
      endif
  endif

;  catch_msg = 'returning the result'
;  profvals.mpfit = profvals.mpfit + (systime(1) - prof_start)

  FINAL_RETURN:
  mpfit_fencepost_active = 0
  nfev = mpconfig.nfev
  if n_elements(xnew) EQ 0 then return, !values.d_nan
  return, xnew

  
  ;; ------------------------------------------------------------------
  ;; Alternate ending if the user supplies the function and gradients
  ;; externally
  ;; ------------------------------------------------------------------

  SAVE_STATE:

  catch_msg = 'saving MPFIT state'

  ;; Names of variables to save
  varlist = ['alpha', 'delta', 'diag', 'dwarf', 'factor', 'fnorm', $
             'fjac', 'gnorm', 'nfree', 'ifree', 'ipvt', 'iter', $
             'm', 'n', 'machvals', 'machep0', 'npegged', $
             'whlpeg', 'whupeg', 'nlpeg', 'nupeg', $
             'mpconfig', 'par', 'pnorm', 'qtf', $
             'wa1', 'wa2', 'wa3', 'xnorm', 'x', 'xnew']
  cmd = ''

  ;; Construct an expression that will save them
  for i = 0L, n_elements(varlist)-1 do begin
      ival = 0
      dummy = execute('ival = n_elements('+varlist[i]+')')
      if ival GT 0 then begin
          cmd = cmd + ',' + varlist[i]+':'+varlist[i]
      endif
  endfor
  cmd = 'state = create_struct({'+strmid(cmd,1)+'})'
  state = 0

  if execute(cmd) NE 1 then $
    message, 'ERROR: could not save MPFIT state'

  ;; Set STATUS keyword to prepare for next iteration, and reset init
  ;; so we do not init the next time
  info = 9
  extinit = 0

  return, xnew

end

