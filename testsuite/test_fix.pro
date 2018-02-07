;
; Alain C, 31 july 2008, under GNU GPL v2 or later
;
; After bugs founded by Nicolas with negatives Real/Double
; and (D)Complex with type in (12,13,15) we do a
; systematic check for potential FIX() problems
;
; Please remember type ULONG64 does not exist on x86
;
; Plateforms dependances:
; --linux IDL 7.0 x86_64 7 tests, 0 errors on 4 cases
; --linux IDL 7.0 x86    6 tests, 0 errors on 3 cases, 2 errors on
; last (INT and BYTE)
; --linux IDL 5.5 x86    6 tests, 0 errors on 4 cases
; --linux IDL 6.0 x86    6 tests, 0 errors on 3 cases, 2 errors on last (INT and BYTE)
;
; ----------------------------------------------------
; Modifications history :
;
; 2018-Feb-05 : AC. during TEST_FINITE rewriting, discovered some FIX() 
;  for NaN and Inf are clearly different in IDL and GDL
;
; ----------------------------------------------------
;
; http://www.harrisgeospatial.com/docs/idl_data_types.html
;
pro TEST_FIX_NAN_INF, cumul_erros, test=test, verbose=verbose
;
nb_errors=0
;
data=[0., -0.0, 10, -10., !values.f_nan, -!values.f_nan]
data=[data, !values.f_infinity, -!values.f_infinity]
;
GIVE_LIST_NUMERIC, list_num_types, list_num_names, /integer
;
l31=(2L^30)*2L
l63=(2lL^62)*2lL
ull63=2ull^63
;
exp_BYTE=BYTE([0, 0, 10, 246, 0, 0, 0, 0])
;
exp_INT=[0s, 0, 10, -10, 0, 0, 0, 0]
;
exp_LONG=[0l, 0, 10, -10, l31, l31, l31, l31]
;
; not ready
exp_FLOAT=data
exp_DOUBLE=data*1.D
exp_COMPLEX=data*COMPLEX(1.,0.)
exp_DCOMPLEX=data*DCOMPLEX(1.,0.)
;
exp_UINT=MAKE_ARRAY(8, type=12)
exp_UINT[2:3]=[10u,65526u]
;
exp_ULONG=MAKE_ARRAY(8, type=13)
exp_ULONG[2:3]=[10ul,4294967286ul]
;
exp_LONG64=MAKE_ARRAY(8, type=14)
exp_LONG64[2:3]=[10ll,-10ll]
exp_LONG64[4:7]=REPLICATE(l63,4)
;
exp_ULONG64=MAKE_ARRAY(8, type=15)
exp_ULONG64[2:3]=[10ull,-10ull]
exp_ULONG64[4:7]=REPLICATE(l63,4)
exp_ULONG64[6]=0ull
;
for ii=0,N_ELEMENTS(list_num_names)-1 do begin
   ;;
   type_value=list_num_types[ii]
   type_name=list_num_names[ii]
   ;;
   res=FIX(data, type=type_value)
   ;;
   OK=EXECUTE('exp=exp_'+type_name)
   if OK then begin
      if ~ARRAY_EQUAL(res, exp) then begin
         ADD_ERROR,  nb_errors, 'pb with type : '+type_name
         print, 'exp :', exp
         print, 'res :', res
      endif
      if KEYWORD_SET(verbose) then begin
         MESSAGE, /continue, 'values for type :'+type_name
         print, 'exp :', exp
         print, 'res :', res
      endif
   endif else begin
      print, 'pb during name conversion for type : '+STRUPCASE(type_name)
   endelse   
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_FIX_NAN_INF', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end

pro TEST_FIX_UNSIGNED, value, type, count=count, quiet=quiet
;
resu=FIX(value, type=type)-FIX(ROUND(value), type=type)
;
if resu NE 0 then begin
   if NOT(KEYWORD_SET(quiet)) then begin
      print, 'Error for Value : ', value
      HELP, value
      HELP, FIX(value, type=type)
      HELP, FIX(ROUND(value), type=type)
   endif
   if (N_ELEMENTS(count) EQ 1) then count=count+1
endif
end
;
; -------------------------------------
;
pro TEST_FIX_ONE_VALUE, cumul_errors, value=value, verbose=verbose
;;, all_types=all_types
;
if KEYWORD_SET(verbose) then quiet=0 else quiet=1

; type  7: string
; type  8: structure
; type 10: pointer
; type 11: object
liste_types=[1,2,3,12,13,14]
;
if (!version.arch EQ 'x86_64') OR KEYWORD_SET(all_types) then begin
   liste_types=[liste_types,15]
endif
;
nb_types=N_ELEMENTS(liste_types)
;
if N_ELEMENTS(value) EQ 0 then x=10.1 else x=value

if (x LT 0.) then begin
   print, 'Positive input mandatory !'
;   return, 0
endif
;
separateur= '---------------------------'
print, separateur
print, 'Tested value : ', x
;
;  --------- positive case -----------------
;
print, separateur
print, 'Positive input'
;
count=0
for ii=0, nb_types-1 do begin
   TEST_FIX_UNSIGNED, x, liste_types[ii], count=count, quiet=quiet
end

print, 'Number of tests done :', nb_types
if count EQ 0 then begin
   print, 'No errors founded'
endif else begin
   print, 'Number of errors', count   
endelse
errors_pos=count
;
;  --------- negative case -----------------
;
print, separateur
print, 'Negative input'
;
count=0
for ii=0, nb_types-1 do begin
   TEST_FIX_UNSIGNED, (-1.0)*x, liste_types[ii], count=count, quiet=quiet
end
;
print, 'Number of tests done :', nb_types
if count EQ 0 then begin
   print, 'No errors founded'
endif else begin
   print, 'Number of errors', count
endelse

errors_neg=count
if (errors_pos GT 0) OR (errors_neg GT 0) then begin
   print, separateur
   print, 'Bad news ! At least one problem encoutered !'
   print, 'Please rerun this test with keyword /verbose !'
   ;;return, 0
endif

;;return, 1
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_FIX', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ----------------------------------------------
;
pro TEST_FIX, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FIX, help=help, test=test, $'
   print, '              no_exit=no_exit, verbose=verbose'
   return
endif
;
cumul_errors=0

TEST_FIX_ONE_VALUE, cumul_errors, value=1.001, verbose=verbose
TEST_FIX_ONE_VALUE, cumul_errors, value=10.1, verbose=verbose
TEST_FIX_ONE_VALUE, cumul_errors, value=1000.1, verbose=verbose
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_FIX', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
