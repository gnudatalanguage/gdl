;
; AC, 5-Feb-2012
;
; the goal of this program is to quickly check
; that reading and displaying known images (JPEG and PNG)
; is OK
;
; based on exchanges on 
; https://sourceforge.net/projects/gnudatalanguage/forums/forum/338692/topic/4992595/index/page/1
;
function TEST_DOWNLOAD_TOOLS, verbose=verbose
;
wget_ok=0
SPAWN, 'which wget', res
if STRLEN(res) GT 0 then begin
   wget_ok=1
   script='wget '
   return, script
endif
;
curl_ok=0
SPAWN, 'which curl', res
if STRLEN(res) GT 0 then begin
   curl_ok=1
   script='curl -O '
   return, script
endif
;
if ((wget_ok EQ 0) AND (curl_ok EQ 0)) then begin
   MESSAGE, /continue, 'No download tool (wget, curl) found'
   MESSAGE, /continue, 'If need you can download by hand or add one of those tools'
   STOP
endif
;
end
;
; -----------------------------------------------------------
;
pro CHECK_IMAGES, list_of_names, links_to_images, download=download
;
if KEYWORD_SET(download) then script=TEST_DOWNLOAD_TOOLS()
;
nb_images=N_ELEMENTS(list_of_names)
;
; first turn: if image is missing and we ask for download, we try to download
;
for ii=0, nb_images-1 do begin
   flag=QUERY_IMAGE(list_of_names[ii], info)
   if (flag EQ 0) then begin
      print, 'Missing Image : ', list_of_names[ii]
      if KEYWORD_SET(download) then begin
         SPAWN, script+links_to_images[ii]
      endif
   endif
endfor
;
; second turn: if image is missing, we remove it from list
;
liste_OK=REPLICATE(1, nb_images)
;
for ii=0, nb_images-1 do begin
   flag=QUERY_IMAGE(list_of_names[ii], info)
   if (flag EQ 0) then liste_OK[ii]=0
endfor
;
ok=WHERE(liste_OK EQ 1, nb_ok)
if nb_ok EQ 0 then begin
   MESSAGE,/continue,'No image found'
   MESSAGE,/continue,'(First time, please use keyword /Download !)'
   EXIT, status=77
endif
;
list_of_names=list_of_names[ok]
;
end
;
; -----------------------------------------------------------
;
pro SET_IMAGES_NAMES, list_of_names, links_to_images
;
pref0='http://upload.wikimedia.org/wikipedia/commons/'
pref='http://upload.wikimedia.org/wikipedia/commons/thumb/'
;
nb_images=6
;
links_to_images=STRARR(nb_images)
links_to_images[0]=pref+'9/94/Lubinus_Duchy_of_Pomerania_Map_1618_monocromatic.jpg/640px-Lubinus_Duchy_of_Pomerania_Map_1618_monocromatic.jpg'
links_to_images[1]=pref+'4/41/European_Alps.jpg/615px-European_Alps.jpg'
links_to_images[2]=pref0+'c/cf/Indexed_palette.png'
links_to_images[3]=pref+'4/44/Lavandula_spica_monochrome.png/589px-Lavandula_spica_monochrome.png'
links_to_images[4]=pref+'2/27/Kafa_gnu.png/432px-Kafa_gnu.png'
links_to_images[5]=pref+'6/64/Gnu_meditate_levitate.png/553px-Gnu_meditate_levitate.png'
;
;
fins=STRPOS(links_to_images,'/',/reverse_search)
list_of_names=STRARR(nb_images)
for ii=0, nb_images-1 do begin
   list_of_names[ii]=STRMID(links_to_images[ii],fins[ii]+1)
endfor
;
end
;
; -----------------------------------------------------------
;
pro TEST_READ_STANDARD_IMAGES, download=download, delais=delais, $
                               verbose=verbose, $
                               test=test, help=help
;
DEVICE, decomposed=0
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_READ_STANDARD_IMAGES, download=download, delais=delais, $'
   print, '                               verbose=verbose, $'
   print, '                               test=test, help=help'
   return
endif
;
; when delais > 0, we close just after
if N_ELEMENTS(delais) EQ 0 then delais=-1
;
SET_IMAGES_NAMES, list_of_names, links_to_images
;
CHECK_IMAGES, list_of_names, links_to_images, download=download
;
nb_images=N_ELEMENTS(list_of_names)
;
for ii=0, nb_images-1 do begin
   flag=QUERY_IMAGE(list_of_names[ii], info)
   if (flag EQ 1) then begin
      if KEYWORD_SET(verbose) then begin
         print, 'Processing image : ', list_of_names[ii]
         HELP,/struct, info
      endif
      ;; managing the dedicated window
      titre=info.type+' : '+list_of_names[ii]
      WINDOW, ii, title=titre, $
              xsize=info.dimensions[0], ysize=info.dimensions[1]
      ;;
      ;; reading with the rigth function
      ;;
      ;; JPEG (only two cases known today)
      ;;
      if (info.type EQ 'JPEG') then begin
         READ_JPEG, list_of_names[ii], image
         if (info.channels EQ 1) then begin
            LOADCT, 0
            TV, image
         endif
         if (info.channels EQ 3) then TV, image, /true
      endif
      ;;
      ;; PNG
      ;;
      if (info.type EQ 'PNG') then begin
         if (info.HAS_PALETTE EQ 1) then begin
            READ_PNG, list_of_names[ii], image, r, g, b
            TVLCT, r, g, b           
         endif else begin
            READ_PNG, list_of_names[ii], image
         endelse
         if (info.channels EQ 1) then TV, image
         if (info.channels EQ 2) then begin
            loadct, 0
            TV, image[0,*,*]
         endif
         if (info.channels EQ 3) then TV, image, /true
         if (info.channels EQ 4) then TV, image[0:2,*,*], /true
      endif
      if (delais GT 0) then begin
         WAIT, delais
         WDELETE, ii
      endif
   endif else begin
      MESSAGE, /Continue, 'Missing image : '+list_of_names[ii]
   endelse
endfor
;
if KEYWORD_SET(test) then STOP
;
end
