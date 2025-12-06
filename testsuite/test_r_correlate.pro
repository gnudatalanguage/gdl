; Test routine for R_CORRELATE
;
; ---------------------------------------
; Modifications history :
;
; * 2025-12-03 : AC : transforming into automatic tests
;
; ---------------------------------------
;
pro TEST_R_CORRELATE, help=help, test=test, verbose=verbose, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_R_CORRELATE, help=help, test=test, verbose=verbose, no_exit=no_exit'
   return
endif 
;
cumul_errors=0
errors=0
eps=1e-5
;
a = [-0.932941, -0.903630, 0.877947, -2.29233, -2.38904, -0.141365, $
     -0.222776, 0.279628, -1.78928,  0.841182, -0.490696, 0.958234, $
     -1.01543, 0.763536, 0.278744, 0.497412, -0.826807, -1.43363, $
     0.512874, 1.06678]
b = [0.467973, 0.824236, 0.408903, -1.15425, 0.462008, 0.298134, $
     -0.354388,  -0.465703, 0.699337, 1.89301, -0.190556, -1.04368, $
     0.466777, 0.800296, 0.642730, 0.990312, -0.965548, -0.0831093, $
     0.514957, 0.908564] 
;
print, "No trend."
expect_Spearman=[ 0.26917294, 0.25113144]
expect_Kendall =[ 0.20000000, 0.21761949]
res_Spearman=R_CORRELATE(a, b)
res_Kendall =R_CORRELATE(a, b, /kendall)
if TOTAL(ABS(res_Spearman-expect_Spearman)) GT eps then $
   ERRORS_ADD, errors, 'Spearman (no trend)'
if TOTAL(ABS(res_Kendall-expect_Kendall)) GT eps then $
   ERRORS_ADD, errors, 'Kendall (no trend)'
;
; Adding a trend
;
print, "With trend."
expect_t_Spearman=[ 0.99248123, 0.]
expect_t_Kendall =[ 0.95789474, 0.]
trend=FINDGEN(20)
res_t_Spearman=R_CORRELATE(trend+a, trend+b)
res_t_Kendall =R_CORRELATE(trend+a, trend+b, /kendall)
if TOTAL(ABS(res_t_Spearman-expect_t_Spearman)) GT eps then $
   ERRORS_ADD, errors, 'Spearman + Trend'
if TOTAL(ABS(res_t_Kendall-expect_t_Kendall)) GT eps then $
   ERRORS_ADD, errors, 'Kendall + Trend'
;
cumul_errors=errors
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_R_CORRELATE', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

