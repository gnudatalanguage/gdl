;
; Alain C, 6 April 2020, under GNU GPL v3 or later
;
; ----------------------------------------------------
; Modifications history :
;
; 2018-April-06 : AC : creation, but not finished :(
; 2025-Dec-02 : AC : fix some details ... working now in IDL and GDL
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
eps=5.e-6
;
gauss3p1=GAUSSIAN_FUNCTION(3.1)
nbp3p1=N_ELEMENTS(gauss3p1)
;
if (nbp3p1 NE 23) then begin
   ERRORS_ADD, nb_errors, 'case Gauss3p1 NbEl 23 : '+STRING(nbp3p1)
endif
;
;exp_moment=[0.408123, 0.129434, 0.386746, -1.51982]
exp_moment=[0.337784, 0.130470, 0.637705, -1.24552]
;0.38026920, 0.12933515, 0.48781705, -1.3957111]
;
if (TOTAL(ABS(exp_moment-MOMENT(gauss3p1))) GT eps) then begin
   ERRORS_ADD, nb_errors, 'case Gauss3p1 Moment'
endif
if KEYWORD_SET(verbose) then begin
   print, 'Expected : ', exp_moment
   print, 'Computed : ', MOMENT(gauss3p1)
endif
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_GAUSSIAN_FUNCTION_NUMERIC', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
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
eps=1.e-6
;
the_width=REPLICATE(width,nbps)
;
for ii=0, nbps-1 do begin
   ;;
   if KEYWORD_SET(verbose) then print, ii, the_width[ii],the_max[ii]
   mygauss=GAUSSIAN_FUNCTION(3, WIDTH=the_width[ii], MAXIMUM=the_max[ii], /double)
;   if KEYWORD_SET(verbose) then print, ii, mygauss
   ;;
   exp_max_val=the_max[ii]
   exp_max_pos=the_width[ii]/2
   ;;
   ;;   print, moment(mygauss)
   res_max_val=MAX(mygauss, res_max_pos)
   ;stop
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
TEST_GAUSSIAN_FUNCTION_1D, cumul_errors, width=10, verbose=verbose
TEST_GAUSSIAN_FUNCTION_1D, cumul_errors, width=11, verbose=verbose
TEST_GAUSSIAN_FUNCTION_1D, cumul_errors, width=12, verbose=verbose
TEST_GAUSSIAN_FUNCTION_1D, cumul_errors, width=20, verbose=verbose
TEST_GAUSSIAN_FUNCTION_1D, cumul_errors, width=21, verbose=verbose
;
TEST_GAUSSFUNC_SUM, cumul_errors, verbose=verbose
;
; testing 2D is not ready
;TEST_GAUSSIAN_FUNCTION_2D, cumul_errors, verbose=verbose
;
TEST_GAUSSIAN_FUNCTION_NUMERIC, cumul_errors, verbose=verbose
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
