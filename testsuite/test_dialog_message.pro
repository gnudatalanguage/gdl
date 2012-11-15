;
; Under GNU GPL v2 or later
; Alain Coulais, 15-Nov-2012
;
; **interactif** test of revised DIALOG_MESSAGE
;
pro ADD_ERROR, texte, error
MESSAGE, /cont, texte
error=error+1
end
;
; ------------------------------------
;
pro TEST_DIALOG_MESSAGE

nb_errors=0
;
expected='OK'
result=DIALOG_MESSAGE('would you like to continue ?')
if (result NE expected) then ADD_ERROR, 'Error case 1', nb_errors
;
result=DIALOG_MESSAGE('would you like to continue ?',/error)
if (result NE expected) then ADD_ERROR, 'Error case 1 /Error', nb_errors
;
result=DIALOG_MESSAGE('would you like to continue ?',/information)
if (result NE expected) then  ADD_ERROR, 'Error case 1 /Information', nb_errors
;
; with /Cancel keyword, case OK to be clicked
;
expected='OK'
result=DIALOG_MESSAGE('Please press OK',/cancel)
if (result NE expected) then ADD_ERROR, 'Error case 1 with /cancel', nb_errors
;
result=DIALOG_MESSAGE('Please press OK',/error,/cancel)
if (result NE expected) then ADD_ERROR, 'Error case 1 /Error with /cancel', nb_errors
;
result=DIALOG_MESSAGE('Please press OK',/information,/cancel)
if (result NE expected) then  ADD_ERROR, 'Error case 1 /Information with /cancel', nb_errors
;
; with /Cancel keyword, case CANCEL to be clicked
;
expected='Cancel'
result=DIALOG_MESSAGE('Please press CANCEL',/cancel)
if (result NE expected) then ADD_ERROR, 'Error case 1 with /cancel', nb_errors
;
result=DIALOG_MESSAGE('Please press CANCEL',/error,/cancel)
if (result NE expected) then ADD_ERROR, 'Error case 1 /Error with /cancel', nb_errors
;
result=DIALOG_MESSAGE('Please press CANCEL',/information,/cancel)
if (result NE expected) then  ADD_ERROR, 'Error case 1 /Information with /cancel', nb_errors
;
end
