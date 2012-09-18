;; Sacha Hony and Alain Coulais, 18 September 2012
;;
;; testing whether the _EXTRA are working as expected.
;;
;; It looks like IDL does not pass on _extra arguments that are
;; accepted by the pro/function which is being called
;
; --------------------------------------------
;
pro FFT_EXTRA_KEYWORDS, x, _extra=_extra, by_execute=by_execute
;
ON_ERROR, 2
;
common cerror, error
;
ok=EXECUTE("z1=FFT(x)")
pb=EXECUTE("z2=FFT(x,_extra=_extra)")
;
z3=FFT(x,_extra=_extra)
;
print, ok, pb, total(abs(z1)), total(abs(z2))
;
error=0
end
;
; --------------------------------------------
;
pro PLOT_EXTRA_KEYWORDS, x, _extra=_extra, by_execute=by_execute
;
ON_ERROR, 2
;
common cerror, error
;
PLOT, x,_extra=_extra
;
; if fact, xrange is not used by OPLOT ...
; (and is not is the keyword list ...)
;
print, !error_state
OPLOT, x,_extra=_extra, col=250
print, !error_state
;
error=0
end
;
; --------------------------------------------
;; Error message:
;; % OPLOT: Keyword XRANGE not allowed in call to: OPLOT
;
pro TEST_EXTRA_KEYWORDS
;
common cerror, error
;
nb_errors=0
;
error=1
x=DINDGEN(10)
PLOT_EXTRA_KEYWORDS, x, xrange=[0,5]
if error EQ 1 then nb_errors=nb_errors+1
;
error=1
x=DIST(128,64)
FFT_EXTRA_KEYWORDS, x, dim=1
FFT_EXTRA_KEYWORDS, x, forthefun=1
if error EQ 1 then nb_errors=nb_errors+1
;
end
