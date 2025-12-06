;
; Testing POLY_FIT
;
; Licensed under GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
;
; ---------------------
; Modification history:
;
; 2019-05-06 : GD 
; - Basic POLY_FIT test
;
; - 2025-Nov-24 : AC. Should work again ! We do had a confusion
;        with the /ran1 keyword
;
; - 2025-Dec-04 : AC. Add the Zanardo data to show how to fail ...
;   Add tests on slopes ...
;
; ---------------------
;
pro TEST_POLY_FIT_ZANARDO, cumul_errors, test=test, verbose=verbose
;
errors=0
;
; AC : it is important that the type of days is INT
; to fail with the old POLY_FIT code ...
;
days=[6139, 6172, 6199, 6244, 6283, 6355, 6461, 6494, 6526, 6603, 6627, $
      6690, 6770, 6836, 6889, 6967, 6989, 7039, 7084, 7202, 7231, 7283, $
      7297, 7370, 7437, 7486, 7558, 7580, 7622, 7692, 7730, 7802, 7847, $
      7901, 7932, 7991, 8014]
flux=[133.900, 132.900, 136.200, 139.100, 138.500, 148.600, 154.400, $
      160.000, 160.400, 165.400, 165.700, 171.000, 175.700, 178.100, $
      184.800, 190.300, 179.700, 196.000, 194.700, 216.800, 206.200, $
      198.900, 213.000, 218.900, 229.500, 227.300, 244.300, 237.800, $
      239.500, 216.800, 259.500, 269.100, 268.900, 279.700, 283.800, $
      283.600, 298.900]

res1 = POLY_FIT( days, flux, 3,  yfit)
res2 = POLY_FIT(DOUBLE(days),DOUBLE(flux), 3,  d_yfit)
;
expected=[-7322.1060, 3.1892600, -0.00046052801, 2.2604239e-08]
;
if ABS(res1[0]-expected[0]) GT 0.1 then ERRORS_ADD, errors, 'bad 0 value INT'
if ABS(res1[1]-expected[1]) GT 0.01 then ERRORS_ADD, errors, 'bad 1 value INT'
if ABS(res1[2]-expected[2]) GT 1e-5 then ERRORS_ADD, errors, 'bad 2 value INT'
if ABS(res1[3]-expected[3]) GT 1e-5 then ERRORS_ADD, errors, 'bad 3 value INT'
;
if ABS(res2[0]-expected[0]) GT 0.1 then ERRORS_ADD, errors, 'bad 0 value'
if ABS(res2[1]-expected[1]) GT 0.01 then ERRORS_ADD, errors, 'bad 1 value'
if ABS(res2[2]-expected[2]) GT 1e-5 then ERRORS_ADD, errors, 'bad 2 value'
if ABS(res2[3]-expected[3]) GT 1e-5 then ERRORS_ADD, errors, 'bad 3 value'
;
if KEYWORD_SET(verbose) then begin
   print, TRANSPOSE(res1)
   print, TRANSPOSE(res2)
   print, expected
   plot, days, flux, thick=2, /ynozero
   oplot, days, yfit, psym=-2, color=color2color(!color.red)
   oplot, days, d_yfit, psym=-2, color=color2color(!color.green_yellow)
endif
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_POLY_FIT_ZANARDO', errors, /short
ERRORS_CUMUL, cumul_errors, errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
pro TEST_POLY_FIT_RAMPES, cumul_errors, test=test, verbose=verbose
;
errors=0
eps=1e-4
;
x=INDGEN(10)
y=INDGEN(10)
for oo=1, 5 do begin
   expected=REPLICATE(0.,oo+1)
   expected[1]=1.
   res=POLY_FIT(x,y,oo)
   if KEYWORD_SET(verbose) then print, 'Res :', TRANSPOSE(res)
   if KEYWORD_SET(verbose) then print, 'Exp :', expected
   if (TOTAL(ABS(res-expected)) GT eps) then begin
      ERRORS_ADD, errors, STRCOMPRES('Cas 1 Order='+STRING(oo))
   endif
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_POLY_FIT_RAMPES', errors, /short
ERRORS_CUMUL, cumul_errors, errors
;
if KEYWORD_SET(test) then STOP
;
end
; ---------------------
;
pro TEST_POLY_FIT_ORDER3, cumul_errors, test=test, verbose=verbose
;
errors=0
;
; When GDL is called with /ran1 it gives in fact
; the same ramdom numbers that IDL (> 8.2) without /ran1 ...
;
; FL does provide same as IDL, but don't have /ran1
;
soft=GDL_IDL_FL()
if soft EQ 'GDL' then begin
   x = RANDOMN(33,256,/ran1)
   err=RANDOMN(44,256,/ran1)
endif else begin
   x = RANDOMN(33,256)
   err=RANDOMN(44,256)
endelse
;
y = 0.25+1.33*x-2.28*x*x+3.14*x^3+err
result = POLY_FIT(x, y, 3, measure_errors=err, chisq=chisq, yfit=yfit,$
                  covar=covar, yerror=yerror, sigma=sigma, yband=yband)
;
str_result=STRING(result, '(f12.6)')
expected=["    0.257944","    1.344486","   -2.280647","    3.133631"]
;
if KEYWORD_SET(verbose) then begin
   print, 'Computed : ', str_result
   print, 'Expected : ', expected
endif
;
if ~ARRAY_EQUAL(str_result,expected) then $
   ERRORS_ADD, errors, 'POLY_FIT TEST FAILED.'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_POLY_FIT_ORDER3', errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------
;
pro TEST_POLY_FIT, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_POLY_FIT, help=help, test=test, no_exit=no_exit, verbose=verbose'
   return
endif
;
cumul_errors = 0
;
TEST_POLY_FIT_ZANARDO, cumul_errors, test=test, verbose=verbose
;
TEST_POLY_FIT_RAMPES, cumul_errors, test=test, verbose=verbose
;
TEST_POLY_FIT_ORDER3, cumul_errors, test=test, verbose=verbose
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_POLY_FIT', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) or KEYWORD_SET(no_exit) then STOP
;
end

