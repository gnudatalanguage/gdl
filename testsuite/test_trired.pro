;
; 2024/04/21, Alain C. under GNU GPL V3+
;
; For unknown reasons, except noone like to write tests,
; we don't have full basic tests for
; TRIRED/TRIQL and LA_TRIRED/LA_TRIQL 
;
; it is true also that the order of eigenvalues and the signs
; of eigenvectors are not fully predictables ......
;
; -----------------
; Not to forgot in case of extension : if input is of type Unsigned,
; it should not come from a matrix with negative values ...
;
; -------------------------------------------
; exemples here are coming from
; https://sites.math.northwestern.edu/~len/LinAlg/chap3.pdf
; -------------------------------------------
;
pro CASE_SYMMETRIC_MATRIX_2X2, cumul_errors, double=double, test=test, verbose=verbose
;
nb_errors=0
eps=1.e-6
;
; we also would like to test *all* the numerics tests
GIVE_LIST_NUMERIC, list_numeric_types, list_numeric_names
;
for ii=0, N_ELEMENTS(list_numeric_types)-1 do begin
   input=FIX( [[2.,1],[1,2]], type=list_numeric_types[ii])
   ;;
   TRIRED, input, d, e, double=double
   ;; Compute the eigenvalues (returned in vector D) and the
   ;; eigenvectors (returned in the rows of the array A):
   TRIQL, D, E, input
   ;;
   if KEYWORD_SET(test) then begin
      print, 'Working on : ', list_numeric_names[ii]
      PRINT, 'Eigenvalues:'
      PRINT, D
      PRINT, 'Eigenvectors:'
      PRINT, input
      print, list_numeric_names[ii], ' ', TYPENAME(input), TYPENAME(d), TYPENAME(e)
   endif
   ;;
   exp_eigval=[1.,3]
   if (TOTAL(ABS(exp_eigval-d)) GT eps) then $
      ERRORS_ADD, nb_errors, 'Case 2x2 '+list_numeric_names[ii]
   ;;
   exp_eigvects=[[1.,-1.],[1.,1.]]*SQRT(2.)/2.
   if (TOTAL(ABS(input-exp_eigvects)) GT eps) then $
      ERRORS_ADD, nb_errors, 'Case 2x2 '+list_numeric_names[ii]
   ;;
endfor
;
; --------------
;
BANNER_FOR_TESTSUITE, "CASE_SYMMETRIC_MATRIX_2X2", nb_errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------------
;
function PLUS_MOINS_NORM, v1, v2, debug=debug
eps=1.e-6
plus=TOTAL(ABS(v1-v2))
moins=TOTAL(ABS(v1+v2))
logical=~((plus LT eps) OR (moins LT eps))
if KEYWORD_SET(debug) then print, plus, moins, logical
return, logical
end
;
; -------------------------------------------
;
pro CASE_SYMMETRIC_MATRIX_3X3, cumul_errors, double=double, test=test, verbose=verbose
;
nb_errors=0
eps=1.e-6
;
; we also would like to test *all* the numerics tests
GIVE_LIST_NUMERIC, list_numeric_types, list_numeric_names
;
for ii=0, N_ELEMENTS(list_numeric_types)-1 do begin
   ;;
   ;; IMPORTANT : by design thie matrix contains negative values
   ;; this is a problem for Unsigned types !!
   ;;
   the_tname=list_numeric_names[ii]
   if the_tname EQ 'BYTE' OR the_tname EQ 'UINT' then CONTINUE
   if the_tname EQ 'ULONG' OR the_tname EQ 'ULONG64' then CONTINUE
   ;;
   input=FIX([[-1,1,1],[1,-1,1],[1,1,-1]], type=list_numeric_types[ii])
   ;;
   TRIRED, input, d, e, double=double
   ;; Compute the eigenvalues (returned in vector D) and the
   ;; eigenvectors (returned in the rows of the array A):
   TRIQL, D, E, input
   ;;
   if KEYWORD_SET(test) or KEYWORD_SET(verbose) then begin
      print, 'Working on : ', list_numeric_names[ii]
      PRINT, 'Eigenvalues:'
      PRINT, D
      PRINT, 'Eigenvectors:'
      PRINT, input
      print, list_numeric_names[ii],' ',TYPENAME(input), ' ',TYPENAME(d),' ', TYPENAME(e)
      print, '-------------------------------------------'
   endif
   ;;
   exp_eigval=[-2.,1., -2]
   if (TOTAL(ABS(exp_eigval-d)) GT eps) then $
      ERRORS_ADD, nb_errors, 'Case 3x3 Eigen Val '+list_numeric_names[ii]
   ;;
   u2=[1.,1.,1]/SQRT(3.)
   u1=[-1.,1, 0]/SQRT(2.)
   u3=[-1./2.,-1/2.,1]*SQRT(2./3.)
   ;;
   if PLUS_MOINS_NORM(input[*,0],u1) then $
      ERRORS_ADD, nb_errors, 'Case 3x3, u1 '+list_numeric_names[ii]
   if PLUS_MOINS_NORM(input[*,1],u2) then $
      ERRORS_ADD, nb_errors, 'Case 3x3, u2 '+list_numeric_names[ii]
   if PLUS_MOINS_NORM(input[*,2],u3) then $
      ERRORS_ADD, nb_errors, 'Case 3x3, u3 '+list_numeric_names[ii]
   ;;   
endfor
;
; --------------
;
BANNER_FOR_TESTSUITE, "CASE_SYMMETRIC_MATRIX_3X3", nb_errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
; --------------------------------------
;
pro TEST_TRIRED, help=help, test=test, $
                 verbose=verbose, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_TRIRED, help=help, test=test, $'
   print, '                 verbose=verbose, no_exit=no_exit'
   return
endif
;
cumul_errors=0
;
CASE_SYMMETRIC_MATRIX_2X2, cumul_errors
CASE_SYMMETRIC_MATRIX_2X2, cumul_errors, /double
;
CASE_SYMMETRIC_MATRIX_3X3, cumul_errors
CASE_SYMMETRIC_MATRIX_3X3, cumul_errors, /double
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_TRIRED', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end


