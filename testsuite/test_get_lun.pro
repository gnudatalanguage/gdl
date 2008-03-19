;
; AC 19 Mars 2008
; Testing if the LUN value increments when calling GET_LUN
;
pro MESSAGE_LUN, message, number, do_open=do_open, val_lun=val_lun
;
print, message+' call to GETLUN, should be :'+STRING(number)
get_lun, x
if KEYWORD_SET(do_open) then OPENW, x,'tyty'
print, 'Which LUN number was given :  ', x
print
;
val_lun=x
;
end
;
pro TEST_GET_LUN
;
print, '--------------------------------------------------'
print, 'Explicitly opening files'
print
do_open=1
MESSAGE_LUN, 'FIRST ', 100, do_open=do_open, val_lun=x
MESSAGE_LUN, 'SECOND', 101, do_open=do_open, val_lun=y
MESSAGE_LUN, 'THIRD ', 102, do_open=do_open, val_lun=z
;
FREE_LUN, x, y, z
;
print, '--------------------------------------------------'
print, 'Whithout really opening files'
do_open=0
print
MESSAGE_LUN, 'FIRST ', 100, do_open=do_open, val_lun=x
MESSAGE_LUN, 'SECOND', 101, do_open=do_open, val_lun=y
MESSAGE_LUN, 'THIRD ', 102, do_open=do_open, val_lun=z
;
FREE_LUN, x, y, z
;
end
