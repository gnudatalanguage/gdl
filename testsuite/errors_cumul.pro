;
; Alain C., 28 Feb. 2017
;
; managing Errors Accumulation
;
; In order to test fake errors accumulations in the tests, 
; you can use :
;
; ERRORS_CUMUL, /debug 
;
; -----------------------------------------------
;
pro ERRORS_CUMUL, cumul_errors, new_errors, help=help, $
                  debug=debug, verbose=verbose, test=test
;
if KEYWORD_SET(debug) then begin
    DEFSYSV, '!cumul', 1
    MESSAGE, /continue, 'Fake error will be added now'
    return
endif
;
if N_PARAMS() NE 2 then begin
    print, 'Usage : pro ERRORS_CUMUL, cumul_errors, new_errors, help=help, $'
    print, '                          debug=debug, verbose=verbose, test=test'
    return
endif
;
if KEYWORD_SET(verbose) then begin
    print, 'Value of >>cumul_errors<< : ', cumul_errors
    print, 'Value of >>new_errors<<   : ', new_errors
endif
;
if ISA(cumul_errors) then begin
    if ISA(new_errors) then begin
        cumul_errors=cumul_errors+new_errors
    endif
    ;; nothing to add if "new_errors" not defined
endif else begin
    if ISA(new_errors) then begin
        cumul_errors=new_errors
    endif
endelse
;
if KEYWORD_SET(verbose) then begin
    print, 'NEW Value of >>cumul_errors<< : ', cumul_errors
endif
;
; debug mode
DEFSYSV, '!cumul', exist=exist
if (exist) then if ISA(cumul_errors) then cumul_errors++ else cumul_errors=1
;
if KEYWORD_SET(verbose) then begin
    print, 'NEW Value of >>cumul_errors<< : ', cumul_errors
endif
;
if KEYWORD_SET(test) then STOP
;
end
