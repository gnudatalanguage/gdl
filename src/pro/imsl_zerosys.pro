; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>

function imsl_zerosys, f, n, double=double, err_rel=err_rel, $
  fnorm=fnorm, jacobian=jacobian, itmax=itmax, xguess=xguess
  on_error, 2

  ; informing about not supported keywords
  fmt = '(%"Warning: Keyword parameter %s not supported in call to: IMSL_ZEROSYS. Ignored.")'
  if keyword_set(err_rel) then message, string('ERR_REL', format=fmt), /continue
  if keyword_set(fnorm) then message, string('FNORM', format=fmt), /continue
  if keyword_set(jacobian) then message, string('JACOBIAN', format=fmt), /continue

  ; sanity checks
  if n_params() eq 0 then message, $
    'please supply a name of a function defining the system as the first parameter'
  if n_params() eq 1 && ~keyword_set(xguess) then message, $
    'please supply the size of the system as the second parameter'

  ; calling NEWTON with the magick /HYBRID keyword for switching to the modified 
  ; Powell hydrid algorithm (FIXME: error messages prefixed by NEWTON)
  if ~keyword_set(xguess) then xguess = keyword_set(double) ? dblarr(n) : fltarr(n)
  return, newton(xguess, f, double=double, itmax=itmax, /hybrid)

end
