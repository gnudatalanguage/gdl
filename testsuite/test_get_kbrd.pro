;
; under GNU GPL or later
;
; Alain Coulais, 14-Dec-2011, because Gille Duvert submits
; a patch for GET_KBRD()
;
; We have to take care that the behavior of GET_KBRD() and GET_KBRD(0)
; is different
;
; Also take care that it was no possible to call GET_KBRD()
; without arg until IDL 6.x (x > 0)
;
pro TEST_GET_KBRD
;
print, 'During 3 seconds delay, please type : azerty'
WAIT, 3
;
print, ">>", GET_KBRD(), '<<'
;
for ii=0, 5 do print, GET_KBRD(0)
;
end
;
pro TEST_GET_KBRD_ZERO
;
print, 'During 3 seconds delay, please type : azerty'
WAIT, 3
;
print, ">>", GET_KBRD(0), '<<'
;
for ii=0, 5 do print, GET_KBRD(0)
;
end
;
pro TEST_GET_KBRD_WAIT, wait=wait
;
if N_ELEMENTS(wait) EQ 0 then wait=1
if wait LE 0 then wait=1
;
print, 'During 3 seconds delay, please type : azerty'
WAIT, 3
;
print, ">>", GET_KBRD(wait), '<<'
;
for ii=0, 5 do print, GET_KBRD(wait)
;
end
