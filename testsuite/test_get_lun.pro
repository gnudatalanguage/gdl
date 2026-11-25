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
function TEST_GET_LUN_SOCKET_THROWS, unit, host
catch, err
if err ne 0 then begin
   catch, /cancel
   return, 1
endif
socket, unit, host, 0, /get_lun
return, 0
end
;
pro TEST_GET_LUN_SOCKET, errors
get_lun, expected
free_lun, expected
unit = -1L
; The socket implementation uses IPv4. This numeric IPv6 address fails lookup locally.
socket, unit, '::1', 0, /get_lun, error=status
if status eq 0 or unit ne -1L then ERRORS_ADD, errors, 'Failed SOCKET returned a LUN'
; Exercise a connection failure and an argument error caught before opening the socket.
if ~TEST_GET_LUN_SOCKET_THROWS(unit, '127.0.0.1') then ERRORS_ADD, errors, 'SOCKET accepted port zero'
if ~TEST_GET_LUN_SOCKET_THROWS(unit, ['a','b']) then ERRORS_ADD, errors, 'SOCKET accepted a host array'
get_lun, held
if held ne expected then ERRORS_ADD, errors, 'Failed SOCKET leaked an automatic LUN'
; A caller-owned reservation must survive the same failure.
socket, held, '::1', 0, error=status
get_lun, next
if status eq 0 or next eq held then ERRORS_ADD, errors, 'Failed SOCKET released a caller-owned LUN'
free_lun, held, next
end
;
function TEST_SOCKET_PORT_ERROR, unit, port
catch, err
if err ne 0 then begin
   msg = !error_state.msg
   catch, /cancel
   return, msg
endif
; The unit is already open. A valid port reaches the open error handled by ERROR=.
socket, unit, '127.0.0.1', port, error=status
return, ''
end
;
pro TEST_SOCKET_PORT_RANGE, errors
openw, u, 'test_socket_port.tmp', /get_lun, /delete
foreach port, list(-1, 65536ul, 70000L, -4294967296LL, 4294967296ULL, 18446744073709551615ULL) do begin
   if TEST_SOCKET_PORT_ERROR(u, port) eq '' then $
      ERRORS_ADD, errors, 'SOCKET accepted out-of-range port '+strtrim(port,2)
endforeach
foreach port, list(0, 65535u, [65535ull]) do begin
   msg = TEST_SOCKET_PORT_ERROR(u, port)
   if msg ne '' then ERRORS_ADD, errors, 'SOCKET rejected valid port '+strtrim(port[0],2)+' '+msg
endforeach
free_lun, u
end
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
defsysv, '!GDL', exists=is_gdl
if is_gdl then TEST_GET_LUN_SOCKET, nb_errors
TEST_SOCKET_PORT_RANGE, nb_errors
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
