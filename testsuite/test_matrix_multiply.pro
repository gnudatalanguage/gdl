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
pro TEST_MATRIX_UNITARY, type=type, nbp=nbp, nb_errors=nb_errors, $
                         test=test, debug=debug, verbose=verbose
;
if ~KEYWORD_SET(nb_errors) then nb_errors=0
if ~KEYWORD_SET(nbp) then nbp=100
if ~KEYWORD_SET(type) then type=4
if KEYWORD_SET(debug) then print, ' nbp: ', nbp, 'type=', type
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
if ABS(MIN(test1)) GT tolerance then errors++
if ABS(MAX(test1)) GT tolerance then errors++
if ABS(MIN(test2)) GT tolerance then errors++
if ABS(MAX(test2)) GT tolerance then errors++
if ABS(MIN(test3)) GT tolerance then errors++
if ABS(MAX(test3)) GT tolerance then errors++
if ABS(MIN(test4)) GT tolerance then errors++
if ABS(MAX(test4)) GT tolerance then errors++
;
if (errors GT 0) then begin
    print, 'size of matrix : ', nbp
    print, 'nb errors : ', errors
    print, 'input type : ', type
    print, 'effective type : ', effective_type
    nb_errors=nb_errors+errors
endif
;
if KEYWORD_SET(debug) then begin
    print, MIN(test1), MAX(test1)
    print, MIN(test2), MAX(test2)
    print, MIN(test3), MAX(test3)
    print, MIN(test4), MAX(test4)
endif
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------------
;
pro  TEST_MATRIX_MULTIPLY, no_exit=no_exit, extended=extended,$
                           help=help, verbose=verbose, test=test
;
if KEYWORD_SET(test) then begin
    print, 'pro  TEST_MATRIX_MULTIPLY, no_exit=no_exit, extended=extended,$'
    print, '                           help=help, verbose=verbose, test=test'
    return
endif
;
; today, the two last cases with types 14 and 15 are not working in GDL
;
if ~KEYWORD_SET(extended) then begin
    msg='Types 14 and 15 desactivated due to pbs in INVERT() in GDL for these 2 type'
    MESSAGE, /continue, msg
    liste_type=[1,2,3,4,5,6,9,12,13]
endif else begin
    liste_type=[1,2,3,4,5,6,9,12,13,14,15]
endelse
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
if KEYWORD_SET(test) then STOP
;
if (nb_errors GT 0) then begin
    msg='At least one error encountered, please rerun code with /verbose'
    MESSAGE, /continue, msg
    if ~KEYWORD_SET(no_exit) then EXIT, status=1
endif
;
end
