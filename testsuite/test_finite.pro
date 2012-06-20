;
; under GNU GPL v2 or later
; Alain Coulais, 19-June-2012
; Mathieu Pinter, 19-June-2012
; 
; -------------------------------------------------
;
pro TEST_FINITE, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_FINITE, help=help, test=test, no_exit=no_exit, verbose=verbose'
    return
endif
;
nb_errors = 0
tab=[[1,!VALUES.F_INFINITY,1],[0,!VALUES.F_NAN,5],[-!VALUES.F_INFINITY,-!VALUES.F_NAN,100]]
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

