;
; Initial version Gilles.
;
; ---------------------------------------
; Modifications history :
;
; - 2024-May-29 : AC. small cosmetic change due to troubles on M1/M2
;
; ---------------------------------------
;
pro PRINT_DEBUG, mess, a, exp_a
print, mess, a, exp_a, ABS(a-exp_a)
end
; ---------------------------------------
;
pro TEST_ELMHES, test=test, no_exit=no_exit, help=help, $
                 verbose=verbose, debug=debug
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_ELMHES, test=test, no_exit=no_exit, help=help, $'
   print, '                 verbose=verbose, debug=debug'
   return
endif
;
nb_errors=0
eps=1e-5
;
a=FINDGEN(4,4)+0.5
b=ELMHES(a)
t=TOTAL(b)
p=PRODUCT(b)
expect_tot=101.32079315
expect_prod=0.0
mess='bad result ELMHES '
if (ABS(expect_tot-t) GT eps) then ERRORS_ADD, nb_errors, mess+'T'
if (ABS(expect_prod-p) GT eps) then ERRORS_ADD, nb_errors, mess+'P'
if KEYWORD_SET(debug) or KEYWORD_SET(verbose) then begin
   PRINT_DEBUG, 'p :', p, expect_prod
   PRINT_DEBUG, 't :', t, expect_tot
endif
if KEYWORD_SET(debug) then STOP
;
; test with /no_balance
;
b=ELMHES(a,/no_bal)
t=TOTAL(b)
p=PRODUCT(b)
expect_tot=96.5808
expect_prod=0.0
mess='bad result ELMHES, /no_balance : '
if (ABS(expect_tot-t) GT eps) then ERRORS_ADD, nb_errors, mess+'T'
if (ABS(expect_prod-p) GT eps) then ERRORS_ADD, nb_errors, mess+'P'
if KEYWORD_SET(debug) or KEYWORD_SET(verbose) then begin
   PRINT_DEBUG, 'p :', p, expect_prod
   PRINT_DEBUG, 't :', t, expect_tot
endif
if KEYWORD_SET(debug) then STOP
;
; test with /Column
;
b=ELMHES(a,/column)
t=TOTAL(b)
p=PRODUCT(b)
expect_tot=99.941322
expect_prod=0.0
mess='bad result ELMHES, /column : '
if (ABS(expect_tot-t) GT eps) then ERRORS_ADD, nb_errors, mess+'T'
if (ABS(expect_prod-p) GT eps) then ERRORS_ADD, nb_errors, mess+'P'
if KEYWORD_SET(debug) or KEYWORD_SET(verbose) then begin
   PRINT_DEBUG, 'p :', p, expect_prod
   PRINT_DEBUG, 't :', t, expect_tot
endif
if KEYWORD_SET(debug) then STOP
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_ELMHES', nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

