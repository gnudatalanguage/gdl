;
; AC 28/01/2010
;
; quick test reading back and displaying a JPEG
;
; What is tested ?
; -- reading back a small (600x259) JPEG image
; -- using well positions index in TVSCL
;
pro TEST_READ_JPEG, path=path, test=test, debug=debug
;
if N_ELEMENTS(path) EQ 0 then path=''
;
READ_JPEG, path+'Saturn.jpg', image
;
xy_img=(SIZE(image,/dim))[1:2]
xy_screen=GET_SCREEN_SIZE()
loose=50
;
half=0
if ((xy_screen[0]-loose) LT xy_img[0]) then half=1
if ((xy_screen[1]-loose) LT 3*xy_img[1]) then half=half+2
;
; just a trick to be able to test for small screens
if KEYWORD_SET(debug) then half=debug
if KEYWORD_SET(test) then STOP
;
; "big" screen
if (half EQ 0) then begin
   im_big=image   
   xy_win0=xy_img
   xy_win1=xy_img
endif
;
; "small" screen in width
if (half EQ 1) then begin
   image=REBIN(image[*,*,0:xy_img[1]-2],3,xy_img[0]/2,xy_img[1]/2)
   im_big=image   
   xy_win0=xy_img/2
   xy_win1=xy_img/2
endif
;
; "small" screen in high (vertical. e.g. 1280x800 screen not OK ;-)
; Nevertheless we can display the full image !
if (half EQ 2) then begin
   im_big=image
   image=REBIN(image[*,*,0:xy_img[1]-2],3,xy_img[0]/2,xy_img[1]/2)
   xy_win0=xy_img/2
   xy_win1=xy_img
endif
;
; "small" screen in two directions ...
if (half EQ 3) then begin
   moitie=0
   if ((xy_screen[0]-loose) LT xy_img[0]/2) then moitie=1
   if ((xy_screen[1]-loose) LT (3*xy_img[1])/2) then moitie=1
   if moitie EQ 1 then begin
      print, 'soo small screen ! nothing done'
      return
   endif
   image=REBIN(image[*,*,0:xy_img[1]-2],3,xy_img[0]/2,xy_img[1]/2)
   im_big=image
   xy_win0=xy_img/2
   xy_win1=xy_img/2
   half=0
endif
;
; from here, flag in {0,1,2}
;
WINDOW, 0, xsize=xy_win0[0], ysize=3*xy_win0[1], title='3 channels in greyscale'
TVSCL, image[0,*,*], 0
TVSCL, image[1,*,*], 1
TVSCL, image[2,*,*], 2
;
WINDOW, 1, xsize=xy_win1[0], ysize=xy_win1[1], title='Saturn in Colors'
TVSCL, im_big, /true
;
if KEYWORD_SET(test) then STOP
;
end
