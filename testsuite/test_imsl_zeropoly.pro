;
; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
;
; Notes by AC 2026-12-15 :
;    * IMSL_ZEROPOLY() and FZ_ROOTS() are functionnaly equivalent,
;    looking for roots of polynomials, but use different numerical approaches.
;    * IMSL_ZEROPOLY() don't accept Complex coeffs (TBD)
;    * FZ_ROOTS() can process Complex coeffs (since 2025 December)
;
; -------------------------------------------------
;
; Modifications history :
; -2025-12-01 AC : renamed from test_zeropoly into test_imsl_zeropoly
; in fact it is a test for IMSL_ZEROPOLY() !
;
; Notes by AC 2026-12-15 : IMSL_ZEROPOLY() don't accept Complex coeffs
;
; -------------------------------------------------
; -------------------------------------------------
;
pro TEST_IMSL_ZP_REAL, cumul_errors, test=test
;
errors=0
;
n = 4                 ; number of test polynomials
eps = 1e-6            ; a small value used for camparing floating point numbers
c = PTRARR(n)         ; polynomial coefficients
r = PTRARR(n)         ; polynomial roots
;
;; IDL-doc example (IMSL_ZEROPOLY)
c[0] = PTR_NEW([-2, 4, -3, 1])
r[0] = PTR_NEW([1, COMPLEX(1,-1), COMPLEX(1,1)])
;
;; GSL-doc example 
c[1] = PTR_NEW([-1, 0, 0, 0, 0, 1])
r[1] = PTR_NEW(EXP([0,2,4,6,8] * !PI * COMPLEX(0,1) / 5))
  
;; IDL-doc examples (FZ_ROOTS)
c[2] = PTR_NEW([-2, -9, -7, 6])
r[2] = PTR_NEW([-.5, -1./3, 2.])
;
c[3] = PTR_NEW([2, 0, 3, 0, 1])
r[3] = PTR_NEW(COMPLEX(0,[-SQRT(2), SQRT(2), -1, 1]))
;
;; trying with different input and output types for all test polynomials
for double_kw = 0, 1 do for i = 0, n - 1 do for t = 2, 15 do begin
   if (t ge 6 && t le 11) || t eq 12 || t eq 13 || t eq 15 then continue
   z = IMSL_ZEROPOLY(FIX(*c[i], type=t), double=double_kw)
   nz = N_ELEMENTS(z) 
   if nz ne N_ELEMENTS(*r[i]) then begin
      ERRORS_ADD, errors, 'TOTAL FAILURE!'
   endif
   ;; checking the results (which might be ordered differently)
   for jz = 0, nz - 1 do for jr = 0, nz - 1 do begin
      if ABS(z[jz] - (*r[i])[jr]) lt eps then begin
         z[jz] = COMPLEX(!VALUES.F_NAN, !VALUES.F_NAN)
         continue
      endif
   endfor
   wh = WHERE(FINITE(z), cnt)
   if cnt ne 0 then begin
      ERRORS_ADD, errors, 'FAILED for test ' + string(i)
   endif
endfor
;;
;; freeing heap vars
foreach i, c do PTR_FREE, I
foreach I, C DO PTR_FREE, i
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_IMSL_ZP_REAL', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------------
;
pro TEST_IMSL_ZP_COMPLEX, cumul_errors, test=test
;
errors=0
;
c1 = [COMPLEX(18,24), COMPLEX(-3,4), COMPLEX(-4,-4), 1.0]
res=EXECUTE('res=IMSL_ZEROPOLY(c1)')
;
txt='IMSL_ZEROPOLY: complex coefficients not supported yet (FIXME !)'
if (res EQ 0) then BANNER_FOR_TESTSUITE, 'MISSING functionnality', txt
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_IMSL_ZP_COMPLEX', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------------
;
pro TEST_IMSL_ZEROPOLY, help=help, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_IMSL_ZEROPOLY, help=help, test=test, no_exit=no_exit'
    return
endif
;
; we need a IMSL licence to run IMSL_ZEROPOLY() in IDL 
;
if (GDL_IDL_FL() EQ 'IDL') then begin
   if ~LMGR('idlimsl') then begin
      MESSAGE, 'ISML licence missing in IDL', /continue
      if KEYWORD_SET(no_exit) then STOP else EXIT, status=77
   endif else  MESSAGE, 'ISML licence activated in IDL', /continue
endif
;
cumul_errors=0
;
TEST_IMSL_ZP_REAL, cumul_errors, test=test
TEST_IMSL_ZP_COMPLEX, cumul_errors, test=test
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_IMSL_POLY', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

