;
; AC 29/02/2008
;
pro TEST_CURSOR, option
;
defsysv, '!gdl', exist=exist
if (exist EQ 1) then begin
   if N_ELEMENTS(option) EQ 0 then option=3
endif
;
plot, findgen(10)^2
cursor, x, y, option
print, 'Data :', x, y
cursor, x, y, option, /normal
print, 'Normal', x, y
cursor, x, y, option, /device
print, 'Device', x, y
end
;
; from the IDL documentation
; You can plot lines on the window ...
;
pro DRAW  
print, 'Press Left Button to quit, other buttons to continue'
; Start with a blank screen:
ERASE  
; Get the initial point in normalized coordinates:  
CURSOR, x, y, /normal, /down  
; Repeat until right button is pressed. Get the second point.  
; Draw the line. Make the current second point be the new first.  
while (!mouse.button ne 4) do begin  
   CURSOR, x1, y1, /normal, /down  
   PLOTS,[x,x1], [y,y1], /normal  
   x = x1
   y = y1  
endwhile  
end  
