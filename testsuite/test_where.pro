;
; AC, 18 March 2014
;
; We found by chance a bug in WHERE related to OpenMP.
; Any idea to extend test here are very welcome.
;
; ------------------------
;
; statistical approach : the initial way to find it !
;
pro TEST_WHERE_WITH_RANDOM, nb_errors, verbose=verbose
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
end
;
; ------------------------
;
pro TEST_WHERE_OVER_TPOOL_MIN_ELTS, nb_errors, verbose=verbose
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
if (nb_errors EQ 0) then begin
    MESSAGE, /continue, 'No error found in TEST_WHERE'
endif else begin
    MESSAGE, /continue, STRING(nb_errors)+' errors found in TEST_WHERE'
    if ~KEYWORD_SET(no_exit) then EXIT, status=1
endelse
;
if KEYWORD_SET(test) then STOP
;
end
