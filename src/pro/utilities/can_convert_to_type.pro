;
; Alain C, 2026-JAN-23
;
; In READ_CSV(), we need to check whether we can convert a
; string into a long or a float. One way is to use a REGEX,
; another is to use FIX() but FIX() is not so easy and gives
; a message when conversion can not be done. This function catches
; the error message. (I tried with catch without success)
;
; This function is used in READ_CSV()
;
; examples :
; GDL> conv2type('z123eeee',4)
;       0
; GDL> conv2type('123eeee',4)
;       1
;
; --------------------------------------------------
;
; The Art to test whether we can convert a string
; into a FLOAT or a INT/LONG is not a Science ! Eg :
;
; GDL> print, fix('123eeee', type=3)
;         123
; GDL> print, fix('123eeee', type=4)
;      123.000
; GDL> print, fix('123eeee', type=5)
;       123.00000
;
function CAN_CONVERT_TO_TYPE, arg1in, type
;
ON_IOERROR, bad
arg1 = FIX(arg1in, type=type)
return, 1
bad:
;print, !error_state.msg
;print, 'defaulting to arg1 = 0'
message, /reset ; make sure to clear our error
on_ioerror, null ; reset error handler
return, 0
;
end
