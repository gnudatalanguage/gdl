;
; AC 25 February 2013
;
; First computation is using "#" operator.
; the 4 computations after are using MATRIX_MULTIPLY() function,
; eventualy calling Eigen3 code (a message will be issued if not)
;
pro BENCH_MATRIX_MULTPLY, n1, n2, n3, help=help, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro BENCH_MATRIX_MULTPLY, n1, n2, n3, help=help, test=test'
endif
;
DEFSYSV, '!gdl', exist=it_is_GDL
;
if (it_is_GDL) then begin
    having_eigen3=EXECUTE("type=MATMUL(/available, quiet=quiet)")
    if (having_eigen3 EQ 0) then begin
        print, 'You are trying testing new capabilities (Eigen3 usage)'
        print, 'on a too OLD GDL version ! Please make tests on CVS version !'
        return
    endif
endif
;
if N_PARAMS() EQ 0 then begin
    colA=1000
    rowA=3000
    rowB=751
endif
;
if N_PARAMS() EQ 1 then begin
    colA=n1
    rowA=n1
    rowB=n1
endif
;
if N_PARAMS() EQ 2 then begin
    colA=n1
    rowA=n2
    rowB=n2
endif
if N_PARAMS() EQ 3 then begin
    colA=n1
    rowA=n2
    rowB=n3
endif
;
colB=rowA
;
a=RANDOMU(seed, colA, rowA)
b=RANDOMU(seed, colB, rowB)

b_t=TRANSPOSE(b)
a_t=TRANSPOSE(a)
;
txt='Matrix size are : [' +STRING(colA)+','+STRING(rowA)
txt=txt+'] # ['+STRING(colB)+','+STRING(rowB)+']'
print, STRCOMPRESS(txt)
;
txt_ref='Classic operator #, '
txt=    'Matrix_Multiply() , '
;
t0=SYSTIME(1) & z=a # b & print, txt_ref+'a#b  :', SYSTIME(1)-t0
t0=SYSTIME(1) & z=MATRIX_MULTIPLY(a,b) & print, txt+'a#b  :', SYSTIME(1)-t0
t0=SYSTIME(1) & z=MATRIX_MULTIPLY(a_t,b,/at) & print, txt+'aT#b :', SYSTIME(1)-t0 
t0=SYSTIME(1) & z=MATRIX_MULTIPLY(a,b_t,/bt) & print, txt+'a#bT :', SYSTIME(1)-t0 
t0=SYSTIME(1) & z=MATRIX_MULTIPLY(a_t,b_t,/at,/bt) & print, txt+'aT#bT:', SYSTIME(1)-t0
;
if KEYWORD_SET(test) then STOP
;
end
