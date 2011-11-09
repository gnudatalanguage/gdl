;
; AC 28/01/2010
;
; quick test reading back and displaying a JPEG
;
; What is tested ?
; -- reading back a small (600x259) JPEG image
; -- using well positions index in TVSCL
; -- reading a color image in Grayscale ... (09/11/2011)
;
pro TEST_READ_JPEG, filename=filename, path=path, $
                    help=help, test=test, debug=debug, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_READ_JPEG, filename=filename, path=path, factor=factor, $'
   print, '                 help=help, test=test, debug=debug, verbose=verbose'
   print, ''
   print, 'You can provide any JPEG image using FILENAME='
   print, 'We look in !Path. You can provide alternate paths with PATH='
   return
endif
;
if N_ELEMENTS(path) EQ 0 then path=!path
if N_ELEMENTS(filename) EQ 0 then filename='Saturn.jpg'
;
title0='the 3 channels in Grayscale'
title1='<<'+filename+'>> in Colors'
title2='read in Grayscale only'
;
liste_of_files=FILE_SEARCH(STRSPLIT(path,':',/ex),filename)
;
if (N_ELEMENTS(liste_of_files) EQ 1) then begin
   if (STRLEN(liste_of_files) EQ 0) then begin
      MESSAGE, /continue, 'No file founded ...'
      MESSAGE, /continue, 'File : '+filename
      MESSAGE, /continue, 'Path : '+path
      return
   endif
   one_file_and_path=liste_of_files
endif
if N_ELEMENTS(liste_of_files) GT 1 then begin
   MESSAGE, /continue, $
            'Warning: more than one file found, we used the first one !'
   one_file_and_path=liste_of_files[0]
endif
;
if KEYWORD_SET(verbose) then begin
   MESSAGE, 'reading : '+one_file_and_path, /continue
endif
;
READ_JPEG, one_file_and_path, image
;
xy_img=(SIZE(image,/dim))[1:2]
xy_screen=GET_SCREEN_SIZE()
loose=50 ; perte (surtout en vertical: la/les barre(s) de menu KDE/Gnome
xy_screen=xy_screen-loose
;
vertical=-1
; rentre-t-on dans de l'horizontal ?
if (xy_screen[0] GT 3*xy_img[0]) AND (xy_screen[1] GT xy_img[1]) then begin
   vertical=0
   xy_win0=xy_img
   xy_win0[0]=xy_win0[0]*3
   xy_win1=xy_img
endif
; rentre-t-on dans du vertical ? (on prefere ce mode-ci)
if (xy_screen[0] GT xy_img[0]) AND (xy_screen[1] GT 3*xy_img[1]) then begin
   vertical=1
   xy_win0=xy_img
   xy_win0[1]=xy_win0[1]*3
   xy_win1=xy_img
endif
;
if (vertical EQ -1) then begin
   MESSAGE, /continue, 'Soo small screen/too big image ! automatic rescaling !!'
   MESSAGE, /continue, 'Some part of the image may be cut (top and right sides)'
   ;;
   factor_h_x=ROUND(1+xy_img[0]*3/xy_screen[0])
   factor_h_y=ROUND(1+xy_img[1]/xy_screen[1])
   factor_v_x=ROUND(1+xy_img[0]/xy_screen[0])
   factor_v_y=ROUND(1+xy_img[1]*3/xy_screen[1])
   factor_h=MAX([factor_h_x,factor_h_y])
   factor_v=MAX([factor_v_x,factor_v_y])
   factor=MIN([factor_h,factor_v],vertical)
   ;;
   xy_win1=xy_img/factor
   image=REBIN(image[*,0:xy_win1[0]*factor-1,0:xy_win1[1]*factor-1],3,xy_win1[0],xy_win1[1])
   ;;
   xy_win0=xy_img/factor
   if (vertical EQ 1) then begin
      xy_win0[1]=xy_win0[1]*3
   endif else begin
      xy_win0[0]=xy_win0[0]*3
   endelse
endif
;
; just a trick to be able to test for small screens
if KEYWORD_SET(debug) then half=debug
;
WINDOW, 0, xsize=xy_win0[0], ysize=xy_win0[1], title=title0
TVSCL, image[0,*,*], 0
TVSCL, image[1,*,*], 1
TVSCL, image[2,*,*], 2
;
WINDOW, 1, xsize=xy_win1[0], ysize=xy_win1[1], title=title1
TVSCL, image, /true
;
; is the /Grayscale keyword OK ?
;
READ_JPEG, one_file_and_path, image_gray, /GRAY
WINDOW, 2, xsize=xy_win1[0], ysize=xy_win1[1], title=title2
image_gray=REBIN(image_gray[0:xy_win1[0]*factor-1,0:xy_win1[1]*factor-1],xy_win1[0],xy_win1[1])
TVSCL, image_gray
;
if KEYWORD_SET(test) then STOP
;
end

