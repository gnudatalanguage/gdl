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
; ---------------------
;
pro TEST_POLY_FIT, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if keyword_set(help) then begin
   print, 'pro TEST_POLY_FIT, help=help, test=test, no_exit=no_exit, verbose=verbose'
   return
endif

cumul_errors = 0
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
   ERRORS_ADD, cumul_errors, 'POLY_FIT TEST FAILED.'
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

