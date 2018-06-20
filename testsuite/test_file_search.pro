;
; Maxime Lenoir (contact also: Alain Coulais)
; Distributed version 2010/06/21
; Under GNU GPL V2 or later
;
; Purpose: Check FILE_SEARCH procedure with different glob patterns
; Tests *,  [] patterns and /fold_case keyword
;
; AC 16 mai 2014: adding test cases for FILE_SEARCH('*',/FULLY_QUALIFY_PATH)
; AC 07 Oct 2015: adding cases related to '.', '..' and '~'
; AC 11 Jan 2017: adding a temporary path ('TMPDIR_FILE_SEARCH')
;
; ----------------------------------------
; generate a list of files with special chars ([ and *)
;
pro TEST_FILE_SEARCH_CREATE, list_luns
;
;
easynames = [ ']foo.txt', $
	'afoo.txt',  $
	'[Foo', 'foobar.txt' ]
neasy = n_elements(easynames)

list_luns = lonarr(neasy)
for k=0,neasy-1 do begin &$
	openw, lu, /delete,/get_lun, easynames[k] &$
	list_luns[k] = lu & endfor
;
if(!version.OS_FAMILY eq "Windows") then return

;
; we can't create a foo\* file and then create a foobar.txt
; So delete the foobar.txt, back up the lun array and fudge the 
;  numbers for the add-on files.
;
neasy = neasy-1
free_lun,list_luns[neasy]
morefiles = [ 'Afoo.txt', 'AfoO.txt', 'foo\*.txt', 'foobar.txt']
nmore = n_elements(morefiles)
list_luns = [list_luns, lonarr(nmore-1)]
for k=0,nmore-1 do begin &$
	openw, lu, /delete,/get_lun, morefiles[k] &$
	list_luns[k+neasy] = lu & endfor
;
end
;
pro TEST_FILE_SEARCH_REMOVE, list_luns
;

for i=0, N_ELEMENTS(list_luns)-1 do FREE_LUN, list_luns[i]

end
;
; --------------------------------------
;
pro TEST_FILE_SEARCH_GLOB, cumul_errors, no_erase=no_erase, test=test
;
errors=0
;
TEST_FILE_SEARCH_CREATE, list_luns
;
if FILE_SEARCH(']foo.txt') ne ']foo.txt' then begin 
    ERRORS_ADD, errors, 'Fail with ]foo.txt'
endif

if FILE_SEARCH('[]]foo.txt') ne ']foo.txt' then begin 
    ERRORS_ADD, errors, 'Fail with []]foo.txt'
endif

f=FILE_SEARCH('foo*.txt')
if WHERE(f eq 'foo*.txt') eq -1 or WHERE(f eq 'foobar.txt') eq -1 then begin 
    ERRORS_ADD, errors, 'Fail with foo*.txt'
endif

if FILE_SEARCH('foo[*].txt') ne 'foo*.txt' then begin 
    ERRORS_ADD, errors, 'Fail with foo[*].txt'
endif

f=FILE_SEARCH('[]a]foo*')
if WHERE(f eq ']foo.txt') eq -1 or WHERE(f eq 'afoo.txt') eq -1 then begin 
    ERRORS_ADD, errors, 'Fail with []a]foo*'
endif

f=FILE_SEARCH('afoo.txt', /fold_case)
if WHERE(f eq 'AfoO.txt') eq -1 or WHERE(f eq 'Afoo.txt') eq -1 then begin
    ERRORS_ADD, errors, 'Fail with afoo.txt,  /fold_case'
endif

f=FILE_SEARCH('[]a]foo*', /fold_case)
if WHERE(f eq 'Afoo.txt') eq -1 or WHERE(f eq ']foo.txt') eq -1 or WHERE(f eq 'afoo.txt') eq -1 or WHERE(f eq 'AfoO.txt') then begin 
    ERRORS_ADD, errors, 'Fail with []a]foo*,  /fold_case'
endif

if FILE_SEARCH('[foo', /fold_case) ne '[Foo' then begin
    ERRORS_ADD, errors, 'Fail with [foo,  /fold_case'
endif
;
if ~KEYWORD_SET(no_erase) then TEST_FILE_SEARCH_REMOVE, list_luns
;
BANNER_FOR_TESTSUITE, "TEST_FILE_SEARCH_GLOB", errors, /status
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------
;
pro TEST_FULLY_QUALIFY_PATH, cumul_errors, no_erase=no_erase, test=test
;
errors=0
;
TEST_FILE_SEARCH_CREATE, list_luns
;
; a way to catch a reference (maybe not the best !)
;
SPAWN, 'ls', res0
;; res0 may contain 'broken symlinks' that GLIBC's glob() do not see
;; due to a IMHO ANNOYING bug. We need to take care of that since GDL
;; uses glob().
;; unofrtunaltely, the following code does not pass well an all machines
;SPAWN, 'find . -xtype l', badlinks
nbadlinks=n_elements(badlinks)
;
;path_sep() cannot be used for Windows because file_search uses forward slash
CD, current=path & path=path+'/'
;   
res1=FILE_SEARCH(/FULLY_QUALIFY_PATH)
res2=FILE_SEARCH('',/FULLY_QUALIFY_PATH)
res3=FILE_SEARCH('*',/FULLY_QUALIFY_PATH)
;
res0=res0[SORT(res0)]
;
if (N_ELEMENTS(res0) NE N_ELEMENTS(res1)) then begin
    if (N_ELEMENTS(res0) NE N_ELEMENTS(res1)+nbadlinks) then begin
      ERRORS_ADD, errors, 'pb with N_elements RES1'
   endif else print,"Dangling symlinks in a directory prevent one test."
endif else begin
    res1=res1[SORT(res1)]
    if ~ARRAY_EQUAL(path+res0, res1) then begin
        ERRORS_ADD, errors, 'pb with content of RES1 vs RES0'
    endif
endelse
;
if (N_ELEMENTS(res1) NE N_ELEMENTS(res2)) then begin
    ERRORS_ADD, errors, 'pb with N_elements RES2'
endif else begin
    res2=res2[SORT(res2)]
    if ~ARRAY_EQUAL(res1, res2) then begin
        ERRORS_ADD, errors, 'pb with content of RES2 vs RES1'
    endif
endelse
;
if (N_ELEMENTS(res2) NE N_ELEMENTS(res3)) then begin
    ERRORS_ADD, errors, 'pb with N_elements RES3'
endif else begin
    res3=res3[SORT(res3)]
    if ~ARRAY_EQUAL(res2, res3) then begin
        ERRORS_ADD, errors, 'pb with content of RES3 vs RES2'
    endif
endelse
;
if ~KEYWORD_SET(no_erase) then TEST_FILE_SEARCH_REMOVE, list_luns
;
BANNER_FOR_TESTSUITE, "TEST_FULLY_QUALIFY_PATH", errors, /status
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------
;
pro TEST_SPECIAL_PATHS, cumul_errors, no_erase=no_erase, test=test
;
errors=0
;
CD,'..', current=current
CD, current, current=updir
home=GETENV('HOME')
;
res1=FILE_SEARCH('.', /FULLY_QUALIFY_PATH)
res2=FILE_SEARCH('..',/FULLY_QUALIFY_PATH)
res3=FILE_SEARCH('~',/FULLY_QUALIFY_PATH)
;
if ~ARRAY_EQUAL(current, res1) then begin
    ERRORS_ADD, errors, 'pb with .'
    print, 'input  : ', current
    print, 'output : ', res1
endif
if ARRAY_EQUAL(current+PATH_SEP()+'..', res2) then begin
    ADD_ERROR, errors, 'pb with .. has wrong path'
    print, 'input :  ', current+PATH_SEP()+'..'
    print, 'output : ', res2
endif
if ~ARRAY_EQUAL(updir, res2) then begin
    ADD_ERROR, errors, 'pb with ..'
    print, 'input  : ', updir
    print, 'output : ', res2
endif

if ~ARRAY_EQUAL(home, res3) then ADD_ERROR, errors, 'pb with ~'
;
BANNER_FOR_TESTSUITE, "TEST_SPECIAL_PATHS", errors, /short
;
if ISA(nb_errors) then nb_errors=nb_errors+errors else nb_errors=errors
;
BANNER_FOR_TESTSUITE, "TEST_SPECIAL_PATHS", errors, /status
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
; --------------------------------------------
;
pro TEST_FILE_SEARCH, help=help, test=test, verbose=verbose, $
                      no_exit=no_exit, no_erase=no_erase
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_FILE_SEARCH, help=help, test=test, verbose=verbose, $'
    print, '                      no_exit=no_exit, no_erase=no_erase'
    return
endif
;
cumul_errors=0
;
tmp_dir='TMPDIR_FILE_SEARCH'
FILE_MKDIR, tmp_dir
CD, tmp_dir, cur=cur
;
if(!version.OS_FAMILY eq "Windows") then $
	message,' Windows System: no glob test done ',/continue $
   else TEST_FILE_SEARCH_GLOB, cumul_errors, no_erase=no_erase, test=test
;
TEST_FULLY_QUALIFY_PATH, cumul_errors, no_erase=no_erase, test=test
;
TEST_SPECIAL_PATHS, cumul_errors, no_erase=no_erase, test=test
;

CD, cur
if ~KEYWORD_SET(no_erase) then FILE_DELETE, tmp_dir, /recursive
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, "TEST_FILE_SEARCH", cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

