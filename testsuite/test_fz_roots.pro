;
; under GNU GPL v2 or later
; Benjamin Laurent, 15-Juin-2012
; 
; -------------------------------------------------
; Modifications history :
;
; - 2018-FEB-04 : AC. Corrected "bug" when cumulating large number
;   of byte ... large rewriting, trying to automatic
;   and to have numerical cases at the limits ...
;
; - 2025-Dec-14 : AC. large rewritting.
;   Adding cases with Complex coeffs
;
; -------------------------------------------------
;
; approche gloutonne robuste ...
; -------------------------------------------------
;
function MATCH_ROOTS, z, w
;; Z, W : vecteurs réels ou complexes de même taille
n = N_ELEMENTS(Z)
D = FLTARR(n, n)
FOR i = 0, n-1 DO $
   FOR j = 0, n-1 DO $
      D[i,j] = ABS(Z[i] - W[j])
;;
used = INTARR(n)
match = INTARR(n)
;;
FOR k = 0, n-1 DO BEGIN
   minval = MIN(D, pos)
   i = pos MOD n
   j = pos / n
   ;;
   match[i] = j
   used[j] = 1
   ;; neutralisation ligne + colonne
   D[i,*] = 1e30
   D[*,j] = 1e30
ENDFOR
RETURN, match
END
;
; -------------------------------------------------
pro CHECK_ROOTS, res, res_expected, errors, message, test=test
;
eps=1e-6
tri=MATCH_ROOTS(res,res_expected) 
liste_errors=ABS(res-res_expected[tri])
pbs=WHERE(liste_errors GT eps, nb_pbs)
if (nb_pbs GT 0) then ERRORS_ADD, errors, message
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------------
; When input coeffs are real, we do use the GSL method
; which is known to be accurate and stable
;
pro TEST_FZ_ROOTS_REAL, cumul_errors, test=test, verbose=verbose
;
errors=0
;
c1=[-1,0,0,0,0,1]
res_expected=EXP([0,2,4,6,8] * !PI * COMPLEX(0,1) / 5)
res=FZ_ROOTS(c1,/double)
CHECK_ROOTS, res, res_expected, errors, 'First Poly Real'
;
; IDL-doc example (IMSL_ZEROPOLY)
c2 = [-2, 4, -3, 1]
res_expected = [1, COMPLEX(1,-1), COMPLEX(1,1)]
res=FZ_ROOTS(c2,/double)
CHECK_ROOTS, res, res_expected, errors, 'Second Poly Real'
;
; IDL-doc examples (FZ_ROOTS)
c3=[-2, -9, -7, 6]
res_expected = [-.5, -1./3, 2.]
res=FZ_ROOTS(c3,/double)
CHECK_ROOTS, res, res_expected, errors, 'Third Poly Real'
;
c4=[2, 0, 3, 0, 1]
res_expected = COMPLEX(0,[-SQRT(2), SQRT(2), -1, 1])
res=FZ_ROOTS(c4,/double)
CHECK_ROOTS, res, res_expected, errors, 'Forth Poly Real'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_FZ_ROOTS_REAL', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
;
; -------------------------------------------------
; When input coeffs are real, we do use the GSL method
; which is known to be accurate and stable
;
pro TEST_FZ_ROOTS_COMPLEX, cumul_errors, test=test, verbose=verbose
;
errors=0
;
c1 = [COMPLEX(18,24), COMPLEX(-3,4), COMPLEX(-4,-4), 1.0]
res_expected=[-2.,3., complex(3.,4.)]
res=FZ_ROOTS(c1,/double)
CHECK_ROOTS, res, res_expected, errors, 'First Poly COMPLEX'
;
c2 = [COMPLEX(1.,0), COMPLEX(1,0), COMPLEX(1,0)]
res_expected = [COMPLEX(-.5,SQRT(3.)/2.), COMPLEX(-.5,-SQRT(3.)/2.)]
res=FZ_ROOTS(c2,/double)
CHECK_ROOTS, res, res_expected, errors, 'Second Poly COMPLEX'
;
c1r=COMPLEX([-1,0,0,0,0,1])
res_expected=EXP([0,2,4,6,8] * !PI * COMPLEX(0,1) / 5)
res=FZ_ROOTS(c1r,/double)
CHECK_ROOTS, res, res_expected, errors, 'First Poly Real as a Complex'

; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_FZ_ROOTS_COMPLEX', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------------
;
pro TEST_FZ_ROOTS, help=help, test=test, verbose=verbose, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FZ_ROOTS, help=help, test=test, verbose=verbose, no_exit=no_exit'
   return
endif 
;
cumul_errors=0
;
TEST_FZ_ROOTS_REAL, cumul_errors, test=test, verbose=verbose
TEST_FZ_ROOTS_COMPLEX, cumul_errors, test=test, verbose=verbose
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_FZ_ROOTS', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end




