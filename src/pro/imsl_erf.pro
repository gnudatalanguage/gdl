; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
function imsl_erf, x, double=dbl, inverse=inv
  on_error, 2
  if ~keyword_set(inv) then return, erf(x, double=dbl)
  if total([x] ge 1 or [x] le -1) gt 0 then $
    message, 'The inverse error function is defined only for -1 < x < 1'
  return, gdl_erfinv(x, double=dbl)
end
