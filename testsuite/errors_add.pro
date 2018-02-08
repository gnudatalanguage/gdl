;
; Alain C., 4 Oct. 2015
;
; Since this procedure is now used in many tests
; and since we do have the level=-1 in MESSAGE,
; it is time to put this code in public place !
;
; Feb. 2018 : Change in naming convention : 
; ERRORS_CUMUL, ERRORS_ADD, ERRORS_RESET
;
; -----------------------------------------------
;
; Purpose : this procedure prints a "message" and
; adds "1" into "nb_errors"
;
; It would surprising if "message" is undefined
; It is *not* surprising if "nb_errors" is undefined
; 
; -----------------------------------------------
;
pro ERRORS_ADD, nb_errors, message
;
if N_PARAMS() NE 2 then begin
   print, 'Usage : pro ERRORS_ADD, nb_errors, message'
   return
endif
;
if ISA(message) then begin
   txt=STRING(message) 
endif else begin
   MESSAGE, /continue, 'We advice to defined a message !'
   txt='Missing Message !'
endelse
;
MESSAGE, level=-1, 'Error on operation : '+txt, /continue
;
; we will have to change this code for old IDL versions
; (no ISA before 8)
;
if ISA(nb_errors) then nb_errors=nb_errors+1 else nb_errors=1
;
end

