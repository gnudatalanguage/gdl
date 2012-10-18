;
; AC 19 Mars 2008
; Testing if the LUN value increments when calling GET_LUN
;
; AC 18-Oct-2012: revisited for inclusion into automatic test suite
; (make check).
; --------------------------------------------------------------
;
pro CALL_GET_LUN, message, number, do_open=do_open, val_lun=val_lun
;
print, message+' call to GETLUN, should be :'+STRING(number)
;
GET_LUN, x
;
; do we ask for opening the file ?
if KEYWORD_SET(do_open) then OPENW, x, 'tyty'
;
print, 'Which LUN number was given :  ', x
;
val_lun=x
;
end
;
; --------------------------------------------------------------
;
pro TEST_GET_LUN, no_exit=no_exit, help=help, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_GET_LUN, no_exit=no_exit, help=help, test=test'
   return
end
;
nb_errors1=0
nb_errors2=0
;
line='--------------------------------------------------'
print, line
print, 'Explicitly opened files'
print
do_open=1
CALL_GET_LUN, 'FIRST ', 100, do_open=do_open, val_lun=x
CALL_GET_LUN, 'SECOND', 101, do_open=do_open, val_lun=y
CALL_GET_LUN, 'THIRD ', 102, do_open=do_open, val_lun=z
;
if (x NE 100) then nb_errors1++
if (y NE 101) then nb_errors1++
if (z NE 102) then nb_errors1++
;
FREE_LUN, x, y, z
;
if (nb_errors1 GT 0) then begin
    txt='errors in Explicitly opened files'
    MESSAGE, /continue, STRING(nb_errors1)+txt
endif
;
print, '--------------------------------------------------'
print, 'Whithout really opened files'
do_open=0
print
CALL_GET_LUN, 'FIRST ', 100, do_open=do_open, val_lun=x
CALL_GET_LUN, 'SECOND', 101, do_open=do_open, val_lun=y
CALL_GET_LUN, 'THIRD ', 102, do_open=do_open, val_lun=z
;
FREE_LUN, x, y, z
;
if (x NE 100) then nb_errors2++
if (y NE 101) then nb_errors2++
if (z NE 102) then nb_errors2++
;
if (nb_errors2 GT 0) then begin
    txt=' errors in not really opened files'
    MESSAGE, /continue, STRING(nb_errors2)+txt
endif
;
; ----------------------------------------------------
;
; if we are here and nb_pbs > 0 then /No_exit was set.
;
print, line
;
nb_errors=nb_errors1+nb_errors2
;
MESSAGE, /continue, 'Final diagnostic :'
if (nb_errors GT 0) then begin
      MESSAGE, /continue, STRING(nb_errors)+' tests related to GET_LUN failed'
      if ~KEYWORD_SET(no_exit) then EXIT, status=1
endif else begin
      MESSAGE, /continue, 'All GET_LUN tests successful'
endelse
;
if KEYWORD_SET(test) then STOP
;
end
