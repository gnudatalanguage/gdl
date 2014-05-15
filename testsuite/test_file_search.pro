;
; Maxime Lenoir (contact also: Alain Coulais)
; Distributed version 2010/06/21
; Under GNU GPL V2 or later
;
; Purpose: Check FILE_SEARCH procedure with different glob patterns
; Tests *,  [] patterns and /fold_case keyword
;
; AC 16 mai 2014: adding test cases for FILE_SEARCH('*',/FULLY_QUALIFY_PATH)
;
pro TEST_FILE_SEARCH_FREE,  fd
for i=0, N_ELEMENTS(fd)-1 do begin
    CLOSE, fd[i]
    FREE_LUN, fd[i]
endfor
end
;
; --------------------------------------
;
pro TEST_FILE_SEARCH_GLOB, nb_errors, no_erase=no_erase, test=test
;
errors=0
;
OPENW, fd1, /delete, /get_lun, ']foo.txt'
OPENW, fd2, /delete, /get_lun, 'foo\*.txt'
OPENW, fd3, /delete, /get_lun, 'foobar.txt'
OPENW, fd4, /delete, /get_lun, 'afoo.txt'
OPENW, fd5, /delete, /get_lun, 'Afoo.txt'
OPENW, fd6, /delete, /get_lun, 'AfoO.txt'
OPENW, fd7, /delete, /get_lun, '[Foo'

fd=[fd1, fd2, fd3, fd4, fd5, fd6, fd7]

if FILE_SEARCH(']foo.txt') ne ']foo.txt' then begin 
    MESSAGE, /continue, 'Fail with ]foo.txt'
    errors++
endif

if FILE_SEARCH('[]]foo.txt') ne ']foo.txt' then begin 
    MESSAGE, /continue, 'Fail with []]foo.txt'
    errors++
endif

f=FILE_SEARCH('foo*.txt')
if WHERE(f eq 'foo*.txt') eq -1 or WHERE(f eq 'foobar.txt') eq -1 then begin 
    MESSAGE, /continue, 'Fail with foo*.txt'
    errors++
endif

if FILE_SEARCH('foo[*].txt') ne 'foo*.txt' then begin 
    MESSAGE, /continue, 'Fail with foo[*].txt'
    errors++
endif

f=FILE_SEARCH('[]a]foo*')
if WHERE(f eq ']foo.txt') eq -1 or WHERE(f eq 'afoo.txt') eq -1 then begin 
    MESSAGE, /continue, 'Fail with []a]foo*'
    errors++
endif

f=FILE_SEARCH('afoo.txt', /fold_case)
if WHERE(f eq 'AfoO.txt') eq -1 or WHERE(f eq 'Afoo.txt') eq -1 then begin
    MESSAGE, /continue, 'Fail with afoo.txt,  /fold_case'
    errors++
endif

f=FILE_SEARCH('[]a]foo*', /fold_case)
if WHERE(f eq 'Afoo.txt') eq -1 or WHERE(f eq ']foo.txt') eq -1 or WHERE(f eq 'afoo.txt') eq -1 or WHERE(f eq 'AfoO.txt') then begin 
    MESSAGE, /continue, 'Fail with []a]foo*,  /fold_case'
    errors++
endif

if FILE_SEARCH('[foo', /fold_case) ne '[Foo' then begin
    MESSAGE, /continue, 'Fail with [foo,  /fold_case'
    errors++
endif
;
if ~KEYWORD_SET(no_erase) then TEST_FILE_SEARCH_FREE, fd
;
BANNER_FOR_TESTSUITE, "TEST_FILE_SEARCH_GLOB", errors, /short
;
nb_errors=nb_errors+errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------
;
pro TEST_FULLY_QUAL_PATH, nb_errors, no_erase=no_erase, test=test
;
errors=0
;
; a way to catch a reference (maybe not the best !)
;
SPAWN, 'ls', res0
CD, current=path
path=path+PATH_SEP()
;
res1=FILE_SEARCH(/FULLY_QUALIFY_PATH)
res2=FILE_SEARCH('',/FULLY_QUALIFY_PATH)
res3=FILE_SEARCH('*',/FULLY_QUALIFY_PATH)
;
res0=res0[SORT(res0)]
;
if (N_ELEMENTS(res0) NE N_ELEMENTS(res1)) then begin
    MESSAGE, /continue, 'pb with N_elements RES1'
    errors++
endif else begin
    res1=res1[SORT(res1)]
    if ~ARRAY_EQUAL(path+res0, res1) then begin
        MESSAGE, /continue, 'pb with content of RES1 vs RES0'
        errors++
    endif
endelse
;
if (N_ELEMENTS(res0) NE N_ELEMENTS(res2)) then begin
    MESSAGE, /continue, 'pb with N_elements RES2'
    errors++
endif else begin
    res2=res2[SORT(res2)]
    if ~ARRAY_EQUAL(path+res0, res2) then begin
        MESSAGE, /continue, 'pb with content of RES2 vs RES0'
        errors++
    endif
endelse
;
if (N_ELEMENTS(res0) NE N_ELEMENTS(res3)) then begin
    MESSAGE, /continue, 'pb with N_elements RES3'
    errors++
endif else begin
    res3=res3[SORT(res3)]
    if ~ARRAY_EQUAL(path+res0, res3) then begin
        MESSAGE, /continue, 'pb with content of RES1 vs RES3'
        errors++
    endif
endelse
;
BANNER_FOR_TESTSUITE, "TEST_FULLY_QUAL_PATH", errors, /short
;
nb_errors=nb_errors+errors
;
if KEYWORD_SET(test) then STOP
;
end

; --------------------------------------------
;
pro TEST_FILE_SEARCH, help=help, test=test, short=short, $
                      verbose=verbose, $
                      no_exit=no_exit, no_erase=no_erase
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_FILE_SEARCH, help=help, test=test, short=short, $'
    print, '                      verbose=verbose, $'
    print, '                      no_exit=no_exit, no_erase=no_erase'
    return
endif
;
nb_errors=0
;
TEST_FILE_SEARCH_GLOB, nb_errors, no_erase=no_erase, test=test
;
TEST_FULLY_QUAL_PATH, nb_errors, no_erase=no_erase, test=test
;
BANNER_FOR_TESTSUITE, "TEST_FILE_SEARCH", nb_errors, short=short
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

