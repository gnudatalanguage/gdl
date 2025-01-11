;
; Very preliminary version of RESOLVE_ALL
; This was enough to run Ulyss 
; http://ulyss.univ-lyon1.fr/download.html
;
; Contributions welcome !
;
; This code is under GNU GPL V2 or later
; Ilia N., Alain C., July 2015
; Gilles D., January 2025 ... now really working except for objects (TBC).
;
function resolve_again, quiet, cont, isfunc, SkipList
    COMPILE_OPT hidden, nosave
    on_error, 2     ; Return to caller if error not caught
    numberOfUnresolved = 0
    numberOfSkipElements = n_elements(SkipList)
    localUnresolvedList = ROUTINE_INFO(FUNCTIONS=isfunc, /UNRESOLVED)
    if numberOfSkipElements gt 0 then begin
        ; Remove skip routines from the unresolved list by agglomerating non-matches to start of list
        j = 0L
        for i=0, n_elements(localUnresolvedList)-1 do if total(localUnresolvedList[i] eq SkipList) eq 0 then begin
            localUnresolvedList[j] = localUnresolvedList[i]
            j = j + 1
        endif
        if j gt 0 then localUnresolvedList = localUnresolvedList[0:j-1] else localUnresolvedList = ''
     endif
    ; something stays to be resolved:
    if strlen(localUnresolvedList[0]) gt 0 then begin
        num = n_elements(localUnresolvedList)
        numberOfUnresolved = numberOfUnresolved + num
        for i=0, num-1 do begin
            name = localUnresolvedList[i]
            if (cont) then catch, error else error = 0
            if error eq 0 then begin
                resolve_routine, name, IS_FUNCTION=isfunc
            endif else begin
                ; Current routine was not found. Let the user see the
                ; error, and then add it to the skip list.
                if quiet eq 0 then print,!error_state.msg
                SkipList = numberOfSkipElements gt 0 ? [SkipList, name] : [name]
                numberOfSkipElements = numberOfSkipElements + 1
                catch, /cancel
                MESSAGE, /RESET
            endelse
        endfor
    endif
    return, numberOfUnresolved
end


pro RESOLVE_ALL, RESOLVE_EITHER=resolve_either, $
                 RESOLVE_FUNCTION=resolve_function, $
                 RESOLVE_PROCEDURE=resolve_procedure, $
                 CONTINUE_ON_ERROR=continue_on_error, UNRESOLVED=SkipList, $
                 class=class,  SKIP_ROUTINES=skip_routines, QUIET=quiet

compile_opt hidden, idl2
;
ON_ERROR, 2

if KEYWORD_SET(CLASS) then begin
    MESSAGE, 'Keyword CLASS is not ready'+please
endif
;
if KEYWORD_SET(QUIET) then begin
    quiet_save=!quiet
    !quiet = quiet
endif else begin
    quiet = 0
endelse

if n_elements(skip_routines) gt 0 then SkipList = strupcase(skip_routines)

doContinue=keyword_set(continue_on_error)
; this throws error
    if keyword_set(resolve_either) then $
    resolve_routine, /NO_RECOMPILE, /EITHER, resolve_either

    if keyword_set(resolve_procedure) then $
    resolve_routine, /NO_RECOMPILE, resolve_procedure

    if keyword_set(resolve_function) then $
    resolve_routine, /NO_RECOMPILE, /IS_FUNCTION, resolve_function
; this accept continue.
    repeat begin
        cnt = 0
        cnt = cnt + resolve_again(quiet, doContinue, 0, SkipList)
        cnt = cnt + resolve_again(quiet, doContinue, 1, SkipList)
    endrep until cnt le 0
;
if KEYWORD_SET(QUIET) then !quiet=quiet_save
;
end
