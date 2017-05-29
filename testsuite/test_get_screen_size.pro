;
; AC 18/10/2009 under GNU/GPL 2 or later
;
; resived 03/05/2010 for integration in the "make check" testsuite
;
; purpose: quickly testing the GET_SCREEN_SIZE() function  
;
; ---------------------------------------------------
;
pro DEMO_GET_SCREEN_SIZE
;
sep = '=============================='
;
print,  sep
print, 'basic test (call without Display name)'
taille=GET_SCREEN_SIZE(resolution=resolution)
print, 'Screen Size (in pixels) :', taille
print, 'Pixel Size (in mm) :', resolution
;
print,  sep
SPAWN, 'echo $DISPLAY',  display
print, 'On current Display, using "'+display+'" as name'
taille=GET_SCREEN_SIZE(display, resolution=resolution)
print, 'Screen Size (in pixels) :', taille
print, 'Pixel Size (in mm) :', resolution
;
print,  sep
print, 'On current Display, using ":0" as name'
print, '(may give <<Xlib: connection to ":0.0" refused by server>> on remote)'
display=':0'
taille=GET_SCREEN_SIZE(display, resolution=resolution)
print, 'Screen Size (in pixels) :', taille
print, 'Pixel Size (in mm) :', resolution
;;
print,  sep
;
end
;
; ---------------------------------------------------
;
pro TESTING_GET_SCREEN_SIZE, test=test, exit_on_error=exit_on_error
;
print, 'basic test (call without Display name)'
taille=GET_SCREEN_SIZE(resolution=resolution)
print, 'Screen Size (in pixels) :', taille
print, 'Pixel Size (in mm) :', resolution
;
; are the values "reasonables" ?
; 1/ positives values
; 2/ (non blocking) quasi square pixels
; 3/ (non blocking) non excessive aspect ratio for the whole screen
;
nb_doubts=0
nb_pbs=0
if (taille[0] LE 0) then nb_pbs=nb_pbs+1
if (taille[1] LE 0) then nb_pbs=nb_pbs+1
if (resolution[0] LE 0) then nb_pbs=nb_pbs+1
if (resolution[1] LE 0) then nb_pbs=nb_pbs+1
;
pixel_aspect_ratio=resolution[1]/resolution[0]
if ((pixel_aspect_ratio LT 0.95) OR (pixel_aspect_ratio GT 1.05)) then begin
   nb_doubts=nb_doubts+1
   MESSAGE, /continue, 'the pixels of this screen are not square !'
endif
;
screen_aspect_ratio=taille[1]/taille[0]
if ((screen_aspect_ratio LT 0.5) OR (screen_aspect_ratio GT 2.)) then begin
   nb_doubts=nb_doubts+1
   MESSAGE, /continue, 'this screen has strange aspect ration !! (> 2)'
endif
;
if (nb_pbs GT 0) then begin
   MESSAGE, STRING(nb_pbs)+' problem(s) found (size(s) <= 0 !!)', /continue
   if KEYWORD_SET(exit_on_error) then EXIT, status=1
endif else begin
   MESSAGE, 'No problem found', /continue
endelse
;
if (nb_doubts GT 0) then begin
   MESSAGE, STRING(nb_doubts)+' doubt(s) found (see the values)', /continue
endif else begin
   MESSAGE, 'No doubt found', /continue
endelse
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------------------
;
pro TEST_GET_SCREEN_SIZE
;;
if GETENV('DISPLAY') eq '' then begin
    MESSAGE, /continue, $
      'apparently no X connection is available (DISPLAY env. var. not set)'
    EXIT, status=77
endif 
;;
TESTING_GET_SCREEN_SIZE, /exit_on_error
;;
end
