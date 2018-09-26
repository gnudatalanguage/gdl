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
pro TESTING_GET_SCREEN_SIZE, cumul_errors, test=test
;
nb_errors=0
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
if (taille[0] LE 0) then $
   ERROTS_ADD, nb_errors, 'Error, screen length < 0'
if (taille[1] LE 0) then $
   ERROTS_ADD, nb_errors, 'Error, screen height < 0'
if (resolution[0] LE 0) then $
   ERROTS_ADD, nb_errors, 'Error, length resolution < 0'
if (resolution[1] LE 0) then $
   ERROTS_ADD, nb_errors, 'Error, height resolution < 0'
;
pixel_aspect_ratio=resolution[1]/resolution[0]
if ((pixel_aspect_ratio LT 0.95) OR (pixel_aspect_ratio GT 1.05)) then begin
   ERROTS_ADD, nb_errors, 'the pixels of this screen are not square !'
endif
;
screen_aspect_ratio=float(taille[1])/float(taille[0])
if ((screen_aspect_ratio LT 0.5) OR (screen_aspect_ratio GT 2.)) then begin
   ERROTS_ADD, nb_errors, 'this screen has strange aspect ratio !! (< 0.5 or > 2)'
endif
;
if (ISA(taille, 'Long')) then begin
   ERROTS_ADD, nb_errors, 'The return value is not a "Long" type !'
endif
;	
BANNER_FOR_TESTSUITE, 'TESTING_GET_SCREEN_SIZE', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------------------
;
pro TEST_GET_SCREEN_SIZE, help=help, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_GET_SCREEN_SIZE, help=help, no_exit=no_exit, test=test'
    return
endif
;
if GETENV('DISPLAY') eq '' then begin
    ERRORS_ADD, nb_errors, 'apparently no X connection is available (DISPLAY env. var. not set)'
    EXIT, status=77
endif 
;
TESTING_GET_SCREEN_SIZE, nb_errors, test=test
;
BANNER_FOR_TESTSUITE, 'TEST_GET_SCREEN_SIZE', nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
