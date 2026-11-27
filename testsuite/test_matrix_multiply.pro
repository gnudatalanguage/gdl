;
; Under GNU GPL v2 or later
; Alain Coulais, 8 April 2013
;
; Basic tests on matrix multiplication
; (and also TRANSPOSE and INVERT ...)
;
; As mention in the Eigen3 documentation, array aligment and other
; tricks might fail, we have to check all cases are equivalent OK
; (internal size of hardware pipes are varying from CPU to CPU ...)
;
; This is not a benchmark on Matrix Multiplication and Inversion,
; look at  testsuite/benchmark/bench_matrix_multiply.pro
;
; ------------------------------------------
; Modification history:
;
; 2026-11-16 : Alain C. limited clean-up
; - standard end of code
; - MSwin should be OK now
; - better messages for any errors
; - restrictions on types 14 & 15 are removed
;
; ------------------------------------------
;
pro TEST_MATRIX_UNITARY, type=type, nbp=nbp, nb_errors=nb_errors, $
                         test=test, debug=debug, verbose=verbose
;
if ~KEYWORD_SET(nb_errors) then nb_errors=0
if ~KEYWORD_SET(nbp) then nbp=100
if ~KEYWORD_SET(type) then type=4
;
if KEYWORD_SET(verbose) then begin
   txt='Runnning on : '+STRCOMPRESS(string(nbp)+'x'+string(nbp),/remove)
   print, txt+' for type : ', type
endif
;
if ((type EQ 6) OR (type EQ 9)) then begin
    value=COMPLEX(2,3.)
    value=value/ABS(value)
    value=MAKE_ARRAY(1, value=value, type=type)
endif else begin
    value=MAKE_ARRAY(1, value=1, type=type)
endelse
;
if KEYWORD_SET(debug) then begin
   print, 'nbp: ', nbp
   print, 'expected type : ', type
   print, 'value : ', value
   print, 'value type : ', SIZE(value,/type)
endif
;
identity=DIAG_MATRIX(REPLICATE(1., nbp))
;
matrice=DIAG_MATRIX(REPLICATE(value, nbp-1), 1)
matrice[0,nbp-1]=value
;
inv_matrice=INVERT(matrice)
;
; We use a "rotation matrix"
;
trans_matrice=TRANSPOSE(matrice)
effective_type=SIZE(matrice,/type)
if ((effective_type EQ 6) OR (effective_type EQ 9)) then trans_matrice=CONJ(trans_matrice)
;
test1=matrice#inv_matrice-identity
test2=inv_matrice#matrice-identity
test3=matrice#trans_matrice-identity
test4=trans_matrice#matrice-identity
;
errors=0
tolerance=1e-5
txt='test on type :'+STRCOMPRESS(STRING(type),/remove)
;
if ABS(MIN(test1)) GT tolerance then ERRORS_ADD, errors, 'Min t1 '+txt
if ABS(MAX(test1)) GT tolerance then ERRORS_ADD, errors, 'Max t1 '+txt
if ABS(MIN(test2)) GT tolerance then ERRORS_ADD, errors, 'Min t2 '+txt
if ABS(MAX(test2)) GT tolerance then ERRORS_ADD, errors, 'Max t2 '+txt
if ABS(MIN(test3)) GT tolerance then ERRORS_ADD, errors, 'Min t3 '+txt
if ABS(MAX(test3)) GT tolerance then ERRORS_ADD, errors, 'Max t3 '+txt
if ABS(MIN(test4)) GT tolerance then ERRORS_ADD, errors, 'Min t4 '+txt
if ABS(MAX(test4)) GT tolerance then ERRORS_ADD, errors, 'Max t4 '+txt
;
if (errors GT 0) then begin
    print, 'size of matrix : ', nbp
    print, 'nb errors : ', errors
    print, 'input type : ', type
    print, 'effective type : ', effective_type
endif
;
if KEYWORD_SET(debug) then begin
    print, MIN(test1), MAX(test1)
    print, MIN(test2), MAX(test2)
    print, MIN(test3), MAX(test3)
    print, MIN(test4), MAX(test4)
endif
;
; --------------
; we activate that only in verbose mode or when errors occur
; to avoid too much messages
if KEYWORD_SET(verbose) OR (errors GT 0) then begin
   BANNER_FOR_TESTSUITE, "TEST_SYNTAX_NUMERIC", errors, /short
endif
ERRORS_CUMUL, nb_errors, errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------------
;
pro TEST_MATRIX_MULTIPLY, no_exit=no_exit, extended=extended,$
                          help=help, verbose=verbose, test=test
;
; AC 2026-04-02 temporary short-cut for tests on MSwin
; AC 2026-11-16 reverted since we move to 'ucrt-x86_64'
; if (!version.os_family eq 'Windows') then EXIT, status=77
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_MATRIX_MULTIPLY, no_exit=no_exit, extended=extended,$'
    print, '                          help=help, verbose=verbose, test=test'
    return
endif
;
; [1,2,3,4,5,6,9,12,13,14,15]
GIVE_LIST_NUMERIC, liste_type
;
; commented out by AC on 2026/11/16, tested with success
;
; today, the two last cases with types 14 and 15 are not working in GDL
;
;;if ~KEYWORD_SET(extended) then begin
;;    msg='Types 14 and 15 desactivated due to pbs in INVERT() in GDL for these 2 type'
;;    MESSAGE, /continue, msg
;;    liste_type=[1,2,3,4,5,6,9,12,13]
;; endif
;
nb_errors=0
;
liste_taille=[2,3,4,10,11,13,25,50,51,100,101,220,255,256,260]
;
for itailles=0, N_ELEMENTS(liste_taille)-1 do begin
    for itypes=0, N_ELEMENTS(liste_type)-1 do begin
        TEST_MATRIX_UNITARY, type=liste_type[itypes], $
          nbp=liste_taille[itailles], nb_errors=nb_errors, verbose=verbose
    endfor
endfor
;
cumul_errors=nb_errors
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_MATRIX_MULTIPLY', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

