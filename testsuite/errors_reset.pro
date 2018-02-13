;
; Alain C., 4 Oct. 2015
;
; Feb. 2018 : Change in naming convention : 
; ERRORS_CUMUL, ERRORS_ADD, ERRORS_RESET
;
; -----------------------------------------------
;
; Purpose : this procedure adds running "nb_errors" into "cumul_errors"
; then reset "nb_errors" to 0 for next block of tests.
;
; It would surprising if "nb_errors" is undefined
; It is *not* surprising that "cumul_errors" may be undefined
; 
; -----------------------------------------------
;
pro ERRORS_RESET, cumul_errors, nb_errors, verbose=verbose, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro ERRORS_RESET, cumul_errors, nb_errors, verbose=verbose, help=help'
   return
endif
;
if N_PARAMS() NE 2 then begin
    print, 'Usage : pro ERRORS_RESET, cumul_errors, nb_errors, verbose=verbose'
    return
endif
;
if KEYWORD_SET(verbose) then print, cumul_errors, nb_errors
;
; it would be surprising if "nb_errors" don't have a value ...
if (SIZE(nb_errors, /type) EQ 0) then $
   MESSAGE, /continue, 'Please check why Nb_Errors is not defined ...'
;
; it is *not* surprining that "cumul_errors" may be undefined
;
if (SIZE(cumul_errors, /type) GT 0) then begin
   if (SIZE(nb_errors, /type) GT 0) then begin
      cumul_errors=cumul_errors+nb_errors
   endif
   ;; no else needed, no change to "cumul_errors"
endif else begin
   if (SIZE(nb_errors, /type) GT 0) then cumul_errors=nb_errors else cumul_errors=0
endelse
;
; reset the running "nb_errors" number
;
nb_errors=0
;
end
