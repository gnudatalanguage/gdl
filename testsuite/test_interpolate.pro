;
; AC, 2018-FEB-01 : merging two files related to INTERPOLATE,
; looking back in bugs report history (bugs 98, 
;
; Modifications history :
;
; ----------------------------------------------------
;  
; by GD date	2012.09.07
pro TEST_BUG_3483402, cumul_errors, test=test
;
errors=0
;
good1=[[[1.41421,1.41421,1.41421],[ 2.23607,2.23607,2.23607], $
        [ 3.16228,3.16228,3.16228],[ 4.12311,4.12311,4.12311]], $
       [[ 2.23607,2.23607,2.23607],[ 2.82843,2.82843,2.82843],$
        [ 3.60555,3.60555,3.60555],[ 4.47214,4.47214,4.47214]], $
       [[ 3.16228,3.16228,3.16228],[ 3.60555,3.60555,3.60555],$
        [ 4.24264,4.24264,4.24264],[ 5.00000,5.00000,5.00000]], $
       [[ 2.23607,2.23607,2.23607],[ 2.82843,2.82843,2.82843],$
        [ 3.60555,3.60555,3.60555],[ 4.47214,4.47214,4.47214]]]
;
good2=[[[3.75318,3.75318,3.75318,3.75318,3.75318],$
        [5.53523,5.53523,5.53523,5.53523,5.53523]],$
       [[4.21408,4.21408,4.21408,4.21408,4.21408],$
        [5.85982,5.85982,5.85982,5.85982,5.85982]]]
;
b=FLTARR(5,17,6)
;
b[0,*,*]=DIST(17,6)
b[1,*,*]=DIST(17,6)
b[2,*,*]=DIST(17,6)
b[3,*,*]=DIST(17,6)
b[4,*,*]=DIST(17,6)
;
c1= INTERPOLATE(b, [0,1,2], [1,2,3,4],[1,2,3,4],/gri)
c2= INTERPOLATE(b, [3.3,5.255],[4.2,2.55],/gri,cubic=-0.3)
;
if (TOTAL(c1-good1)/4/4/3 ge 1E-4) then ADD_ERROR, errors, 'problem within C1'
if (TOTAL(c2-good2)/5/2/2 ge 1E-4) then ADD_ERROR, errors, 'problem within C2'
;
; ----------
;
BANNER_FOR_TESTSUITE, 'TEST_BUG_3483402', errors, /short
;
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------------------------------
; https://sourceforge.net/p/gnudatalanguage/bugs/458/
; seems to be fully equivalent to what was reported before in bug 98
; https://sourceforge.net/p/gnudatalanguage/bugs/98/
;
pro TEST_BUG_458, cumul_errors, test=test
;
errors=0
;
b=FLTARR(3,128,128)
b[0,*,*]=DIST(128)
;
u=dist(12)
expected=FLTARR(3,4,4)
expected[0,*,*]=u[1:4,1:4]
;
result=INTERPOLATE(b, [0,1,2], [1,2,3,4], [1,2,3,4],/gri)
;
tol=1e-6
;
if (TOTAL(ABS(result-expected)) GT tol) then ADD_ERROR, errors, 'pb 3D'
;
; ----------
;
BANNER_FOR_TESTSUITE, 'TEST_BUG_458', errors, /short
;
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------------------------------
; https://sourceforge.net/p/gnudatalanguage/bugs/223/
;
pro TEST_BUG_223, cumul_errors, test=test
;
errors=0
;
p = FINDGEN(4,4)
;
res1=INTERPOLATE(p, [.5, 1.5, 2.5], [.5, 1.5, 2.5], /GRID) 
res2=INTERPOLATE(p, [.5, 1.5, 2.5], [.5, 1.5, 2.5]) 
res3=INTERPOLATE(p, [.5, 1.5, 2.5])
;
exp1=p[0:2,0:2]+2.5
exp2=[2.5, 7.5, 12.5]
exp3=FINDGEN(4,3)+2.
;
tol=1e-6
if (TOTAL(ABS(res1-exp1)) GT tol) then ADD_ERROR, errors, 'pb res1 (value)'
if (TOTAL(ABS(res2-exp2)) GT tol) then ADD_ERROR, errors, 'pb res2 (value)'
if (TOTAL(ABS(res2-exp2)) GT tol) then ADD_ERROR, errors, 'pb res3 (value)'
;
if ~ARRAY_EQUAL(SIZE(res1),SIZE(exp1)) then ADD_ERROR, errors, 'pb res1 (dim)'
if ~ARRAY_EQUAL(SIZE(res2),SIZE(exp2)) then ADD_ERROR, errors, 'pb res2 (dim)'
if ~ARRAY_EQUAL(SIZE(res3),SIZE(exp3)) then ADD_ERROR, errors, 'pb res3 (din)'
;
BANNER_FOR_TESTSUITE, 'TEST_BUG_223', errors, /short
;
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------------------------------
;
pro TEST_INTERPOLATE_MISSING, cumul_errors, test=test
;
errors=0
;
a = INTERPOLATE([1,3],[-1,0,1,2], missing=-10)
if ((a[0] ne -10) || (a[3] ne -10)) then ADD_ERROR, errors, 'Case 1'
;
a = INTERPOLATE([1,3,4],[-1,0,1,3], missing=-10, /cubic)
if ((a[0] ne -10) || (a[3] ne -10)) then ADD_ERROR, errors, 'Case 2'
;
BANNER_FOR_TESTSUITE, 'TEST_INTERPOLATE_MISSING', errors, /short
;
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------------------------------
;
; Testing the type of the output of INTERPOLATE()
; It should be the same than the input !!!
;
pro TEST_INTERPOLATE_TYPE, cumul_errors, test=test
;
errors=0
;
GIVE_LIST_NUMERIC, list_num_types, list_num_names
;
the_value=1.2
;
for ii=0,N_ELEMENTS(list_num_names)-1 do begin
   ;;
   type_value=list_num_types[ii]
   type_name=list_num_names[ii] 
   ;;
   res=INTERPOLATE(INDGEN(5, type=type_value), the_value)
   exp=FIX(the_value, type=type_value)
   ;;
   if ~ARRAY_EQUAL(exp, res,/no_typeconv) then ADD_ERROR, errors, 'bad for type '+type_name
endfor
;
BANNER_FOR_TESTSUITE, 'TEST_INTERPOLATE_TYPE', errors, /short
;
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------------------------------
;
pro TEST_INTERPOLATE, help=help, verbose=verbose, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_INTERPOLATE, help=help, verbose=verbose, test=test, no_exit=no_exit'
    return
endif
;
cumul_errors=0
;
TEST_BUG_3483402, cumul_errors
TEST_BUG_458, cumul_errors
TEST_BUG_223, cumul_errors
TEST_INTERPOLATE_TYPE, cumul_errors
TEST_INTERPOLATE_MISSING, cumul_errors
;
; ----------------- final message ----------
BANNER_FOR_TESTSUITE, 'TEST_INTERPOLATE', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

