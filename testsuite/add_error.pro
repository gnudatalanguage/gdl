;
; Alain C., 4 Oct. 2015
;
; Since this procedure is now used in many tests
; and since we do have the level=-1 in MESSAGE,
; it is time to put this code in public place !
;
; -----------------------------------------------
;
pro ADD_ERROR, errors, message
;
if N_PARAMS() NE 2 then begin
    print, 'Usage : pro ADD_ERROR, count_errors, message'
    return
endif
;
MESSAGE, level=-1, 'Error on operation : '+message, /continue
;
if ISA(errors) then errors=errors+1 else errors=1
;
end
