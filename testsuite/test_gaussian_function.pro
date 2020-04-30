;
; Alain C, 6 April 2020, under GNU GPL v3 or later
;
; ----------------------------------------------------
; Modifications history :
;
; 2018-April-06 : AC : creation, but not finished :(
;
; ----------------------------------------------------
;
pro TEST_GAUSSFUNC_SUM, cumul_errors, verbose=verbose, test=test
;
nb_errors=0
eps=1. ; below 1% error
;
for ii=3, 100 do begin
   sigma=1.1*ii
   integral=TOTAL(GAUSSIAN_FUNCTION(sigma))
   ;;
   exp_integral=sigma*SQRT(!pi*2.)
   if ((integral-exp_integral)/exp_integral GT eps) then $
      ERRORS_ADD, nb_errors, 'case Integral for : '+STRING(sigma)
   if KEYWORD_SET(verbose) then print, sigma, integral, exp_integral
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_GAUSSFUNC_SUM', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ----------------------------------------------------
;
pro TEST_GAUSSIAN_FUNCTION_NUMERIC, cumul_errors, verbose=verbose, test=test
;
nb_errors=0
eps=1.e-6
;
gauss31=MOMENT(GAUSSIAN_FUNCTION(3.1))
if (N_ELEMENTS(gauss31) NE 19) then $
      ERRORS_ADD, nb_errors, 'case Gauss31 NbEl 19 : '+STRING(N_ELEMENTS(gauss31))
exp_moment=[0.408123, 0.129434, 0.386746, -1.51982]


;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_GAUSSIAN_FUNCTION_NUMERIC', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ----------------------------------------------------
;
pro TEST_GAUSSIAN_FUNCTION_1D, cumul_errors, width=width, verbose=verbose, test=test
;
nb_errors=0
;
nbps=10
the_max=1+FINDGEN(nbps)
;
if ~KEYWORD_SET(width) then width=10
;
; in Odd case, we are far from Max ;)
if (width mod 2) EQ 0 then eps=0.25 else eps=1.e-6
;
the_width=REPLICATE(width,nbps)
;
for ii=0, nbps-1 do begin
   ;;
   mygauss=GAUSSIAN_FUNCTION(3, WIDTH=the_width[ii], MAXIMUM=the_max[ii], /double)
   ;;
   exp_max_val=the_max[ii]
   exp_max_pos=(the_width[ii]-1)/2
   ;;
   res_max_val=MAX(mygauss, res_max_pos)
   if ABS(res_max_val-exp_max_val) GT eps then $
      ERRORS_ADD, nb_errors, 'case max val @'+STRING(ii)
   ;;
   if (res_max_pos NE exp_max_pos) then $
      ERRORS_ADD, nb_errors, 'case pos @'+STRING(ii)

   ;;
   if KEYWORD_SET(verbose) then print, ii, exp_max_val, res_max_val, exp_max_pos, res_max_pos
;;stop
;;   print, MOMENT(mygauss)
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_GAUSSIAN_FUNCTION_1D', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ----------------------------------------------
;
pro TEST_GAUSSIAN_FUNCTION, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_GAUSSIAN_FUNCTION, help=help, test=test, $'
   print, '                            no_exit=no_exit, verbose=verbose'
   return
endif
;
cumul_errors=0
;
BANNER_FOR_TESTSUITE, 'TEST_GAUSSIAN_FUNCTION', 0, /line
TEST_GAUSSIAN_FUNCTION_1D, cumul_errors, width=10, verbose=verbose
TEST_GAUSSIAN_FUNCTION_1D, cumul_errors, width=11, verbose=verbose
TEST_GAUSSIAN_FUNCTION_1D, cumul_errors, width=12, verbose=verbose
TEST_GAUSSIAN_FUNCTION_1D, cumul_errors, width=20, verbose=verbose
TEST_GAUSSIAN_FUNCTION_1D, cumul_errors, width=21, verbose=verbose
;
BANNER_FOR_TESTSUITE, 'TEST_GAUSSIAN_FUNCTION', 0, /line
TEST_GAUSSFUNC_SUM, cumul_errors, verbose=verbose
BANNER_FOR_TESTSUITE, 'TEST_GAUSSIAN_FUNCTION', 0, /line
;TEST_GAUSSIAN_FUNCTION_2D, cumul_errors, verbose=verbose
;BANNER_FOR_TESTSUITE, 'TEST_GAUSSIAN_FUNCTION', 0, /line
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_GAUSSIAN_FUNCTION', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
