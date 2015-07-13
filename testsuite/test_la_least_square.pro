;
; very basic tests for LA_LEAST_SQUARES
;
; Alain C. and Ilia N.
;
; We do have poor quality results with method 0
; in some cases, help welcome
;
pro TEST_LLS_FROM_MKL, external_errors, test=test, verbose=verbose
;
; test case from MKL
; https://software.intel.com/sites/products/documentation/doclib/mkl_sa/11/mkl_lapack_examples/sgels_ex.f.htm
;
a=[ 1.44,  -7.84, -4.39,  4.53, $
    -9.96, -0.28, -3.24,  3.83, $
    -7.55,  3.24,  6.27, -6.64, $
    8.34,  8.09,  5.28,  2.06, $
    7.08,  2.52,  0.74, -2.47, $
    -5.45, -5.70, -1.19,  4.70]
;
a=REFORM(A, 4, 6)
;    
;, and B is the right-hand side matrix:
;
b=[ 8.58,  9.35, $
    8.26, -4.43, $
    8.48, -0.70, $
    -5.28, -0.26, $
    5.72, -7.36, $
    8.93, -2.52]
b=REFORM(b, 2, 6)

expected_b=[[-0.450637, -0.849150, 0.706612, 0.128886], $
            [0.249748,-0.902019, 0.632343, 0.135124]]
;
nb_errors=0
;
for input=0, 1 do begin
    ;;
    in=REFORM(b[input,*])
    expect=REFORM(expected_b[*,input])
    ;;
    for method=0, 3 do begin
        res=LA_LEAST_SQUARES(a, in, method=method)
        diff=SQRT(TOTAL((res-expect)^2))
        if (diff GT 1e-6) then nb_errors++
        if KEYWORD_SET(verbose) then print, diff, method, res
    endfor
endfor
;
if KEYWORD_SET(test) then STOP
;
if ISA(external_errors) then external_errors=nb_errors+external_errors else $
  external_errors=nb_errors
;
end
;
; --------------------------------------------------------------
;
pro TEST_LA_LEAST_SQUARES, help=help, test=test, no_exit=no_exit, $
                           verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'TEST_LA_LEAST_SQUARES, help=help, test=test, no_exit=no_exit, $'
    print, '                       verbose=verbose'
    print, ''
    print, 'few tests related to LA_LEAST_SQUARES ... idea welcome !'
    return
endif
;
cumul_errors=0
;
TEST_LLS_FROM_MKL, cumul_errors, verbose=verbose, test=test
;
BANNER_FOR_TESTSUITE, "TEST_LA_LEAST_SQUARES", cumul_errors
;
; if /debug OR /test nodes, we don't want to exit
if (cumul_errors GT 0) then begin
    if ~KEYWORD_SET(verbose) then MESSAGE, /continue, 're-run with /verbose for details'
    if ~(KEYWORD_SET(test) or KEYWORD_SET(no_exit)) then EXIT, status=1
endif
;
if KEYWORD_SET(test) then STOP
;
end

