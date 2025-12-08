;
; Initial code by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
;
; ---------------------------------------
; Modifications history :
;
; - 2025-Dec-08 : AC. managing ISML licence on IDL
;   ERF(/double) don't exist 
; ---------------------------------------
;
pro TEST_ERFINV_NUMERIC, cumul_error, test=test, verbose=verbose
;
errors=0
;
esp_vect=2e-6  ; relative error
;
; ---------------------------------------
; test on FLOAT
;
x = (2 * FINDGEN(100) / 99 - 1)[1 : 98]
for i=0, N_ELEMENTS(x) - 1 do begin
   ;; in a loop
   if ABS(IMSL_ERF(ERF(x[i]), /inv) - x[i]) gt 1e-6 then  begin
      ERRORS_ADD, errors, 'erf-1(erf(float x)) != x, index ['+STRING(i)+']'
   endif
   ;; in a loop, promoting to double
   if ABS(IMSL_ERF(ERF(DOUBLE(x[i])), /inv, /double) - x[i]) gt 1e-7 then begin
      MESSAGE, 'erf-1(erf(float x, /double), /double) != x', /conti
      EXIT, status=1
   endif
endfor
;; vector
if TOTAL(ABS(IMSL_ERF(ERF(x),/inv)-x)) GT esp_vect then begin
   ERRORS_ADD, errors, 'erf-1(erf( x)) != x, FLOAT Vector'
endif
if TOTAL(ABS(IMSL_ERF(ERF(DOUBLE(x)),/inv)-x)) GT esp_vect then begin
   ERRORS_ADD, errors, 'erf-1(erf( x)) != x, FLOAT DOUBLE Double'
endif
;
; ---------------------------------------
; test on DOUBLE
x = (2 * DINDGEN(100) / 99 - 1)[1 : 98]
;; loop
for i=0, n_elements(x) - 1 do begin
   if ABS(IMSL_ERF(ERF(x[i]), /inv) - x[i]) gt 1e-7 then begin
      ERRORS_ADD, errors, 'erf-1(erf(double x)) != x, index ['+STRING(i)+']'
   endif
endfor
; vector
if TOTAL(ABS(IMSL_ERF(ERF(x),/inv)-x)) GT esp_vect then begin
   ERRORS_ADD, errors, 'erf-1(erf( x)) != x, DOUBLE Vector '
endif
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_ERFINV_NUMERIC", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------
;
pro TEST_ERFINV_TYPE, cumul_error, test=test, verbose=verbose
;
errors=0
;
GIVE_LIST_NUMERIC, list_numeric_types
nb_types=N_ELEMENTS(list_numeric_types)
;
expect=REPLICATE('FLOAT', nb_types)
expect[4:6]=['DOUBLE','COMPLEX','DCOMPLEX']
;
; For ERF() and IMSL_ERF() all types are OK, including Complex & DComplex
;
for ii=0, nb_types-1 do begin
   if TYPENAME(ERF(FIX(0., type=list_numeric_types[ii]))) NE expect[ii] then $
      ERRORS_ADD, errors, 'Bad type in ERF() '+list_numeric_types[ii]
   if TYPENAME(IMSL_ERF(FIX(0., type=list_numeric_types[ii]))) NE expect[ii] then $
      ERRORS_ADD, errors, 'Bad type in ERF() '+list_numeric_types[ii]
endfor
;
; For IMSL_ERF(/inv) Complex & DComplex should be skipped !
;
for ii=0, nb_types-1 do begin
   res_exe=EXECUTE("val=IMSL_ERF(FIX(0., type=list_numeric_types[ii]),/inv)")
   if (res_exe GT 0) then begin
      if TYPENAME(val) NE expect[ii] then $
         ERRORS_ADD, errors, 'Bad type in IMSL_ERF(/inv) '+list_numeric_types[ii]
   endif
endfor
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_ERFINV_TYPE", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------
;
pro TEST_ERFINV, test=test, no_exit=no_exit, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_ERFINV, test=test, no_exit=no_exit, help=help'
   return
endif
;
cumul_errors=0
;
; we need a IMSL licence to run IMSL_ERF() in IDL 
;
if (GDL_IDL_FL() EQ 'IDL') then begin
   if ~LMGR('idlimsl') then begin
      MESSAGE, 'ISML licence missing in IDL', /continue
      if KEYWORD_SET(no_exit) then STOP else EXIT, status=77
   endif else  MESSAGE, 'ISML licence activated in IDL', /continue
endif
;
TEST_ERFINV_TYPE, cumul_error, test=test, verbose=verbose
TEST_ERFINV_NUMERIC, cumul_error, test=test, verbose=verbose
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_ERFINV', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
