;
; under GNU GPL v2 or later
; Alain Coulais, 19-June-2012
; Mathieu Pinter, 19-June-2012
; 
pro BASIC_FINITE, data=data, comp=comp, double=double, test=test
;
if ~ARG_PRESENT(data) then begin
   data=[0., -0.0, 10, -10., !values.f_nan, -!values.f_nan]
   data=[data, !values.f_infinity, -!values.f_infinity]
endif
;
if KEYWORD_SET(double) then data=DOUBLE(data)
if KEYWORD_SET(comp) then data=COMPLEX(0.,0.)+data
;
print, data
print, FINITE(data)
print, FINITE(data,/nan)
print, FINITE(data,/inf)
;
print, FINITE(data,/nan, sign=-1)
print, FINITE(data,/nan, sign=0)
print, FINITE(data,/nan, sign=1)
;
print, FINITE(data,/inf, sign=-1)
print, FINITE(data,/inf, sign=0)
print, FINITE(data,/inf, sign=1)
;
if KEYWORD_SET(test) then STOP
;
end
; -------------------------------------------------
;
pro TEST_FINITE, help=help, test=test, debug=debug, $
                 no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_FINITE, help=help, test=test, debug=debug, $'
    print, '                 no_exit=no_exit, verbose=verbose'
    return
endif
;
nb_errors = 0
tab=[[1.,!VALUES.F_INFINITY,1],$
     [0,!VALUES.F_NAN,5],$
     [-!VALUES.F_INFINITY,-!VALUES.F_NAN,100]]
;
a=FINITE(tab) 
if a(1,0) NE 0 then nb_errors=nb_errors + 1
;
a=FINITE(tab, /INF) 
if a(1,0) NE 1 then nb_errors=nb_errors + 1
;
a=FINITE(tab, /NAN) 
if a(1,1) NE 1 then nb_errors=nb_errors + 1
;
a=FINITE(tab, /INF, SIGN=1) 
if a(1,0) NE 1 then nb_errors=nb_errors + 1
;
a=FINITE(tab, /INF, SIGN=-1) 
if a(0,2) NE 1 then nb_errors=nb_errors + 1
;
a=FINITE(tab, /NAN, SIGN=1) 
if a(1,1) NE 1 then nb_errors=nb_errors + 1
;
a=FINITE(tab, /NAN, SIGN=-1) 
if a(1,2) NE 1 then nb_errors=nb_errors + 1
;
if KEYWORD_SET(debug) then STOP
;
b=FLTARR(1000)
b[10:19]=-!VALUES.F_NAN
b[110:119]=!VALUES.F_NAN
a=WHERE(FINITE(b, /NAN, SIGN=-1) EQ 1, count)
if count NE 10 then nb_errors=nb_errors + 1
;
if count EQ 20 then begin
    MESSAGE,/continue, "This platform does not distinguish between +Nan and -Nan"
endif
;
a=WHERE(FINITE(b, /inf, SIGN=-1) EQ 1, count)
if count NE 0 then nb_errors=nb_errors + 1
; counting +Inf
b[210:219]=!VALUES.F_infinity
b[410:419]=-!VALUES.F_infinity
a=WHERE(FINITE(b, /inf, SIGN=1) EQ 1, count)
if count NE 10 then nb_errors=nb_errors + 1
if count EQ 20 then begin
    MESSAGE,/continue, "This platform does not distinguish between +Inf and -Inf"
endif
;
; now we have +-Nan and +inf, do we still know how to count ?
a=WHERE(FINITE(b, /NAN, SIGN=-1) EQ 1, count)
if count NE 10 then nb_errors=nb_errors + 1
;
if (nb_errors GT 0) then begin
    MESSAGE, STRING(nb_errors)+' Errors founded when testing FINITE', /continue
endif else begin
    MESSAGE, 'testing FINITE: No Errors founded', /continue
endelse
;
if KEYWORD_SET(test) then STOP
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
end
;
;
;
pro TEST_FINITE_TIMETEST, nbp=nbp, test=test

if ~ARG_PRESENT(nbp) then nbp=1e7

a=randomu(seed, nbp)

a(where(a GT 0.75))=!values.f_nan
a(where(a LT 0.25))=-!values.f_nan

print, 'finite only'
t0=systime(1) & b=finite(a) & print, systime(1)-t0

print, 'NaN only'
t0=systime(1) & b=finite(a, /nan) & print, systime(1)-t0
t0=systime(1) & b=finite(a, /nan, sign=1) & print, systime(1)-t0
t0=systime(1) & b=finite(a, /nan, sign=-1) & print, systime(1)-t0

print, 'Inf only'
t0=systime(1) & b=finite(a, /inf) & print, systime(1)-t0
t0=systime(1) & b=finite(a, /inf, sign=1) & print, systime(1)-t0
t0=systime(1) & b=finite(a, /inf, sign=-1) & print, systime(1)-t0

if KEYWORD_SET(test) then STOP


end
