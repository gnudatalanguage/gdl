;
; AC, 18 March 2014
;
; We found by chance a bug in WHERE related to OpenMP.
; Any idea to extend test here are very welcome.
;
;
; ------------------------
;
pro TEST_WHERE_NULL, cumul_errors, verbose=verbose, test=test
;
nb_errors=0
;
a=REPLICATE(1,10)
;
null1=WHERE(a GT 1,complement=c1,/null)
if ~ISA(null1,/null) then ADD_ERROR, nb_errors, 'bad value for : null1'
if ISA(c1,/null) then ADD_ERROR, nb_errors, 'bad value for : c1'
;
ok=WHERE(a EQ 1,complement=null2,/null)
if ISA(ok,/null) then ADD_ERROR, nb_errors, 'bad value for : ok'
if ~ISA(null2,/null) then ADD_ERROR, nb_errors, 'bad value for : null2'
;
BANNER_FOR_TESTSUITE, 'TEST_WHERE_NULL', nb_errors, /short
;
ERRORS_CUMUL, cumul_errors, nb_errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
;
; statistical approach : the initial way to find it !
;
pro TEST_WHERE_WITH_RANDOM, cumul_errors, verbose=verbose, test=test
;
nb_errors=0
;
size_of_interest=!cpu.TPOOL_MIN_ELTS
;
nbp_in=6*size_of_interest
input=RANDOMU(seed, nbp_in)
;
ok=WHERE(input GE 0.5, nbp_ok)
;
if KEYWORD_SET(verbose) then begin
    print, '!cpu.TPOOL_MIN_ELTS : ', LONG(size_of_interest)
    print, ' expected :           ', LONG(nbp_in/2)
    print, ' result   :           ', LONG(nbp_ok)
endif
;
if (100.*ABS(nbp_in/2-nbp_ok)/FLOAT(nbp_ok) GT 1) then begin
    nb_errors++
endif
;
BANNER_FOR_TESTSUITE, 'TEST_WHERE_WITH_RANDOM', nb_errors, /short
;
ERRORS_CUMUL, cumul_errors, nb_errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
;
pro TEST_WHERE_OVER_TPOOL_MIN_ELTS, cumul_errors, verbose=verbose, test=test
;
nb_errors=0
;
size_of_interest=!cpu.TPOOL_MIN_ELTS
;
; we are not fully ready for very big numbers !
if (size_of_interest GT 2L^28) then MESSAGE, 'Not ready for L64 range !'
;
input=LINDGEN(4*size_of_interest)
;
ok=WHERE(input GE 2*size_of_interest)
;
nbp=N_ELEMENTS(ok)
;
if KEYWORD_SET(verbose) then begin
    print, '!cpu.TPOOL_MIN_ELTS : ', LONG(size_of_interest)
    print, ' expected :           ', LONG(2*size_of_interest)
    print, ' result   :           ', LONG(nbp)
endif
;
if (nbp NE 2*size_of_interest) then nb_errors++
;
BANNER_FOR_TESTSUITE, 'TEST_WHERE_OVER_TPOOL_MIN_ELTS', nb_errors, /short
;
ERRORS_CUMUL, cumul_errors, nb_errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
;
pro TEST_WHERE, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_WHERE, help=help, verbose=verbose, $'
    print, '                no_exit=no_exit, test=test'
    return
endif
;
nb_errors=0
;
TEST_WHERE_NULL, nb_errors, verbose=verbose 
;
CPU, /reset
TEST_WHERE_OVER_TPOOL_MIN_ELTS, nb_errors, verbose=verbose
;
CPU, TPOOL_MIN_ELTS=!cpu.TPOOL_MIN_ELTS/4
TEST_WHERE_OVER_TPOOL_MIN_ELTS, nb_errors, verbose=verbose
;
CPU, /reset
TEST_WHERE_WITH_RANDOM, nb_errors, verbose=verbose
;
CPU, TPOOL_MIN_ELTS=!cpu.TPOOL_MIN_ELTS/4
TEST_WHERE_WITH_RANDOM, nb_errors, verbose=verbose
;
CPU, /reset
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_WHERE', nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
