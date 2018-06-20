;
; Under GNU GPL v2 or later
; Alain Coulais, 15-Nov-2012
;
; **interactif** test of revised DIALOG_MESSAGE
;
; ------------------------------------
;
pro TEST_DIALOG_MESSAGE, no_exit=no_exit, help=help, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_DIALOG_MESSAGE, no_exit=no_exit, help=help, test=test'
   return
end
;
nb_errors=0
;
expected='OK'
result=DIALOG_MESSAGE('would you like to continue ?')
if (result NE expected) then ERRORS_ADD, 'Error case 1', nb_errors
;
result=DIALOG_MESSAGE('would you like to continue ?',/error)
if (result NE expected) then ERRORS_ADD, 'Error case 1 /Error', nb_errors
;
result=DIALOG_MESSAGE('would you like to continue ?',/information)
if (result NE expected) then  ERRORS_ADD, 'Error case 1 /Information', nb_errors
;
; with /Cancel keyword, case OK to be clicked
;
expected='OK'
result=DIALOG_MESSAGE('Please press OK/Yes',/cancel)
if (result NE expected) then ERRORS_ADD, 'Error case 2 with /cancel', nb_errors
;
result=DIALOG_MESSAGE('Please press OK/Yes',/error,/cancel)
if (result NE expected) then ERRORS_ADD, 'Error case 2 /Error with /cancel', nb_errors
;
result=DIALOG_MESSAGE('Please press OK/Yes',/information,/cancel)
if (result NE expected) then  ERRORS_ADD, 'Error case 2 /Information with /cancel', nb_errors
;
; with /Cancel keyword, case CANCEL to be clicked
;
expected='Cancel'
result=DIALOG_MESSAGE('Please press CANCEL',/cancel)
if (result NE expected) then ERRORS_ADD, 'Error case 3 with /cancel', nb_errors
;
result=DIALOG_MESSAGE('Please press CANCEL',/error,/cancel)
if (result NE expected) then ERRORS_ADD, 'Error case 3 /Error with /cancel', nb_errors
;
result=DIALOG_MESSAGE('Please press CANCEL',/information,/cancel)
if (result NE expected) then  ERRORS_ADD, 'Error case 3 /Information with /cancel', nb_errors
;
; QUESTION without /Cancel keyword, responses are in {"Yes","No"[,"Cancel"]}
;
expected='Yes'
result=DIALOG_MESSAGE('Please press YES',/question)
if (result NE expected) then ERRORS_ADD, 'Error case 4 with /question', nb_errors
;
expected='No'
result=DIALOG_MESSAGE('Please press NO',/question)
if (result NE expected) then ERRORS_ADD, 'Error case 4 with /question', nb_errors
;
expected='Yes'
result=DIALOG_MESSAGE('Please press Yes',/question, /Cancel)
if (result NE expected) then ERRORS_ADD, 'Error case 5 [Yes] with /question and /cancel', nb_errors
expected='No'
result=DIALOG_MESSAGE('Please press NO',/question, /Cancel)
if (result NE expected) then ERRORS_ADD, 'Error case 5 [No] with /question and /cancel', nb_errors
expected='Cancel'
result=DIALOG_MESSAGE('Please press Cancel',/question, /Cancel)
if (result NE expected) then ERRORS_ADD, 'Error case 5 [Cancel] with /question and /cancel', nb_errors
;
if (nb_errors EQ 0) then begin
   MESSAGE, /continue, 'All TEST_DIALOG_MESSAGE tests successful'
endif else begin
   MESSAGE, /continue, STRING(nb_errors)+' tests failed'
   if ~KEYWORD_SET(no_exit) then EXIT, status=1
endelse
;
if KEYWORD_SET(test) then STOP
;
end
