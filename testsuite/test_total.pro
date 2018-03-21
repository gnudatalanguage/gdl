;
; Initial author : P. Chanial 2006.01.17
; Code under GNU GPL V2 or later
;
; NOTA BENE : AC, 9 Feb. 2018 : unfinished modifs and extensions
; Some changes need in source code (basic_fun.cpp)
;
; ---------------------------------------
; Modifications history :
;
; - 2018-FEB-04 : AC. Corrected "bug" when cumulating large number
;   of byte ... large rewriting, trying to automatic
;   and to have numerical cases at the limits ...
;
; ---------------------------------------
; Script : regression-total
pro regression, a
;
; AC 2018 March : this part of the test should be recycle ... TODO !!!
;

print, 'Testing '+size(a, /tname)+':'
; help, total(a), total(a, /NaN), total(a, /cumul), total(a, /cumul, /NaN), output = output
; print, output
; help, total(a, /double), total(a, /NaN, /double), total(a, /cumul, /double), total(a, /cumul, /NaN, /double), output=output
; print, output
; help, total(a, 1), total(a, 1, /NaN), total(a, 1, /cumul), total(a, 1, /cumul, /NaN), output=output
; print, output
; help, total(a, 1, /double), total(a, 1, /NaN, /double), total(a, 1, /cumul, /double), total(a, 1, /cumul, /NaN, /double), output=output
; print, output
; help, total(a, 2), total(a, 2, /NaN), total(a, 2, /cumul), total(a, 2, /cumul, /NaN), output=output
; print, output
; help, total(a, 2, /double), total(a, 2, /NaN, /double), total(a, 2, /cumul, /double), total(a, 2, /cumul, /NaN, /double), output=output
; print, output
; help, total(a, 3), total(a, 3, /NaN), total(a, 3, /cumul), total(a, 3, /cumul, /NaN), output=output
; print, output
; help, total(a, 3, /double), total(a, 3, /NaN, /double), total(a, 3, /cumul, /double), total(a, 3, /cumul, /NaN, /double), output=output
; print, output

 print, '-- 1' & print, total(a)
 print, '-- 2' & print, total(a, /NaN)
 print, '-- 3' & print, total(a, /cumul)
 print, '-- 4' & print, total(a, /cumul, /NaN)
 print, '-- 5' & print, total(a, /double)
 print, '-- 6' & print, total(a, /NaN, /double)
 print, '-- 7' & print, total(a, /cumul, /double)
 print, '-- 8' & print, total(a, /cumul, /NaN, /double)

 print, '==========================================='
 print, '-- 9' & print, total(a, 1)
 print, '-- 10' & print, total(a, 1, /NaN)
 print, '-- 11' & print, total(a, 1, /cumul)
 print, '-- 12' & print, total(a, 1, /cumul, /NaN)
 print, '-- 13' & print, total(a, 1, /double)
 print, '-- 14' & print, total(a, 1, /NaN, /double)
 print, '-- 15' & print, total(a, 1, /cumul, /double)
 print, '-- 16' & print, total(a, 1, /cumul, /NaN, /double)
 
 print, '==========================================='

 print, '-- 17' & print, total(a, 2)
 print, '-- 18' & print, total(a, 2, /NaN)
 print, '-- 19' & print, total(a, 2, /cumul)
 print, '-- 20' & print, total(a, 2, /cumul, /NaN)
 print, '-- 21' & print, total(a, 2, /double)
 print, '-- 22' & print, total(a, 2, /NaN, /double)
 print, '-- 23' & print, total(a, 2, /cumul, /double)
 print, '-- 24' & print, total(a, 2, /cumul, /NaN, /double)

 print, '==========================================='

 print, '-- 25' & print, total(a, 3)
 print, '-- 26' & print, total(a, 3, /NaN)
 print, '-- 27' & print, total(a, 3, /cumul)
 print, '-- 28' & print, total(a, 3, /cumul, /NaN)
 print, '-- 29' & print, total(a, 3, /double)
 print, '-- 30' & print, total(a, 3, /NaN, /double)
 print, '-- 31' & print, total(a, 3, /cumul, /double)
 print, '-- 32' & print, total(a, 3, /cumul, /NaN, /double)

end

pro TEST_TOTAL_NAN_INF, cumul_errors, test=test, verbose=verbose
;
print, 'AC 2018 March : this part of the test should be recycle ... TODO !!!'

BANNER_FOR_TESTSUITE, pref='TEST_TOTAL_NAN_INF', 'unfinished !', 1000
; unfinished !
return

;; we need a way to know whether IDL or GDL is running...
DEFSYSV, '!gdl', exists=isGDL

;filename = (isGDL?'gdl':'idl'+strtrim(!version.release,1))+$
;           '-test-total.txt'
;journal, filename
 
 a = bytarr(3, 3, 3)+1b
 regression, a
 
 a = findgen(3, 3, 3)
 a[0,0,0] = !values.f_nan
 a[1,1,0] = !values.f_infinity
 a[2,1,0] = -!values.f_infinity
 a[0,2,1] = !values.f_nan
 a[2,0,2] = !values.f_nan
 regression, a

 a = dindgen(3, 3, 3)
 a[0,0,0] = !values.d_nan
 a[0,1,1] = !values.d_infinity
 a[2,0,1] = -!values.d_infinity
 a[0,2,1] = !values.d_nan
 a[2,2,2] = !values.d_nan
 regression, a

 a = complexarr(3, 3, 3)+1
 a[0,0,0] = complex(3, !values.f_nan)
 a[1,1,0] = complex(!values.f_nan, !values.f_infinity)
 a[2,1,0] = complex(0, -!values.f_infinity)
 a[0,2,1] = complex(78, !values.f_nan)
 a[2,0,2] = complex(!values.f_infinity, 40)
 regression, a
 
 a = dcomplexarr(3, 3, 3)+1
 a[0,0,0] = complex(3, !values.d_nan)
 a[1,1,0] = complex(!values.d_nan, !values.d_infinity)
 a[2,1,0] = complex(0, -!values.d_infinity)
 a[0,2,1] = complex(78, !values.d_nan)
 a[2,0,2] = complex(!values.d_infinity, 40)
 regression, a

; journal
 
end
;
; -----------------------------------------------------------------
;
pro TEST_TOTAL_LARGE, cumul_errors, test=test, verbose=verbose
;
;BANNER_FOR_TESTSUITE, pref='TEST_TOTAL_LARGE', 'unfinished !', 1000
;return
;
errors=0

kk=1
for ii=0, 7 do begin
   ;;
   internal=0
   ;;
   kk=10L*kk
   a=LINDGEN(kk)
   res1=TOTAL([a,-a],/int)
   res2=TOTAL([a,-a])
   tmp=TOTAL([a,-a],/int,/cumul)
   res3=tmp[-1] ; last value
   tmp=TOTAL([a,-a],/cumul);,/dou)
   res4=tmp[-1] ; last value
   ;;
   if ~ARRAY_EQUAL(0ll, res1, /no_type) then $
      ERRORS_ADD, internal, 'res 1, kk='+string(kk)
   if ~ARRAY_EQUAL(0ll, res3, /no_type) then $
      ERRORS_ADD, internal, 'res 3, kk='+string(kk)
   if ~ARRAY_EQUAL(0., res2, /no_type) then $
      ERRORS_ADD, internal, 'res 2, kk='+string(kk)
   ;; we cannot test on "res4", just to show the problem
   if KEYWORD_SET(verbose) then print, kk, res1, res2, res3, res4
   ;;
   ERRORS_CUMUL, errors, internal
   ;;
endfor
;
; https://sourceforge.net/p/gnudatalanguage/bugs/593/
; showing Float conversion tricks ...
; (computations are OK, print seems to be not OK if bad format)
;
res_f=17865335.
res_d=17865335d
res_l=17865335l
res_l64=17865335ll
;
a=[8926507,8938828,0]

if ~ARRAY_EQUAL(TOTAL(a), res_f, /no_type) then $
      ERRORS_ADD, errors, 'case 3 Long to Float'
if ~ARRAY_EQUAL(TOTAL(a,/double), res_d, /no_type) then $
      ERRORS_ADD, errors, 'case 3 Long to Double'
if ~ARRAY_EQUAL(TOTAL(a,/int), res_l64, /no_type) then $
      ERRORS_ADD, errors, 'case 3 Long to /int Long64'
if ~ARRAY_EQUAL(TOTAL(a,/preserve), res_l, /no_type) then $
      ERRORS_ADD, errors, 'case 3 Long to /pre Long'
;
; playing with big values in "special" type
;
grand=80000000ll
;
; res=grand*(grand-1)/2
;
res_f=3199999960000000.
res_d=3199999960000000.d
res_l64=3199999960000000
res_ul=216376832ul

if ~ARRAY_EQUAL(TOTAL(ULINDGEN(grand)), res_f, /no_type) then $
         ERRORS_ADD, errors, 'case default in ULong'
if ~ARRAY_EQUAL(TOTAL(ULINDGEN(grand), /double), res_d, /no_type) then $
         ERRORS_ADD, errors, 'case ULong Double'
if ~ARRAY_EQUAL(TOTAL(ULINDGEN(grand),/pres), res_ul, /no_type) then $
         ERRORS_ADD, errors, 'case ULong ULong'
if ~ARRAY_EQUAL(TOTAL(ULINDGEN(grand),/int), res_l64, /no_type) then $
         ERRORS_ADD, errors, 'case ULong Long64'
;
; cf mail @ Gilles
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_TOTAL_LARGE", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------
; this code fails in IDL : 
; * Long64 & ULong64 should be converted to double in default case
; this code fails in GDL : 
; * Long64 & ULong64 should be converted to double in default case
;
pro TEST_TOTAL_INT, cumul_errors, test=test, verbose=verbose, debug=debug
;
offset=201b
nbps=0
;
dims=[3,4,5]
muldims=PRODUCT(dims, /integer)
in=BYTARR(dims)
in=in+offset
;
GIVE_LIST_NUMERIC, list_num_types, list_num_names, /integer
;
exp1f_ref=FLOAT(muldims)*FLOAT(offset)
exp1int_ref=muldims*LONG64(offset)
;
for ii=0, N_ELEMENTS(list_num_types)-1 do begin
   ;;
   internal=nbps
   ;;
   type=list_num_types[ii]
   name=list_num_names[ii]
   input=FIX(in, type=type)
   exp1pres=FIX(exp1int_ref, type=type)
   res1f=TOTAL(input)
   res1int=TOTAL(input,/int)
   res1pres=TOTAL(input,/preserve)
   ;;
   ;; few special cases
   exp1f=exp1f_ref
   if type GE 14 then exp1f=DOUBLE(exp1f_ref)
   exp1int=exp1int_ref
   if type EQ 15 then exp1int=ULONG64(exp1int_ref)
   ;;
   if KEYWORD_SET(verbose) then begin
      print, 'Exp: ', name, exp1f, exp1int, exp1pres
      print, 'Res: ', name, res1f, res1int, res1pres
   endif
   if ~ARRAY_EQUAL(res1f, exp1f, /no_type) then $
      ERRORS_ADD, nbps, 'res 1 F Type='+name
   if ~ARRAY_EQUAL(res1int, exp1int, /no_type) then $
      ERRORS_ADD, nbps, 'res 1 /Int Type='+name
   if ~ARRAY_EQUAL(res1pres, exp1pres, /no_type) then $
      ERRORS_ADD, nbps, 'res 1 /Pres Type='+name
   ;;
   if KEYWORD_SET(debug) then if nbps GT internal then stop  
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_TOTAL_INT', nbps, /short
ERRORS_CUMUL, cumul_errors, nbps
if KEYWORD_set(test) then STOP
;
end
;
; -----------------------------------------------------------------
;
pro TEST_TOTAL, help=help, test=test, verbose=verbose, no_exit=no_exit
;
if KEYWORD_SET(test) then begin
   print, 'pro TEST_TOTAL, help=help, test=test, verbose=verbose, no_exit=no_exit'
   return
endif 
;
cumul_errors=0
;
TEST_TOTAL_NAN_INF, cumul_errors, test=test, verbose=verbose
;
TEST_TOTAL_LARGE, cumul_errors, test=test, verbose=verbose
;
TEST_TOTAL_INT, cumul_errors, test=test, verbose=verbose
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_TOTAL', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
