;
; Alain C.
; 2015-Oct-07
; Under GNU GPL 3+
;
; Current limitation : redundancy not managed !
;  (a dir. can be added/show many times)
;
; purpose : simplest way to display a *smart* list of the paths.
; an equivalent of HELP, /path_cache as is in GDL
; Unfortunatelly, the IDL HELP, /path_cache does not take
; into account change in !PATH
;
pro PATH_SHOW, count=count, full_count=full_count, help=help, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro PATH_SHOW, count=count, full_count=full_count, help=help, test=test'
    return
endif
;
path_sep=PATH_SEP(/search)
;
list_of_paths=STRSPLIT(!path, path_sep, /extract)
;
if ~KEYWORD_SET(count) AND ~KEYWORD_SET(full_count) then begin
    print, 'Without counting files per Directories :'
    print, TRANSPOSE(list_of_paths)
endif 
;
if KEYWORD_SET(full_count) then begin
    print, 'Count .pro and .sav files per Directories :'
    for ii=0, N_ELEMENTS(list_of_paths)-1 do begin
        tmp=FILE_SEARCH(list_of_paths[ii]+PATH_SEP()+'*.pro', count=count1, /fold_case)
        tmp=FILE_SEARCH(list_of_paths[ii]+PATH_SEP()+'*.sav', count=count2, /fold_case)
        print, list_of_paths[ii]+' ('+STRCOMPRESS(STRING(count1+count2),/remove_all)+' files)'
    endfor
endif else begin
    if KEYWORD_SET(count) then begin
        print, 'Count .pro files per Directories :'
        for ii=0, N_ELEMENTS(list_of_paths)-1 do begin
            tmp=FILE_SEARCH(list_of_paths[ii]+PATH_SEP()+'*.pro', count=count, /fold_case)
            print, list_of_paths[ii]+' ('+STRCOMPRESS(STRING(count),/remove_all)+' files)'
        endfor
    endif
endelse
print, STRING(N_ELEMENTS(list_of_paths))+'PATHs defined'
;
if KEYWORD_SET(test) then STOP
;
end
