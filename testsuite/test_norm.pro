;
; under GNU GPL v3
; Alain Coulais, 17 july 2017
;
; we are not ready for LA_SVD :(
;
; -----------------------------------------------------------------
;
pro TEST_NORM_VECTORS, cumul_errors, verbose=verbose, test=test
;
nb_errors=0
;
; to be extend to other types ? (float, complex ?)
;
vector=INDGEN(10)
norms=FINDGEN(6)/2
expected=[9.0, 372.72165, 45.0, 23.103502, 16.881943, 14.141151]
;
results=FLTARR(N_ELEMENTS(norms))
;
for ii=0, N_ELEMENTS(norms)-1 do begin 
   results[ii]=NORM(vector, lnorm=norms[ii])
endfor
;
if TOTAL(ABS(expected-results)) GT 1.e-6 then nb_errors++
;
BANNER_FOR_TESTSUITE, 'TEST_NORM_VECTORS', nb_errors, /short, verb=verbose
;
ERRORS_CUMUL, cumul_errors, nb_errors
;
if KEYWORD_SET(test) then STOP
;
end
; -----------------------------------------------------------------
;
pro TEST_NORM_MATRIX, cumul_errors, verbose=verbose, test=test, type=type
;
nb_errors=0
;
matrix=DIST(4)
if KEYWORD_SET(type) then matrix=FIX(matrix, type=type)
;
norms=[0,1,2]
expected=[9.3005631, 9.3005631, 6.84515559]
;
results=FLTARR(N_ELEMENTS(norms))
;
for ii=0, N_ELEMENTS(norms)-1 do begin 
   results[ii]=NORM(matrix, lnorm=norms[ii])
endfor
;
if TOTAL(ABS(expected-results)) GT 1.e-6 then nb_errors++
;
BANNER_FOR_TESTSUITE, 'TEST_NORM_MATRIX', nb_errors, /short, verb=verbose
;
ERRORS_CUMUL, cumul_errors, nb_errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------------------------
;
pro TEST_NORM, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_NORM, help=help, verbose=verbose, $'
   print, '               no_exit=no_exit, test=test'
   return
endif
;
; do we need to add tests for other types in "vectors" ?
; e.g. : a=dist(4) & print, norm(a^25L, lnorm=1,/dou)
;
TEST_NORM_VECTORS, nb_errors
;
; same question !
TEST_NORM_MATRIX, nb_errors
TEST_NORM_MATRIX, nb_errors, type=5
a=EXECUTE("TEST_NORM_MATRIX, nb_errors, type=6")
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_NORM', nb_errors
; 
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
