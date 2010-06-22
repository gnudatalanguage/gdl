;
; Maxime Lenoir (contact also: Alain Coulais)
; Distributed version 2010/06/21
; Under GNU GPL V2 or later
;
; Purpose: Check FILE_SEARCH procedure with different glob patterns
; Tests *,  [] patterns and /fold_case keyword
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
pro TEST_FILE_SEARCH
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
    TEST_FILE_SEARCH_free, fd
    EXIT, status=1 
endif

if FILE_SEARCH('[]]foo.txt') ne ']foo.txt' then begin 
    MESSAGE, /continue, 'Fail with []]foo.txt'
    TEST_FILE_SEARCH_free, fd
    EXIT, status=1
endif

f=FILE_SEARCH('foo*.txt')
if WHERE(f eq 'foo*.txt') eq -1 or WHERE(f eq 'foobar.txt') eq -1 then begin 
    MESSAGE, /continue, 'Fail with foo*.txt'
    TEST_FILE_SEARCH_free, fd
    EXIT, status=1
endif

if FILE_SEARCH('foo[*].txt') ne 'foo*.txt' then begin 
    MESSAGE, /continue, 'Fail with foo[*].txt'
    TEST_FILE_SEARCH_free, fd
    EXIT, status=1
endif

f=FILE_SEARCH('[]a]foo*')
if WHERE(f eq ']foo.txt') eq -1 or WHERE(f eq 'afoo.txt') eq -1 then begin 
    MESSAGE, /continue, 'Fail with []a]foo*'
    TEST_FILE_SEARCH_free, fd
    EXIT, status=1
endif

f=FILE_SEARCH('afoo.txt', /fold_case)
if WHERE(f eq 'AfoO.txt') eq -1 or WHERE(f eq 'Afoo.txt') eq -1 then begin
    MESSAGE, /continue, 'Fail with afoo.txt,  /fold_case'
    TEST_FILE_SEARCH_free, fd
    EXIT, status=1
endif

f=FILE_SEARCH('[]a]foo*', /fold_case)
if WHERE(f eq 'Afoo.txt') eq -1 or WHERE(f eq ']foo.txt') eq -1 or WHERE(f eq 'afoo.txt') eq -1 or WHERE(f eq 'AfoO.txt') then begin 
    MESSAGE, /continue, 'Fail with []a]foo*,  /fold_case'
    TEST_FILE_SEARCH_free, fd
    EXIT, status=1
endif

if FILE_SEARCH('[foo', /fold_case) ne '[Foo' then begin
    MESSAGE, /continue, 'Fail with [foo,  /fold_case'
    TEST_FILE_SEARCH_free, fd
    EXIT, status=1
endif

TEST_FILE_SEARCH_FREE, fd

end
