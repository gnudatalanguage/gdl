;
; Under GNU GPL v2 or later
; Alain C., 24-Jan-2012
;
; tested on various Linux and OSX ...
;
function GET_LOGIN_INFO
;
machine_name='Unknow'
user_name='Unknow'
;
; it seems that it is better to use 'hostname' insteed 'hostname -s'
;
SPAWN, 'hostname', resu, exit_status=exit_status
if (exit_status EQ 0) then machine_name=resu
;
SPAWN, 'whoami', resu, exit_status=exit_status
if (exit_status EQ 0) then user_name=resu
;
return, {MACHINE_NAME: machine_name, $
         USER_NAME: user_name}
;
end
;
