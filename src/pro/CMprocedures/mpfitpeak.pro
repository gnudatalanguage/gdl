;+
; NAME:
;   MPFITPEAK
;
; AUTHOR:
;   Craig B. Markwardt, NASA/GSFC Code 662, Greenbelt, MD 20770
;   craigm@lheamail.gsfc.nasa.gov
;   UPDATED VERSIONs can be found on my WEB PAGE: 
;      http://cow.physics.wisc.edu/~craigm/idl/idl.html
;
; PURPOSE:
;   Fit a gaussian, lorentzian or Moffat model to data
;
; MAJOR TOPICS:
;   Curve and Surface Fitting
;
; CALLING SEQUENCE:
;   yfit = MPFITPEAK(X, Y, A, NTERMS=nterms, ...)
;
; DESCRIPTION:
;
;   MPFITPEAK fits a gaussian, lorentzian or Moffat model using the
;   non-linear least squares fitter MPFIT.  MPFITPEAK is meant to be a
;   drop-in replacement for IDL's GAUSSFIT function (and requires
;   MPFIT and MPFITFUN).
;
;   The choice of the fitting function is determined by the keywords
;   GAUSSIAN, LORENTZIAN and MOFFAT.  By default the gaussian model
;   function is used.  [ The Moffat function is a modified Lorentzian
;   with variable power law index. (Moffat, A. F. J. 1969, Astronomy &
;   Astrophysics, v. 3, p. 455-461) ]
;
;   The functional form of the baseline is determined by NTERMS and
;   the function to be fitted.  NTERMS represents the total number of
;   parameters, A, to be fitted.  The functional forms and the
;   meanings of the parameters are described in this table:
;
;                 GAUSSIAN#          Lorentzian#         Moffat#
;
;   Model     A[0]*exp(-0.5*u^2)   A[0]/(u^2 + 1)   A[0]/(u^2 + 1)^A[3]
;
;   A[0]         Peak Value          Peak Value        Peak Value
;   A[1]        Peak Centroid       Peak Centroid     Peak Centroid
;   A[2]       Gaussian Sigma           HWHM%             HWHM%
;   A[3]         + A[3]    *          + A[3]   *      Moffat Index
;   A[4]         + A[4]*x  *          + A[4]*x *         + A[4]   *
;   A[5]                                                 + A[5]*x *
;
;   Notes: # u = (x - A[1])/A[2]
;          % Half-width at half maximum
;          * Optional depending on NTERMS
;
;   By default the initial starting values for the parameters A are
;   estimated from the data.  However, explicit starting values can be
;   supplied using the ESTIMATES keyword.  Also, error or weighting
;   values can optionally be provided; otherwise the fit is
;   unweighted.
;
;   MPFITPEAK fits the peak value of the curve.  The area under a
;   gaussian peak is A[0]*A[2]*SQRT(2*!DPI); the area under a
;   lorentzian peak is A[0]*A[2]*!DPI.
;
;  Data values of NaN or Infinity for "Y", "ERROR" or "WEIGHTS" will
;  be ignored as missing data if the NAN keyword is set.  Otherwise,
;  they may cause the fitting loop to halt with an error message.
;  Note that the fit will still halt if the model function, or its
;  derivatives, produces infinite or NaN values, or if an "X" value is
;  missing.
;
; RESTRICTIONS:
;
;   If no starting parameter ESTIMATES are provided, then MPFITPEAK
;   attempts to estimate them from the data.  This is not a perfect
;   science; however, the author believes that the technique
;   implemented here is more robust than the one used in IDL's
;   GAUSSFIT.  The author has tested cases of strong peaks, noisy
;   peaks and broad peaks, all with success.
;
;   Users should be aware that if the baseline term contains a strong
;   linear component then the automatic estimation may fail.  For
;   automatic estimation to work the peak amplitude should dominate
;   over the the maximum baseline.
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
;   X - Array of independent variable values, whose values should
;       monotonically increase.
;
;   Y - Array of "measured" dependent variable values.  Y should have
;       the same data type and dimension as X.
;         NOTE: the following special cases apply:
;                * if Y is NaN or Infinite, and the NAN keyword is
;                  set, then the corresponding data point is ignored
;
; OUTPUTS:
;   A - Upon return, an array of NTERMS best fit parameter values.
;       See the table above for the meanings of each parameter
;       element.
;
;
; RETURNS:
;
;   Returns the best fitting model function.
;
; KEYWORDS:
;
;   ** NOTE ** Additional keywords such as PARINFO, BESTNORM, and
;              STATUS are accepted by MPFITPEAK but not documented
;              here.  Please see the documentation for MPFIT for the
;              description of these advanced options.
;
;   AUTODERIV - Set to 1 to have MPFIT compute the derivatives numerically.
;          Default is 0 -  derivatives are computed analytically, which is 
;              generally faster.    (Prior to Jan 2011, the default was 1)
;
;   CHISQ - the value of the summed squared residuals for the
;           returned parameter values.
;
;   DOF - number of degrees of freedom, computed as
;             DOF = N_ELEMENTS(DEVIATES) - NFREE
;         Note that this doesn't account for pegged parameters (see
;         NPEGGED).
;
;   ERROR - upon input, the measured 1-sigma uncertainties in the "Y"
;           values.  If no ERROR or WEIGHTS are given, then the fit is
;           unweighted.
;         NOTE: the following special cases apply:
;                * if ERROR is zero, then the corresponding data point
;                  is ignored
;                * if ERROR is NaN or Infinite, and the NAN keyword is
;                  set, then the corresponding data point is ignored
;                * if ERROR is negative, then the absolute value of 
;                  ERROR is used.
;
;   ESTIMATES - Array of starting values for each parameter of the
;               model.  The number of parameters should at least be
;               three (four for Moffat), and if less than NTERMS, will
;               be extended with zeroes.  If ESTIMATES is not set,
;               then the starting values are estimated from the data
;               directly, before fitting.  (This also means that
;               PARINFO.VALUES is ignored.)
;               Default: not set - parameter values are estimated from data.
;
;   GAUSSIAN - if set, fit a gaussian model function.  The Default.
;   LORENTZIAN - if set, fit a lorentzian model function.
;   MOFFAT - if set, fit a Moffat model function.
;
;   MEASURE_ERRORS - synonym for ERRORS, for consistency with built-in
;                    IDL fitting routines.
;
;   NAN - ignore infinite or NaN values in the Y, ERR or WEIGHTS
;         parameters.  These values will be treated as missing data.
;         However, the fit will still halt with an error condition if
;         the model function becomes infinite, or if X has missing
;         values.
;
;   NEGATIVE / POSITIVE - if set, and ESTIMATES is not provided, then
;                         MPFITPEAK will assume that a
;                         negative/positive peak is present.
;                         Default: determined automatically
;
;   NFREE - the number of free parameters in the fit.  This includes
;           parameters which are not FIXED and not TIED, but it does
;           include parameters which are pegged at LIMITS.
;
;   NO_FIT - if set, then return only the initial estimates without
;            fitting.  Useful to find out what the estimates the
;            automatic guessing algorithm produced.  If NO_FIT is set,
;            then SIGMA and CHISQ values are not produced.  The
;            routine returns, NAN, and STATUS=5.
;
;   NTERMS - An integer describing the number of fitting terms.
;            NTERMS must have a minimum value, but can optionally be
;            larger depending on the desired baseline.  
;
;            For gaussian and lorentzian models, NTERMS must be three
;            (zero baseline), four (constant baseline) or five (linear
;            baseline).  Default: 4
;
;            For the Moffat model, NTERMS must be four (zero
;            baseline), five (constant baseline), or six (linear
;            baseline).  Default: 5
;
;   PERROR - upon return, the 1-sigma uncertainties of the parameter
;            values A.  These values are only meaningful if the ERRORS
;            or WEIGHTS keywords are specified properly.
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
;   QUIET - if set then diagnostic fitting messages are suppressed.
;           Default: QUIET=1 (i.e., no diagnostics)
;
;   SIGMA - synonym for PERROR (1-sigma parameter uncertainties), for
;           compatibility with GAUSSFIT.  Do not confuse this with the
;           Gaussian "sigma" width parameter.
;
;   WEIGHTS - Array of weights to be used in calculating the
;             chi-squared value.  If WEIGHTS is specified then the ERROR
;             keyword is ignored.  The chi-squared value is computed
;             as follows:
;
;                CHISQ = TOTAL( (Y-MYFUNCT(X,P))^2 * ABS(WEIGHTS) )
;
;             Here are common values of WEIGHTS:
;
;                1D/ERR^2 - Normal weighting (ERR is the measurement error)
;                1D/Y     - Poisson weighting (counting statistics)
;                1D       - Unweighted
;
;             The ERROR keyword takes precedence over any WEIGHTS
;             keyword values.  If no ERROR or WEIGHTS are given, then
;             the fit is unweighted.
;         NOTE: the following special cases apply:
;                * if WEIGHTS is zero, then the corresponding data point
;                  is ignored
;                * if WEIGHTS is NaN or Infinite, and the NAN keyword is
;                  set, then the corresponding data point is ignored
;                * if WEIGHTS is negative, then the absolute value of 
;                  WEIGHTS is used.
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
;   yi = gauss1(x, [2.2D, 1.4, 3000.]) + 1000       ; "Ideal" Y variable
;   y  = yi + randomn(seed, npts) * sqrt(1000. + yi); Measured, w/ noise
;   sy = sqrt(1000.D + y)                           ; Poisson errors
;
;   ; Now fit a Gaussian to see how well we can recover the original
;   yfit = mpfitpeak(x, y, a, error=sy)
;   print, p
;
;   Generates a synthetic data set with a Gaussian peak, and Poisson
;   statistical uncertainty.  Then the same function is fitted to the
;   data.
;
; REFERENCES:
;
;   MINPACK-1, Jorge More', available from netlib (www.netlib.org).
;   "Optimization Software Guide," Jorge More' and Stephen Wright, 
;     SIAM, *Frontiers in Applied Mathematics*, Number 14.
;
; MODIFICATION HISTORY:
;
;   New algorithm for estimating starting values, CM, 31 Oct 1999
;   Documented, 02 Nov 1999
;   Small documentation fixes, 02 Nov 1999
;   Slight correction to calculation of dx, CM, 02 Nov 1999
;   Documented PERROR for unweighted fits, 03 Nov 1999, CM
;   Copying permission terms have been liberalized, 26 Mar 2000, CM
;   Change requirements on # elements in X and Y, 20 Jul 2000, CM
;      (thanks to David Schlegel <schlegel@astro.princeton.edu>)
;   Added documentation on area under curve, 29 Aug 2000, CM
;   Added POSITIVE and NEGATIVE keywords, 17 Nov 2000, CM
;   Added reference to Moffat paper, 10 Jan 2001, CM
;   Added usage message, 26 Jul 2001, CM
;   Documentation clarification, 05 Sep 2001, CM
;   Make more consistent with comparable IDL routines, 30 Jun 2003, CM
;   Assumption of sorted data was removed, CM, 06 Sep 2003, CM
;   Add some defensive code against divide by zero, 30 Nov 2005, CM
;   Add some defensive code against all Y values equal to each other,
;     17 Apr 2005, CM
;   Convert to IDL 5 array syntax (!), 16 Jul 2006, CM
;   Move STRICTARR compile option inside each function/procedure, 9 Oct 2006
;   Add COMPATIBILITY section, CM, 13 Dec 2007
;   Missed some old IDL 4 () array syntax, now corrected, 13 Jun 2008
;   Slightly more error checking for pathalogical case, CM, 11 Nov 2008
;   Clarify documentation regarding what happens when ESTIMATES is not
;     set, CM, 14 Dec 2008
;   Add the NAN keyword, document how NAN, WEIGHTS and ERROR interact,
;     CM, 30 Mar 2009
;   Correct one case of old IDL 4 () array syntax (thanks to I. Urra),
;     CM, 25 Jan 2010
;  Improve performance by analytic derivative computation, added AUTODERIV 
;      keyword, W. Landsman, 2011-01-21
;  Move estimation code to its own function; allow the user to compute
;    only the estimate and return immediately without fitting,
;    C. Markwardt, 2011-07-12
;
;  $Id: mpfitpeak.pro,v 1.19 2011/12/08 17:51:33 cmarkwar Exp $
;-
; Copyright (C) 1997-2001, 2003, 2005, 2007, 2008, 2009, 2010, Craig Markwardt
; This software is provided as is without any warranty whatsoever.
; Permission to use, copy, modify, and distribute modified or
; unmodified copies is granted, provided this copyright and disclaimer
; are included unchanged.
;-

forward_function mpfit, mpfitfun, mpfitpeak, mpfitpeak_gauss, $
  mpfitpeak_lorentz, mpfitpeak_moffat, mpfitpeak_u

function mpfitpeak_u, x, p
  COMPILE_OPT strictarr
  wid = abs(p[2]) > 1e-20
  return, ((x-p[1])/wid)
end

; Gaussian Function
function mpfitpeak_gauss, x, p, pder, _extra=extra
  COMPILE_OPT strictarr,hidden
  sz = size(x,/type)
  if sz EQ 5 then smax = 26D else smax = 13.
  u = mpfitpeak_u(x, p)
  mask = abs(u) LT smax  ;; Prevents floating underflow
  Np = N_elements(p)
  if Np GE 4 then f = p[3] else f = 0
  if Np GE 5 then f = f + p[4]*x
  uz = mask*exp(-0.5 * u^2 * mask)
  if N_params() GT 2 then begin    ;; Compute derivatives if asked
      pder = make_array(N_elements(x), Np,type= size(p,/type) )
      pder[*,0] = uz      
      if p[2] NE 0 then pder[*,1] = p[0]*uz*u/p[2]
      pder[*,2] = pder[*,1]*u
      if Np GE 4 then pder[*,3] = 1.
      if Np GE 5 then pder[*,4] = x
      endif
  return,  f + p[0] * uz
end

; Lorentzian Function
function mpfitpeak_lorentz, x, p, pder, _extra=extra
  COMPILE_OPT strictarr,hidden
  u = mpfitpeak_u(x, p)
  Np = N_elements(p)
  if Np GE 4 then f = p[3] else f = 0
  if Np GE 5 then f = f + p[4]*x
  denom = 1/(u^2 + 1)
  if N_params() GT 2 then begin   ;; Compute derivatives if asked
      pder = make_array(N_elements(x), Np,type= size(p,/type) )
      pder[*,0] = denom
      if p[2] NE 0 then pder[*,1] = 2*p[0]*u*denom*denom/p[2]    
      pder[*,2] = pder[*,1]*u
      if Np GE 4 then pder[*,3] = 1.
      if Np GE 5 then pder[*,4] = x
    endif
    return, f + p[0] *denom
end

; Moffat Function
function mpfitpeak_moffat, x, p, pder,_extra=extra
  COMPILE_OPT strictarr
  u = mpfitpeak_u(x, p)
  Np = N_elements(p)
  if Np GE 5 then f = p[4] else f = 0
  if Np GE 6 then f = f + p[5]*x
  denom0 =   (u^2 +1)
  denom = denom0^(-p[3])
  if N_params() GT 2 then begin      ;; Compute derivatives if asked
      pder = make_array(N_elements(x), Np,type= size(p,/type) )
      pder[*,0] = denom
      if p[2] NE 0 then pder[*,1] = 2*p[3]*p[0]*u*denom/p[2]/denom0    
      pder[*,2] = pder[*,1]*u
      pder[*,3] = -alog(denom0)*p[0]*denom
      if Np GE 5 then pder[*,4] = 1.
      if Np GE 6 then pder[*,5] = x
    endif
  return, f + p[0]* denom
end

;
; Utility function to estimate peak parameters from an X,Y dataset
;
; X - independent variable
; Y - dependent variable (possibly noisy)
; NAN - if set, then ignore NAN values
; POSITIVE_PEAK - if set, search for positive peak
; NEGATIVE_PEAK - if set, search for negative peak
; (if neither POSITIVE_PEAK nor NEGATIVE_PEAK is set, then search
; for the largest magnitude peak)
; ERRMSG - upon return, set to an error code if an error occurred
;
function mpfitpeak_est, x, y, nan=nan, $
                          positive_peak=pos, negative_peak=neg, $
                          errmsg=errmsg

  ;; Here is the secret - the width is estimated based on the area
  ;; above/below the average.  Thus, as the signal becomes more
  ;; noisy the width automatically broadens as it should.

  nx = n_elements(x)

  is = sort(x)
  xs = x[is] & ys = y[is]
  maxx = max(xs, min=minx) & maxy = max(ys, min=miny, nan=nan)
  dx = 0.5 * [xs[1]-xs[0], xs[2:*] - xs, xs[nx-1] - xs[nx-2]]
  totarea = total(dx*ys, nan=nan)       ;; Total area under curve
  av = totarea/(maxx - minx)  ;; Average height

  ;; Degenerate case: all flat with no noise
  if miny EQ maxy then begin
      est = ys[0]*0.0 + [0,xs[nx/2],(xs[nx-1]-xs[0])/2, ys[0]]
      guess = 1
      return, est
  endif

  ;; Compute the spread in values above and below average... we
  ;; take the narrowest one as the one with the peak
  wh1 = where(y GE av, ct1)
  wh2 = where(y LE av, ct2)
  if ct1 EQ 0 OR ct2 EQ 0 then begin
      errmsg = 'ERROR: average Y value should fall within the range of Y data values but does not'
      return, !values.d_nan
  endif
  sd1 = total(x[wh1]^2)/ct1 - (total(x[wh1])/ct1)^2
  sd2 = total(x[wh2]^2)/ct2 - (total(x[wh2])/ct2)^2
      
  ;; Compute area above/below average

  if keyword_set(pos) then goto, POS_PEAK
  if keyword_set(neg) then goto, NEG_PEAK
  if sd1 LT sd2 then begin  ;; This is a positive peak
      POS_PEAK:
      cent  = x[where(y EQ maxy)] & cent = cent[0]
      peak  = maxy - av
  endif else begin          ;; This is a negative peak
      NEG_PEAK:
      cent  = x[where(y EQ miny)] & cent = cent[0]
      peak  = miny - av
  endelse
  peakarea = totarea - total(dx*(ys<av), nan=nan)
  if peak EQ 0 then peak = 0.5*peakarea
  width = peakarea / (2*abs(peak))
  if width EQ 0 OR finite(width) EQ 0 then width = median(dx)

  est = [peak, cent, width, av]
  return, est
end


function mpfitpeak, x, y, a, estimates=est, nterms=nterms, $
                    gaussian=gauss, lorentzian=lorentz, moffat=moffat, $
                    perror=perror, sigma=sigma, yerror=yerror, $
                    chisq=chisq, bestnorm=bestnorm, niter=iter, nfev=nfev, $
                    error=dy, weights=weights, measure_errors=dym, $
                    nfree=nfree, dof=dof, nan=nan, $
                    no_fit=no_fit, $
                    negative=neg, positive=pos, parinfo=parinfo, $
                    best_fjac=best_fjac, pfree_index=pfree_index, covar=covar,$
                    errmsg=errmsg, status=status, autoderiv=autoderiv0, $
                    query=query, quiet=quiet, _extra=extra

  COMPILE_OPT strictarr
  status = 0L
  errmsg = ''

  if n_params() EQ 0 then begin
      message, 'USAGE: yfit = MPFITPEAK(X, Y, A, ...)', /info
      return, !values.d_nan
  endif

  ;; Detect MPFIT and crash if it was not found
  catch, catcherror
  if catcherror NE 0 then begin
      MPFIT_NOTFOUND:
      catch, /cancel
      message, 'ERROR: the required functions MPFIT and MPFITFUN ' + $
        'must be in your IDL path', /info
      return, !values.d_nan
  endif
  if mpfit(/query)    NE 1 then goto, MPFIT_NOTFOUND
  if mpfitfun(/query) NE 1 then goto, MPFIT_NOTFOUND
  catch, /cancel
  if keyword_set(query) then return, 1

  ;; Check the number of parameter estimates
  if n_elements(quiet) EQ 0 then quiet = 1
  if n_elements(nterms) EQ 0 then nterms = 4
  if n_elements(autoderiv0) EQ 0 then autoderiv = 0 else autoderiv = keyword_set(autoderiv0)

  ;; Reject data vectors that are too simple
  if n_elements(x) LT nterms OR n_elements(y) LT nterms then begin
      errmsg = 'ERROR: X and Y must have at least NTERMS elements'
      message, errmsg, /cont
      status = 0
      return, !values.d_nan
  endif

  ;; Compute the weighting factors to use
  if (n_elements(dy) EQ 0 AND n_elements(weights) EQ 0 AND $
      n_elements(dym) EQ 0) then begin
      weights = x*0+1        ;; Unweighted by default
  endif else if n_elements(dy) GT 0 then begin
      weights = dy * 0   ;; Avoid division by zero
      wh = where(dy NE 0, ct)
      if ct GT 0 then weights[wh] = 1./dy[wh]^2
  endif else if n_elements(dym) GT 0 then begin
      weights = dym * 0   ;; Avoid division by zero
      wh = where(dym NE 0, ct)
      if ct GT 0 then weights[wh] = 1./dym[wh]^2
  endif

  ;; If the user did not supply an estimate of the parameter values,
  ;; then try to guestimate them.
  if n_elements(est) EQ 0 then begin
      guess = 1
      est = mpfitpeak_est(x, y, nan=nan, pos=pos, neg=neg, $
                          errmsg=errmsg)

      if errmsg NE '' then begin
          message, errmsg, /cont
          status = 0
      endif
  endif

  ;; Parameter checking for individual function types
  np = 3
  if keyword_set(moffat) then begin               ;; MOFFAT
      fun = 'mpfitpeak_moffat'
      if keyword_set(guess) then est = [est[0:2], 1, est[3:*]]
      np = 4
  endif else if keyword_set(lorentz) then begin  ;; LORENTZIAN
      fun = 'mpfitpeak_lorentz'
  endif else begin                               ;; GAUSSIAN
      fun = 'mpfitpeak_gauss'
  endelse
  if n_elements(est) LT np then begin
      errmsg = 'ERROR: parameter ESTIMATES must have at least '+strtrim(np,2)+$
        ' elements'
      message, errmsg, /cont
      return, !values.d_nan
  endif
  if nterms[0] LT np then begin
      errmsg = 'ERROR: NTERMS must be at least '+strtrim(np,2)
      message, errmsg, /cont
      return, !values.d_nan
  endif
  p0 = replicate(est[0]*0, nterms[0] > n_elements(est))
  p0[0] = est

  ;; If the user wanted only to get an estimate, then return here
  if keyword_set(no_fit) then begin
      status = 5
      a = est
      return, !values.d_nan
  endif

  ;; Function call
  a = mpfitfun(fun, x, y, 0, p0[0:nterms[0]-1], weights=weights, $
               bestnorm=bestnorm, nfev=nfev, status=status, $
               nfree=nfree, dof=dof, nan=nan, $
               parinfo=parinfo, perror=perror, niter=iter, yfit=yfit, $
               best_fjac=best_fjac, pfree_index=pfree_index, covar=covar, $
               quiet=quiet, errmsg=errmsg, autoderiv=autoderiv, _EXTRA=extra)

  ;; Print error message if there is one.
  if NOT keyword_set(quiet) AND errmsg NE '' then $
    message, errmsg, /cont

  if status NE 0 then begin
      ;; Make sure the width is positive
      a[2] = abs(a[2])

      ;; For compatibility with GAUSSFIT
      if n_elements(perror)   GT 0 then sigma = perror
      if n_elements(bestnorm) GT 0 then chisq = bestnorm

      ;; Root mean squared of residuals
      yerror = a[0]*0
      if n_elements(dof) GT 0 AND dof[0] GT 0 then begin
          yerror[0] = sqrt( total( (y-yfit)^2, nan=nan ) / dof[0])
      endif

      return, yfit
  endif

  return, !values.d_nan
end

