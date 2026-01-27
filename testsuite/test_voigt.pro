;
; Note by AC 2025-Dec-21 : This code is one of the oldest tests we
; wrote for the testsuite/. For sure we improved the mechanisc over the
; time !
;
; NG  27/05/2008
; test procedures for Intrinsic VOIGT() function
; If you find bugs, limitations, other interresting cases,
; please report them to Alain Coulais : alaingdl@users.sourceforge.net
;
; performance : on my laptop, 2025 TEST_VOIGT_TIME
; gdl 1.1.3 : 0.25 s / IDL 8.8 : 1.69 s / FL 0.79.53 : 3.36 s
;--------------------------------------------------------------------
;
;	I-Compatibility with IDL syntax 
;	II-Plot of voigt function
;	III-Comparison with Armstrong et al. VOIGT values
;	IV-Test with negative numbers 
;	V-Test with not a number and infinite values
;
; ---------------------------------------
; Modifications history :
;
; - 2025-Dec-21 : AC. largely rewrite to have a true test.
;   The motivation is to be ready for enforce chanegs in the C++ code
;   (moderm c++ tests with std:isfinite)
;
;-----------------------------------------------------------------------
; 		I-Compatibility with IDL syntax
;-----------------------------------------------------------------------
pro TEST_VOIGT_ARRAY_DIMS, cumul_errors, test=test
;
errors=0
eps=1e-6
;
;1. If both arguments are scalars, the function returns a scalar'
;
a=1.705
u=0.04
res=VOIGT(A,U)
expect=0.29088697
if (ABS(res-expect) GT eps) then ERRORS_ADD, errors, '1 numeric'
if ~ISA(res, /FLOAT) then ERRORS_ADD, errors, '1 type'
if ~ARRAY_EQUAL(SIZE(res), [0,4,1]) then ERRORS_ADD, errors, '1 size'
;
;2. "arguments are both arrays"
; 2-1."With same dimensions"
;
a=[ 5 , 4 , 3.22 , 1.0005 ]
u=[ 1.002 , 2.003 , 5.106 , 9.0025 ]
res=VOIGT(A,U)
expect=[0.10678264, 0.11207753, 0.051142626, 0.0070074787]
if (TOTAL(ABS(res-expect)) GT 4.*eps) then ERRORS_ADD, errors, '2.1 numeric'
if ~ISA(res, /FLOAT) then ERRORS_ADD, errors, '2.1 type'
if ~ARRAY_EQUAL(SIZE(res), SIZE(expect)) then ERRORS_ADD, errors, '2.1 size'
;
; 2-2."With different dimensions"
a=[ 5 , 3.22 , 1.0005 ]
u=[ 1.002 , 2.003 , 5.106 , 9.0025 ]
res=voigt(A,U)
expect=[0.10678264, 0.126405, 0.0220518]
if (TOTAL(ABS(res-expect)) GT 3.*eps) then ERRORS_ADD, errors, '2.2a numeric'
if ~ISA(res, /FLOAT) then ERRORS_ADD, errors, '2.2a type'
if ~ARRAY_EQUAL(SIZE(res), SIZE(expect)) then ERRORS_ADD, errors, '2.2a size'
;
a=[ 5, 3.22 , 1.0005 ]
u=[1.002, 2.003] 
res=voigt(a,u)
expect=[0.10678264, 0.126405]
if (TOTAL(ABS(res-expect)) GT 2.*eps) then ERRORS_ADD, errors, '2.2b numeric'
if ~ISA(res, /FLOAT) then ERRORS_ADD, errors, '2.2b type'
if ~ARRAY_EQUAL(SIZE(res), SIZE(expect)) then ERRORS_ADD, errors, '2.2b size'
;
;3. "One argument is a scalar or one element array and the other is an array
; 3.1a A array, U singleton
a=[5 , 3.22 , 1.0005 ]
u=5.00 
res=VOIGT(a,u)
expect=[0.0569654, 0.0527041, 0.0230136]
if (TOTAL(ABS(res-expect)) GT 3.*eps) then ERRORS_ADD, errors, '3.1a numeric'
if ~ISA(res, /FLOAT) then ERRORS_ADD, errors, '3.1a type'
if ~ARRAY_EQUAL(SIZE(res), SIZE(expect)) then ERRORS_ADD, errors, '3.1a size'
;
; 3.1b A array, U singleton but array
a=[5 , 3.22 , 1.0005 ]
u=[5.00]
res=VOIGT(a,u)
expect=[0.0569654]
if (TOTAL(ABS(res-expect)) GT eps) then ERRORS_ADD, errors, '3.1b numeric'
if ~ISA(res, /FLOAT) then ERRORS_ADD, errors, '3.1b type'
if ~ARRAY_EQUAL(SIZE(res), SIZE(expect)) then ERRORS_ADD, errors, '3.1b size'
;
; 3.2a A singleton, U array
a=4.002 
u=[ 5.00 , 2.003 , 4 , 6 ]
res=VOIGT(a,u)
expect=[0.0560025, 0.112043, 0.0715688, 0.0441488]
if (TOTAL(ABS(res-expect)) GT 4.*eps) then ERRORS_ADD, errors, '3.2a numeric'
if ~ISA(res, /FLOAT) then ERRORS_ADD, errors, '3.2a type'
if ~ARRAY_EQUAL(SIZE(res), SIZE(expect)) then ERRORS_ADD, errors, '3.2a size'
;
; 3.2b A array with one element, U array
a=[4.002]
u=[ 5.00 , 2.003 , 4 , 6 ]
res=VOIGT(a,u)
expect=[0.0560025]
if (TOTAL(ABS(res-expect)) GT 4.*eps) then ERRORS_ADD, errors, '3.2b numeric'
if ~ISA(res, /FLOAT) then ERRORS_ADD, errors, '3.2b type'
if ~ARRAY_EQUAL(SIZE(res), SIZE(expect)) then ERRORS_ADD, errors, '3.2b size'
;
; 4 : more general test without detail on numbers
;
a=REPLICATE(1., [2,3])
u2d=TRANSPOSE(a)
expect=0.30474421
eps=5.e6
;
res=VOIGT(a,u2d)
if (TOTAL(ABS(res-expect)) GT eps) then ERRORS_ADD, errors, '4a numeric'
if ~ISA(res, /FLOAT) then ERRORS_ADD, errors, '4a type'
if ~ARRAY_EQUAL(SIZE(res), SIZE(a)) then ERRORS_ADD, errors, '4a size'
;
res=VOIGT(u2d,a)
if (TOTAL(ABS(res-expect)) GT eps) then ERRORS_ADD, errors, '4b numeric'
if ~ISA(res, /FLOAT) then ERRORS_ADD, errors, '4b type'
if ~ARRAY_EQUAL(SIZE(res), SIZE(u2d)) then ERRORS_ADD, errors, '4b size'
;
; no more idea ...
;
BANNER_FOR_TESTSUITE, 'TEST_VOIGT_ARRAY_DIMS', errors, /status
ERRORS_CUMUL, cumul_errors, errors
;
if KEYWORD_SET(test) then STOP
;
end
;
;-----------------------------------------------------------------------
; 	 	 II-Plot of voigt function
; AC 2025 : maybe plotting comparisaon/normalzation with GAUSSIAN
;-----------------------------------------------------------------------
pro TEST_VOIGT_PLOT

x=findgen(2000)/10-100
a=[0.01, 0.1, 0.25, .5, 1., 2., 4., 8., 16., 32., 64., 128.]
nb_a=N_ELEMENTS(a)

window, 0, xsize=800, ysize=600
!p.multi=[0,3,4]
txt='VOIGT function with damper = '
for ii=0, nb_a-1 do begin  
    plot, x, VOIGT(a[ii],x), title = txt+ STRING( a[ii])
end 
!p.multi=0
;
end
;
;-----------------------------------------------------------------------
;  		III-Comparison with Armstrong et al. VOIGT values
; (Values computed in regions where accuracy is challenging)
;-----------------------------------------------------------------------
pro TEST_VOIGT_ArmstrongValues, cumul_errors, verbose=verbose, test=test
;
errors=0

soft=GDL_IDL_FL()
;
eps1=1e-14
res1=VOIGT(1e-10,5.4)
exp1=2.260842e-12
if (ABS(res1-exp1) GT eps1) then ERRORS_ADD, errors, 'Asmst. Numeric 1'
;
eps2=1e-16
res2=VOIGT(1e-14,5.5)
exp2=7.307387e-14
if (ABS(res2-exp2) GT eps2) then ERRORS_ADD, errors, 'Asmst. Numeric 2'
;
if KEYWORD_SET(verbose) then begin
   print,'-------------------------------------------------'
   print,'Comparison between Armstrong and '+soft+' VOIGT values'
   print,'-------------------------------------------------'
   print,'. For x=5.4 et y=e-10  Armstrong           => voigt = 2.260842e-12'
   print,'. For x=5.4 et y=e-10  Humlicek CPF12 1979 => voigt = 2.260845e-12'
   print,'. For x=5.4 et y=e-10  Humlicek W4 1982    => voigt = 2.260842e-12'
   print,'. For x=5.4 et y=e-10  Hui                 => voigt = 2.667847e-8'
   print,'. For x=5.4 et y=e-10  Lether and Wenston  => voigt = 2.260845e-12'
   print,'                Computed by '+soft+' value of voigt = ', res1
   print,''
   print,'. For x=5.5 et y=e-14  Armstrong           => voigt = 7.307387e-14'
   print,'. For x=5.5 et y=e-14  Humlicek CPF12 1979 => voigt = 7.307387e-14'
   print,'. For x=5.5 et y=e-14  Humlicek W4 1982    => voigt = 1.966215e-16'
   print,'. For x=5.5 et y=e-14  Hui                 => voigt = 9.238980e-9'
   print,'. For x=5.5 et y=e-14  Lether and Wenston  => voigt = 7.307386e-14'
   print,'                Computed by '+soft+' value of voigt = ', res2
   print,'-------------------------------------------------'
endif
;
BANNER_FOR_TESTSUITE, 'TEST_VOIGT_ArmstrongValues', errors, /status
ERRORS_CUMUL, cumul_errors, errors
;
if KEYWORD_SET(test) then STOP
;
end
;
;-----------------------------------------------------------------------
;   		IV-Test with negative numbers 
;-----------------------------------------------------------------------
pro TEST_VOIGT_NEGATIVE, cumul_errors, test=test
;
errors=0
eps=2e-6
;
a=[-1,1]
res1=VOIGT(A,1.)
res2=VOIGT(A,-1.)
expected=[-0.304744, 0.304744]
if (TOTAL(ABS(res1-expected)) GT eps) then ERRORS_ADD, errors, 'Negative U=1'
if (TOTAL(ABS(res2-expected)) GT eps) then ERRORS_ADD, errors, 'Negative U=-1'
;
BANNER_FOR_TESTSUITE, 'TEST_VOIGT_NEGATIVE', errors, /status
ERRORS_CUMUL, cumul_errors, errors
;
if KEYWORD_SET(test) then STOP
;
end
;
;-----------------------------------------------------------------------
;   		V-Test with not a number and infinite values 
;-----------------------------------------------------------------------
pro TEST_VOIGT_NAN_INF, cumul_errors, test=test
;
errors=0
eps=1e-6
;
; putting NaN and Inf in A --> should give Nan
;
A = [1.0005,8.33,4222d, $
     !VALUES.D_NAN,-!VALUES.D_NAN,!VALUES.F_INFINITY, -!VALUES.F_INFINITY ]
U=2.00
res=VOIGT(a,u)
expect=[0.14025996, 0.063700637, 0.00013363085, $
        !VALUES.D_NAN, !VALUES.D_NAN, !VALUES.D_NAN, !VALUES.D_NAN]
;
ok=WHERE(FINITE(res), nbp_ok)
if (nbp_ok NE 3) then ERRORS_ADD, errors, 'A bad number of OK points :('
if (TOTAL(ABS(res[ok]-expect[ok])) GT 3.*eps) then $
   ERRORS_ADD, errors, 'A bad val'
;
; putting Inf in U  --> should give Zero
;
expect=[0.,0.,!VALUES.D_NAN, !VALUES.D_NAN]
res=VOIGT(2.000,[!VALUES.F_INFINITY, -!VALUES.F_INFINITY,$
                 !VALUES.F_NAN, !VALUES.F_NAN])
ok=WHERE(FINITE(res), nbp_ok)
if (nbp_ok NE 2) then ERRORS_ADD, errors, 'U bad number of OK points :('
if (TOTAL(ABS(res[ok]-expect[ok])) GT 3.*eps) then $
   ERRORS_ADD, errors, 'U bad val'
;
BANNER_FOR_TESTSUITE, 'TEST_VOIGT_NAN_INF', errors, /status
ERRORS_CUMUL, cumul_errors, errors
;
if KEYWORD_SET(test) then STOP
;
end
;-----------------------------------------------------------------------
;   		V-promotion to Double
;-----------------------------------------------------------------------
pro TEST_VOIGT_TYPE, cumul_errors, test=test
;
errors=0
eps=1e-5
expected=0.3047445
;;   IDL 0.30474475888621738
;;   GDL 0.30474421381950378
;
GIVE_LIST_NUMERIC, num_types, num_names
;
for ii=0, N_ELEMENTS(num_types)-1 do begin
   a=FIX(REPLICATE(1, [2,3]), type=num_types[ii])
   res=VOIGT(a,1.)
   if (TOTAL(ABS(res-expected)) GT eps) then begin
      ERRORS_ADD, errors, '1 Bad num U=1'+num_names[ii]
   endif
   if TYPENAME(res) EQ 'DOUBLE' then begin
      if ~((num_names[ii] EQ 'DOUBLE') OR (num_names[ii] EQ 'DCOMPLEX')) then begin
         ERRORS_ADD, errors, 'Bad Type promotion for '+num_names[ii]
      endif
   endif
   ;; all should go to double because U is double
   res=VOIGT(a,1.d)
   if (TOTAL(ABS(res-expected)) GT eps) then ERRORS_ADD, errors, '2 Bad num U=1'
   if TYPENAME(res) NE 'DOUBLE' then begin
      ERRORS_ADD, errors, 'Bad Type promotion for '+num_names[ii]         
   endif
endfor
;
BANNER_FOR_TESTSUITE, 'TEST_VOIGT_TYPE', errors, /status
ERRORS_CUMUL, cumul_errors, errors
;
if KEYWORD_SET(test) then STOP

end
;
;-----------------------------------------------------------------------
;   		VI-Test use to see if there is no memory leak
; We used fast algo ...
;-----------------------------------------------------------------------
pro TEST_VOIGT_TIME, nb_loops=nb_loops
;
if (N_ELEMENTS(nb_loops) EQ 0) then nb_loops=12
;
;x=findgen(2000)/10-100
a=[0.01, 0.1, 0.25, .5, 1., 2., 4., 8., 16., 32., 64., 128.]
nb_a=N_ELEMENTS(a)
;
t0=SYSTIME(1)
for jj=0, nb_loops-1 do begin
    for ii=0, nb_a-1 do begin
        y1=VOIGT(a[ii],dist(512))
    endfor
endfor
print, 'VOIGT: ', SYSTIME(1)-t0
;
end
;
;-----------------------------------------------------------------------
; AC 2025 : I have no trace for VOIGT() that we have the same
; problems we had for the Besel family (bugs corrected in IDL)
; 1-element array [1] and singleton "1" should behaved differently ...
;
; showing IDL/GDL differences in behavoir 
;
pro TEST_VOIGT_SINGLETON, test=test
;
a=1
u=[1,2,3,-10.]
;
res1=VOIGT(a,u)
res2=VOIGT([a],u)
res3=VOIGT(REPLICATE(a,N_ELEMENTS(u)),u)
;
if KEYWORD_SET(test) then STOP
;
end
;
;-----------------------------------------------------------------------
;
pro TEST_VOIGT, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
   print, 'TEST_VOIGT, help=help, verbose=verbose, no_exit=no_exit, test=test'
    return
endif
;
cumul_errors=0
;
TEST_VOIGT_ARRAY_DIMS, cumul_errors, test=test
TEST_VOIGT_ARMSTRONGVALUES, cumul_errors, verbose=verbose, test=test
TEST_VOIGT_NEGATIVE, cumul_errors, test=test
TEST_VOIGT_NAN_INF, cumul_errors, test=test
TEST_VOIGT_TYPE, cumul_errors, test=test
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_VOIGT', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
