;
; Alain C., March 3, 2016
;
; Because of various usages (comaprison with other interpretors,
; locale tests), we may be in a situation where the current path
; (ie testsuite/) is not in the !PATH
; then we add it at the first position when looking for file !
;
; When more than one file is found, we always return the first one
;
function FILE_SEARCH_FOR_TESTSUITE, filename, help=help, test=test, $
                                    quiet=quiet, verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'function FILE_SEARCH_FOR_TESTSUITE, filename, help=help, test=test, $'
    print, '                                    quiet=quiet, verbose=verbose'
    return, ''
endif 
;
; we will add current dir. to the !Path in first position
CD, current=current
;
list_of_dirs=STRSPLIT(!PATH, PATH_SEP(/SEARCH_PATH), /EXTRACT)
;
list_of_dirs=[current,list_of_dirs]
;
; it i s important to work in current directory !!
full_file=FILE_SEARCH(list_of_dirs+PATH_SEP()+filename, /nosort)
;
; We may have multiplicity ... we select the first one (priority to
; current dir. thanks to /nosort)
;
if (N_ELEMENTS(full_file) GT 1) then begin
    if ~KEYWORD_SET(quiet) then begin
        print, 'Multiple occurences of File <<'+filename+'>> in !Path'
        print, TRANSPOSE(full_file)
        print & print, 'First one to be selected'
    endif
    ;; selection of first one
    full_file=full_file[0]
endif
;
if KEYWORD_SET(verbose) then begin
    if STRLEN(full_file) GT 0 then begin
        print, full_file
    endif else begin
        print, 'No File <<'+filename+'>> in !Path + current dir.'
    endelse
endif
;
if KEYWORD_SET(test) then STOP
;
return, full_file
;
end
