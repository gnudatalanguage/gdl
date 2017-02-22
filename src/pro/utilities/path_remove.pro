;
; Alain C.
; 2015-Oct-07
; Under GNU GPL 3+
;
; Current limitation : redundancy not managed !
;  (a dir. can be added many times)
;
; purpose : simplest way to remove PATHs into !PATH,
; without taking into account the syntax of EXPAND_PATH(),
; and giving warning if needed (path not found)
;
; Several equivalents do exist (e.g. : cgaddtopath.pro in Coyote Lib.)
; but they are not "inside" the basic code, or do not manage list, 
; or don't have prep/app option ...
;
; "lists_to_remove" can be a list of full qualified PATHs or just strings ...
;
pro PATH_REMOVE, list_to_remove, current=current, $
                 help=help, verbose=verbose, quiet=quiet, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro PATH_REMOVE, list_to_remove, current=current, $'
    print, '                 help=help, verbose=verbose, quiet=quiet, test=test'
    return
endif
;
if ((N_PARAMS() EQ 0) AND (~KEYWORD_SET(current))) then begin
    print, 'USAGE : pro PATH_REMOVE, list_to_remove [,/current][,/help][,/verbose][,/quiet]'
    return
endif
;
; first of all, we need to establish an array of PATHs
;
path_sep=PATH_SEP(/search)
;
list_of_paths=STRSPLIT(!path, path_sep, /extract)
;
; removing current if requested
;
if KEYWORD_SET(current) then begin
    CD, current=current
    indices=STRPOS(list_of_paths, current)
    keep=WHERE(indices LT 0, nb_keep)
    if (keep GT 0) then begin
        list_of_paths=list_of_paths[keep]
    endif else begin
        list_of_paths=''
    endelse
endif
;
if KEYWORD_SET(verbose) then begin
    print, '--------------------- !PATH before ---------------'
    PATH_SHOW
endif
;
for ii=0, N_ELEMENTS(list_to_remove)-1 do begin
    indices=STRPOS(list_of_paths, list_to_remove[ii])
    keep=WHERE(indices LT 0, nb_keep)
    if (nb_keep GT 0) then begin
        list_of_paths=list_of_paths[keep]
    endif else begin
        list_of_paths=''
        BREAK
    endelse
endfor
;
!path=list_of_paths[0]
for ii=1, N_ELEMENTS(list_of_paths)-1 do !path=!path+path_sep+list_of_paths[ii]
;
if KEYWORD_SET(verbose) then begin
    print, '--------------------- !PATH after ---------------'
    PATH_SHOW
endif
;
if KEYWORD_SET(test) then STOP
;
end
