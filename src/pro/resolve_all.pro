;
; Very preliminary version of RESOLVE_ALL
; This was enough to run Ulyss 
; http://ulyss.univ-lyon1.fr/download.html
;
; Contributions welcome !
;
; This code is under GNU GPL V2 or later
; Ilia N., Alain C., July 2015
;
pro RESOLVE_ALL, RESOLVE_EITHER=resolve_either, $
                 RESOLVE_FUNCTION=resolve_function, $
                 RESOLVE_PROCEDURE=resolve_procedure, $
                 CONTINUE_ON_ERROR=continue_on_error, UNRESOLVED=unres, $
                 class=class,  SKIP_ROUTINES=skip_routines, QUIET=quiet, $
                 help=help, test=test
;

if KEYWORD_SET(help) then begin
    print, 'pro RESOLVE_ALL, RESOLVE_EITHER=resolve_either, $'
    print, '                 RESOLVE_FUNCTION=res_fun, RESOLVE_PROCEDURE=res_pro, $'
    print, '                 CONTINUE_ON_ERROR=continue_on_error, UNRESOLVED=UNRESOLVED, $' 
    print, '                 CLASS=class,  SKIP_ROUTINES=skip_routines, QUIET=quiet, $'
    print, '                 help=help, test=test'
    print, ' '
    print, 'keyword CLASS= not ready'
    print, 'keyword UNRESOLVED= not ready'
    print, 'keyword SKIP_ROUTINES= not ready'
    return
endif
;
please='; Please contribute !'
;
if KEYWORD_SET(CLASS) then begin
    MESSAGE, 'Keyword CLASS is not ready'+please
endif
if KEYWORD_SET(SKIP_ROUTINES) then begin
    MESSAGE, 'Keyword SKIP_ROUTINES is not ready'+please
endif
if KEYWORD_SET(UNRESOLVED) then begin
    MESSAGE, 'Keyword UNRESOLVED is not ready'+please
endif
;
if KEYWORD_SET(QUIET) then begin
    quiet_save=!quiet
    !quiet = quiet
endif else begin
    quiet = 0
endelse
;
; procedures only
;
if KEYWORD_SET(resolve_procedure) then begin
    if ~KEYWORD_SET(continue_on_error) then begin
        RESOLVE_ROUTINE, resolve_procedure
    endif else begin
        for i = 0, N_ELEMENTS(resolve_procedure)-1 do begin
            command='RESOLVE_ROUTINE, resolve_procedure[i]'
            a=EXECUTE(command)
        endfor
    endelse
endif
;
; functions only
;
if KEYWORD_SET(resolve_function) then begin
    if ~KEYWORD_SET(continue_on_error) then begin
        RESOLVE_ROUTINE, resolve_function, /is_function
    endif else begin
        for i = 0, N_ELEMENTS(resolve_function)-1 do begin
            command='RESOLVE_ROUTINE, resolve_function[i], /is_function'
            a=EXECUTE(command)
        endfor
    endelse
endif
;
; procedures or functions : /either
;
if KEYWORD_SET(resolve_either) then begin
    if ~KEYWORD_SET(continue_on_error) then begin
        RESOLVE_ROUTINE, resolve_either, /either        
    endif else begin
        for i = 0, N_ELEMENTS(resolve_either)-1 do begin
            command='RESOLVE_ROUTINE, resolve_either[i], /either'
            a=EXECUTE(command)
        endfor
    endelse
endif
;
if KEYWORD_SET(QUIET) then !quiet=quiet_save
;
if KEYWORD_SET(test) then STOP
;
end
