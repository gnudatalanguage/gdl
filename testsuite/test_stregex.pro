;
; AC, 12-oct-2011
;
; some basic tests for STREGEX
;
pro TEST_STREGEX, verbose=verbose, test=test, help=help
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_STREGEX, verbose=verbose, test=test, help=help'
    return
endif
;
nb_errors=0
;
; First serie : basic tests
;
if KEYWORD_SET(verbose) then MESSAGE, /continue, 'basic test 1a START'
pos = STREGEX('aaabccc', 'abc', length=len)  
resu=STRMID('aaabccc', pos, len)
if (resu NE 'abc') then begin
    if KEYWORD_SET(verbose) then MESSAGE, /continue, 'test 1a FAILED'
    nb_errors=nb_errors+1
endif else begin
    if KEYWORD_SET(verbose) then MESSAGE, /continue, 'basic test 1a OK'
endelse
;
if KEYWORD_SET(verbose) then MESSAGE, /continue, 'basic test 1b START'
pos = STREGEX('aaabccc', 'abc+', length=len)  
resu=STRMID('aaabccc', pos, len)
if (resu NE 'abccc') then begin
    if KEYWORD_SET(verbose) then MESSAGE, /continue, 'test 1b FAILED'
    nb_errors=nb_errors+1
endif else begin
    if KEYWORD_SET(verbose) then MESSAGE, /continue, 'basic test 1b OK'
endelse
;
if KEYWORD_SET(verbose) then MESSAGE, /continue, 'basic test 1c START'
pos = STREGEX('aaabccc', '(a)(b)(c+)', length=len, /SUBEXPR)  
resu=STRMID('aaabccc', pos, len)
expected=['abccc','a','b','ccc']
if (TOTAL(STRCMP(resu, expected)) NE 4) then begin
    if KEYWORD_SET(verbose) then MESSAGE, /continue, 'test 1c FAILED'
    nb_errors=nb_errors+1
endif else begin
   if KEYWORD_SET(verbose) then MESSAGE, /continue, 'basic test 1c OK'
endelse
;
; this bug 581 was reported 14-Jan-2014
;
if KEYWORD_SET(verbose) then MESSAGE, /continue, 'basic test 1d START'
pos=STREGEX('abcdefg', '[xyz]', length=lstr)
expected_pos=-1
expected_lstr=-1
if ((pos NE expected_pos) OR (lstr NE expected_lstr)) then begin
    if KEYWORD_SET(verbose) then MESSAGE, /continue, 'test 1d FAILED'
    nb_errors=nb_errors+1
endif else begin
   if KEYWORD_SET(verbose) then MESSAGE, /continue, 'basic test 1d OK'
endelse
;
; Second serie : more complex Regex tests
;
str = ['foot', 'Feet', 'fate', 'FAST', 'ferret', 'affluent']
str2D=REFORM(str,[3,2])
;
if KEYWORD_SET(verbose) then MESSAGE, /continue, 'basic test 2a START'
resu=STREGEX(str, '^f[^o]*t$')
expected=REPLICATE(-1L,N_ELEMENTS(str))
expected[4]=0
if ~ARRAY_EQUAL(resu, expected) then begin
    if KEYWORD_SET(verbose) then MESSAGE, /continue, 'test 2a FAILED'
    nb_errors=nb_errors+1
endif else begin
    if KEYWORD_SET(verbose) then MESSAGE, /continue, 'test 2a OK'
endelse
;
if KEYWORD_SET(verbose) then MESSAGE, /continue, 'basic test 2b START'
resu=STREGEX(str, '^f[^o]*t$', /FOLD_CASE)
expected=REPLICATE(0L,N_ELEMENTS(str))
expected[[0,2,5]]=-1L
if ~ARRAY_EQUAL(resu, expected) then begin
    if KEYWORD_SET(verbose) then MESSAGE, /continue, 'test 2b FAILED'
    nb_errors=nb_errors+1
endif else begin
    if KEYWORD_SET(verbose) then MESSAGE, /continue, 'test 2b OK'
endelse

resu=STREGEX(str, '^f[^o]*t$', /EXTRACT)
expected=['' , '' , '' , '' , 'ferret' , '']
if ~ARRAY_EQUAL(resu, expected) then begin
    if KEYWORD_SET(verbose) then MESSAGE, /continue, 'test 3a FAILED'
    nb_errors=nb_errors+1
endif else begin
    if KEYWORD_SET(verbose) then MESSAGE, /continue, 'test 3a OK'
endelse


resu=STREGEX(str, '^f[^o]*t$', /EXTRACT, /FOLD_CASE)
expected=['' , 'Feet', '' ,'FAST', 'ferret' ,'']
if ~ARRAY_EQUAL(resu, expected) then begin
    if KEYWORD_SET(verbose) then MESSAGE, /continue, 'test 3b FAILED'
    nb_errors=nb_errors+1
endif else begin
    if KEYWORD_SET(verbose) then MESSAGE, /continue, 'test 3b OK'
endelse

if (nb_errors GT 0) then begin
    MESSAGE, /continue, STRING(nb_errors)+' Errors found'
    if ~KEYWORD_SET(test) then EXIT, status=1
endif else begin
    MESSAGE, /continue, 'No Errors found'
endelse
;
if KEYWORD_SET(test) then STOP
;
end
