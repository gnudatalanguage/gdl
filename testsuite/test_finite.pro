;
; under GNU GPL v2 or later
; Alain Coulais, 19-June-2012
; Mathieu Pinter, 19-June-2012
;
; ---------------------------------
; 
; Modifications history :
;
; - 2018-01-31 : AC. Revisiting TEST_FINITE_TIMETEST, counting the
;   fraction of detected NaN or Inf ...
; - 2018-02-05 : AC. Now working in GDL too (small bug in TOTAL solved)
;
; ---------------------------------
;
pro TEST_FINITE_BASIC, cumul_errors, type=type, test=test
;
nb_errors=0
;
if ~KEYWORD_SET(type) then type=4
all_types=[4,5,6,9]
;
ok=WHERE(all_types EQ type, nb_ok)
if (nb_ok NE 1) then begin
   ADD_ERROR, nb_errors, 'bad input type'
   print, 'Allow input types are : ', all_types
   ERRORS_CUMUL, cumul_errors, nb_errors
   return
endif
;
data=[0., -0.0, 10, -10., !values.f_nan, -!values.f_nan]
data=[data, !values.f_infinity, -!values.f_infinity]
;
if type EQ 5 then data=DOUBLE(data)
if type EQ 6 then data=COMPLEX(0.,0.)+data
if type EQ 9 then data=DCOMPLEX(0.,0.)+data
;
nb_errors=0
nb_cases=10
;
exp=BYTARR(nb_cases,8)
;
;expected results (general)
;
exp[0,*]=BYTE([1,1,1,1,0,0,0,0])
exp[1,*]=BYTE([0,0,0,0,1,1,0,0])
exp[2,*]=SHIFT(exp[1,*],2)
;exp[3,*]=exp[1,*]+exp[2,*]
;
; expected results for signs
;
exp[4,*]=BYTE([0,0,0,0,1,0,0,0])
exp[5,*]=BYTE([0,0,0,0,0,1,0,0])
exp[6,*]=BYTE([0,0,0,0,1,1,0,0])
;
exp[7,*]=BYTE([0,0,0,0,0,0,1,0])
exp[8,*]=BYTE([0,0,0,0,0,0,0,1])
exp[9,*]=BYTE([0,0,0,0,0,0,1,1])
;
; computations
;
res=BYTARR(10,8)
;
res[0,*]=FINITE(data)
res[1,*]=FINITE(data,/nan)
res[2,*]=FINITE(data,/inf)
;res[3,*]=FINITE(data,/nan,/inf)
;
res[4,*]=FINITE(data,/nan, sign=1)
res[5,*]=FINITE(data,/nan, sign=-1)
res[6,*]=FINITE(data,/nan, sign=0)
;
res[7,*]=FINITE(data,/inf, sign=1)
res[8,*]=FINITE(data,/inf, sign=-1)
res[9,*]=FINITE(data,/inf, sign=0)
;
for ii=0, nb_cases-1 do begin
   if ~ARRAY_EQUAL(exp[ii,*],res[ii,*]) then begin
      ADD_ERROR, nb_errors, 'problem with Case '+STRING(ii)
      
   endif
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_FINITE_BASIC', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------------
;
pro TEST_FINITE_TYPE, cumul_errors
;
nb_errors=0
;
byt_arr= BYTARR(10)
int_arr = INTARR(10)
uint_arr = UINTARR(10)
lon_arr = LONARR(10)
ulon_arr = ULONARR(10)
lon64_arr = LON64ARR(10)
ulon64_arr = ULON64ARR(10)
flt_arr = FLTARR(10)
dbl_arr = DBLARR(10)
comp_arr = COMPLEXARR(10)
dcomp_arr = DCOMPLEXARR(10)
str_arr = STRARR(10)
;struct_var = {name, tag:1}
;ptr_var = PTR_NEW(struct_var)
;list_var = LIST(1, 2)
;hash_var =HASH('id', 1664)
;
; You can only assign infinite and NaN values to :
; COMPLEX, DCOMPLEX, DOUBLE, FLOAT and STR
;
comp_arr[0] = !VALUES.F_NAN
dcomp_arr[1] = !VALUES.F_NAN
dbl_arr[2] = -!VALUES.F_NAN
flt_arr[3] = !VALUES.F_INFINITY
str_arr[4] = -!VALUES.F_INFINITY
;
nb_cases=12
;
exp=BYTARR(nb_cases,10)
;
;expected results (general)
;
exp[0,*]=BYTE([1,1,1,1,1,1,1,1,1,1])
exp[1,*]=BYTE([1,1,1,1,1,1,1,1,1,1])
exp[2,*]=BYTE([1,1,1,1,1,1,1,1,1,1])
exp[3,*]=BYTE([1,1,1,1,1,1,1,1,1,1])
exp[4,*]=BYTE([1,1,1,1,1,1,1,1,1,1])
exp[5,*]=BYTE([1,1,1,1,1,1,1,1,1,1])
exp[6,*]=BYTE([1,1,1,1,1,1,1,1,1,1])
exp[7,*]=BYTE([1,1,1,0,1,1,1,1,1,1])
exp[8,*]=BYTE([1,1,0,1,1,1,1,1,1,1])
exp[9,*]=BYTE([0,1,1,1,1,1,1,1,1,1])
exp[10,*]=BYTE([1,0,1,1,1,1,1,1,1,1])
exp[11,*]=BYTE([1,1,1,1,0,1,1,1,1,1])
;exp[12,*]=BYTE([1,1,1,1,1,1,1,1,1,1])
;exp[13,*]=BYTE([1,1,1,1,1,1,1,1,1,1])
;exp[14,*]=BYTE([1,1,1,1,1,1,1,1,1,1])
;exp[15,*]=BYTE([1,1,1,1,1,1,1,1,1,1])
;
; computations
;
res=BYTARR(12,10)
;
res[0,*]=FINITE(byt_arr)
res[1,*]=FINITE(int_arr)
res[2,*]=FINITE(uint_arr)
res[3,*]=FINITE(lon_arr)
res[4,*]=FINITE(ulon_arr)
res[5,*]=FINITE(lon64_arr)
res[6,*]=FINITE(ulon64_arr)
res[7,*]=FINITE(flt_arr)
res[8,*]=FINITE(dbl_arr)
res[9,*]=FINITE(comp_arr)
res[10,*]=FINITE(dcomp_arr)
res[11,*]=FINITE(str_arr)
;res[12,*]=FINITE(struct_var)
;res[13,*]=FINITE(ptr_var)
;res[14,*]=FINITE(list_var)
;res[15,*]=FINITE(hash_var)
;
for ii=0, nb_cases-1 do begin
   if ~ARRAY_EQUAL(exp[ii,*],res[ii,*]) then begin
      ADD_ERROR, nb_errors, 'problem with Case '+STRING(ii)
      
   endif
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_FINITE_TYPE', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------------
;
pro TEST_FINITE_VALUES, cumul_errors, test=test, verbose=verbose
;
txt="This platform does not distinguish between"
;
nb_errors = 0
;
tab=[[1.,!VALUES.F_INFINITY,1],$
     [0,!VALUES.F_NAN,5],$
     [-!VALUES.F_INFINITY,-!VALUES.F_NAN,100]]
;
a=FINITE(tab) 
if a[1,0] NE 0 then ADD_ERROR, nb_errors, 'case 1a'
;
a=FINITE(tab, /INF) 
if a[1,0] NE 1 then ADD_ERROR, nb_errors, 'case 1b'
;
a=FINITE(tab, /NAN) 
if a[1,1] NE 1 then ADD_ERROR, nb_errors, 'case 1c'
;
a=FINITE(tab, /INF, SIGN=1) 
if a[1,0] NE 1 then ADD_ERROR, nb_errors, 'case 1d'
;
a=FINITE(tab, /INF, SIGN=-1) 
if a[0,2] NE 1 then ADD_ERROR, nb_errors, 'case 1e'
;
a=FINITE(tab, /NAN, SIGN=1) 
if a[1,1] NE 1 then ADD_ERROR, nb_errors, 'case 1f'
;
a=FINITE(tab, /NAN, SIGN=-1) 
if a[1,2] NE 1 then ADD_ERROR, nb_errors, 'case 1g'
;
; ------------ testing NaN & Sign ----
;
b=FLTARR(1000)
b[10:19]=-!VALUES.F_NAN
b[110:119]=!VALUES.F_NAN
;
a=WHERE(FINITE(b, /NAN, SIGN=-1) EQ 1, count)
if count NE 10 then ADD_ERROR, nb_errors, 'case 2 (NaN & Sign)'
;
if count EQ 20 then MESSAGE, /continue, txt+" +Nan and -Nan"
;
; cross-check ! do we wrongly detect Inf ?
;
a=WHERE(FINITE(b, /inf, SIGN=-1) EQ 1, count)
if count NE 0 then ADD_ERROR, nb_errors, 'case 2 (Inf & Sign)'
;
; ------------ testing Inf & Sign ----
;
b[210:219]=!VALUES.F_infinity
b[410:419]=-!VALUES.F_infinity
;
a=WHERE(FINITE(b, /inf, SIGN=1) EQ 1, count)
if count NE 10 then ADD_ERROR, nb_errors, 'case 1'
if count EQ 20 then MESSAGE, /continue, txt+" +Inf and -Inf"
;
; now we have +-Nan and +inf, do we still know how to count ?
a=WHERE(FINITE(b, /NAN, SIGN=-1) EQ 1, count)
if count NE 10 then ADD_ERROR, nb_errors, 'case 1'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_FINITE_VALUES', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; -----------------------------
;
pro TEST_FINITE_TIMETEST, cumul_errors, nbp=nbp, infinity=infinity, $
                          test=test, verbose=verbose
;
errors=0
;
if ~KEYWORD_SET(nbp) then nbp=ULONG(1e5)
;
if nbp LT 1e4 then begin
   MESSAGE, /cont, "Don't be surprise if failed because"
   MESSAGE, /cont, "not enough points (statistical approach)"
endif
;
if KEYWORD_SET(verbose) then print, 'Using Nbps : ', nbp
if KEYWORD_SET(verbose) then print, 'Input array initialization started'
;
a=RANDOMU(seed, nbp)
;; we add 25% of +NaN and 25% of -NaN (total : 50%)
a[where(a GT 0.75)]=!values.f_nan
a[where(a LT 0.25)]=-!values.f_nan
;
expect_ratio=[0.5, 0.5, 0.25, 0.25, 0., 0., 0.]
;
if KEYWORD_SET(infinity) then begin
   ;;
   ;; we add 10% of +Inf and 10% of -Inf
   ;; but since it is equi-probable in normal and NaN parts,
   ;; we add 10% of non-finite elements --> 40% only are finite
   ;;
   a[1+LINDGEN(nbp/10-1)*10]=!values.f_infinity
   a[3+LINDGEN(nbp/10-1)*10]=-!values.f_infinity
   expect_ratio=[0.4, 0.4, 0.2, 0.2, 0.2, 0.1, 0.1]
endif
;
if KEYWORD_SET(verbose) then print, 'Input array initialization done'
;
time=FLTARR(N_ELEMENTS(expect_ratio))
ratio=FLTARR(N_ELEMENTS(expect_ratio))
;
; print, 'finite only'
t0=systime(1) & b=finite(a) & time[0]=systime(1)-t0 & ratio[0]=TOTAL(b,/dou)
; print, 'NaN only'
t0=systime(1) & b=finite(a, /nan) & time[1]=systime(1)-t0 & ratio[1]=TOTAL(b,/dou)
t0=systime(1) & b=finite(a, /nan, sign=1) & time[2]=systime(1)-t0 & ratio[2]=TOTAL(b)
t0=systime(1) & b=finite(a, /nan, sign=-1) & time[3]=systime(1)-t0 & ratio[3]=TOTAL(b)
; print, 'Inf only'
t0=systime(1) & b=finite(a, /inf) & time[4]=systime(1)-t0 & ratio[4]=TOTAL(b)
t0=systime(1) & b=finite(a, /inf, sign=1) & time[5]=systime(1)-t0 & ratio[5]=TOTAL(b)
t0=systime(1) & b=finite(a, /inf, sign=-1) & time[6]=systime(1)-t0 & ratio[6]=TOTAL(b)
;
ratio=FLOAT(ratio/DOUBLE(nbp))
erreur=ABS(ratio-expect_ratio)*100
flag=INTARR(N_ELEMENTS(expect_ratio))
pb=WHERE(erreur GT 1., nb_pb)
if (nb_pb GT 0) then flag[pb]=1
;
nb_errors=TOTAL(flag,/integer)
;
if KEYWORD_SET(verbose) then begin
   format='(A6, f9.5,A12, f7.4, A8, f7.4, a8, i3)'
   for ii=0, N_ELEMENTS(expect_ratio)-1 do begin
      print, format=format, 'time : ', time[ii], $
             ', Exp. Ratio : ', expect_ratio[ii], $
             ', Ratio : ', ratio[ii], ', flag = ',flag[ii]
   endfor
endif
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_FINITE_TIMETEST', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------
;
pro TEST_FINITE, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FINITE, help=help, test=test, $'
   print, '                 no_exit=no_exit, verbose=verbose'
   return
endif
;
cumul_errors=0
;
;BASIC_FINITE, data=data, comp=comp, double=double, test=test
;
TEST_FINITE_BASIC, cumul_errors, type=4
TEST_FINITE_BASIC, cumul_errors, type=5
TEST_FINITE_BASIC, cumul_errors, type=6
TEST_FINITE_BASIC, cumul_errors, type=9
;
TEST_FINITE_TYPE, cumul_errors
;
TEST_FINITE_VALUES, cumul_errors, test=test, verbose=verbose
;
print, 'Running this test with various numbers of points ...'
print, 'Please wait ...'
TEST_FINITE_TIMETEST, cumul_errors, verbose=verbose, nbp=10000L
TEST_FINITE_TIMETEST, cumul_errors, verbose=verbose, nbp=100000L
TEST_FINITE_TIMETEST, cumul_errors, verbose=verbose, nbp=1000000L
TEST_FINITE_TIMETEST, cumul_errors, verbose=verbose, nbp=10000000L
TEST_FINITE_TIMETEST, cumul_errors, verbose=verbose, nbp=100000000L
;
; with the /Infinity Keyword
;
print, 'Same test with various numbers of points and /Inf...'
TEST_FINITE_TIMETEST, cumul_errors, verbose=verbose, /infi, nbp=10000L
TEST_FINITE_TIMETEST, cumul_errors, verbose=verbose, /infi, nbp=100000L
TEST_FINITE_TIMETEST, cumul_errors, verbose=verbose, /infi, nbp=1000000L
TEST_FINITE_TIMETEST, cumul_errors, verbose=verbose, /infi, nbp=10000000L
TEST_FINITE_TIMETEST, cumul_errors, verbose=verbose, /infi, nbp=100000000L
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_FINITE', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
