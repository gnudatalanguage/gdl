;
; Alain C.
; 2015-Oct-07
; Under GNU GPL 3+
;
; Current limitation : redundancy not managed !
;  (a dir. can be added many times)
;
; purpose : simplest way to add PATHs into !PATH,
; without taking into account the syntax of EXPAND_PATH(),
; and giving warning if needed (path not found)
;
; Several equivalents do exist (e.g. : cgaddtopath.pro in Coyote Lib.)
; but they are not "inside" the basic code, or do not manage list, 
; or don't have prep/app option ...
;
pro PATH_ADD, list_of_paths, current=current, recursive=recursive, $ 
              append=append, after=after, $
              prepend=prepend, before=before, $
              help=help, verbose=verbose, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro PATH_ADD, list_of_paths, current=current, recursive=recursive, $'
    print, '              append=append, after=after, $'
    print, '              prepend=prepend, before=before, $'
    print, '              help=help, verbose=verbose, test=test'
    return
endif
;
if ((N_PARAMS() EQ 0) AND (~KEYWORD_SET(current))) then begin
    print, 'USAGE : pro PATH_ADD, paths [,/current][,/recursive][,/prepend][,/append][,/help][,/verbose]'
    return
endif
;
; adding current if requested
;
if KEYWORD_SET(current) then begin
    CD, current=current
    if (N_PARAMS() EQ 0) then list_of_paths=current else list_of_paths=[current,list_of_paths]
endif
;
; managing (append, after) versus (prepend, before).
; Default is adding paths at the beginng !
;
if KEYWORD_set(append) or KEYWORD_set(after) then at_end=1 else at_end=0
if KEYWORD_set(prepend) or KEYWORD_set(before) then at_begin=1 else at_begin=0
;
if ((at_end EQ 1) AND (at_begin EQ 1)) then begin
    print, 'Exclusive options '
endif
;
if ((at_end EQ 0) AND (at_begin EQ 0)) then begin
    print, 'No explicit position : the defaut is AT THE BEGINNING'
    at_begin=1
endif
;
if KEYWORD_SET(verbose) then begin
    print, '--------------------- !PATH before ---------------'
    PATH_SHOW, /count
endif
;
path_sep=PATH_SEP(/search)
if KEYWORD_SET(recursive) then rec='+' else rec=''
;
for ii=0, N_ELEMENTS(list_of_paths)-1 do begin
    path=FILE_SEARCH(list_of_paths[ii], /FULLY_QUALIFY_PATH)
    if FILE_TEST(path,/dir) then begin
        if at_begin then begin
            !path=EXPAND_PATH(rec+path)+path_sep+!path
        endif else begin
            !path=!path+path_sep+EXPAND_PATH(rec+path)
        endelse
    endif else begin
        print, 'SKIPPED bad input path : ', list_of_paths[ii]
    endelse
endfor
;
if KEYWORD_SET(verbose) then begin
    print, '--------------------- !PATH after ---------------'
    info=['AT THE END','AT THE BEGINNG']
    print, 'the path(s) to add have been added at : ' +info[at_begin]
    PATH_SHOW, /count
endif
;
if KEYWORD_SET(test) then STOP
;
end
