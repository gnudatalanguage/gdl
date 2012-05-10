;
; AC, 5-Feb-2012
;
; Revisited on May 2012, after I realized that ImageMagick is not
; reliable (different results with various lib. versions OR different
; OS).
;
; GOAL : the goal of this program is to quickly check
; that reading and displaying known images (JPEG and PNG) is OK
; We used images available on the WEB, on Wikipedia;
; if not on Hard Drive, automatic download available.
;
; Consistancy checks (md5 sum and expected values) hardcoded
;
; The test cases in this code can easily be extended.
;
; HISTORY: based on exchanges on 
; https://sourceforge.net/projects/gnudatalanguage/forums/forum/338692/topic/4992595/index/page/1
;
; -----------------------------------------------------------
;
pro PRINT_INFO_IMAGE_STRUCTURE, images, noheader=noheader
;
nb_images=N_ELEMENTS(images)
if nb_images EQ 0 then begin
   MESSAGE,/continue, 'No Image provided !'
   return
endif
;
myformat1="(A40,a3,a6,a3,a6,a3,a12,a3,a6,a3,a6,a3,a5,a3)"
myformat2="(A40,a3,a6,a3,a6,a3,a12,a3,i6,a3,i6,a3,i5,a3)"
sep=' | '
;
if ~KEYWORD_SET(no_header) then begin
   print, format=myformat1, 'name', sep, 'format', sep, $
          'class', sep, 'type', sep, 'matte', sep,$
          'pal.', sep, 'chan.', sep
endif
;
for ii=0, nb_images-1 do begin
   print, format=myformat2, images[ii].name, sep, images[ii].format, sep, $
          images[ii].class, sep,$
          images[ii].type, sep, images[ii].matte, sep, $
          images[ii].has_palette, sep, images[ii].channels, sep
endfor
;
end
;
; -----------------------------------------------------------
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
pro CHECK_MD5_OF_IMAGES, images, test=test, verbose=verbose
;
list_of_names=images.name
list_of_md5=images.md5
;
md5cmd=''
if STRLOWCASE(!version.os_name) eq 'linux' then md5cmd='md5sum '
if STRLOWCASE(!version.os_name) eq 'darwin' then  md5cmd='md5 -r '
;
if (STRLEN(md5cmd) EQ 0) then begin
   MESSAGE, /cont, 'MD5 command not known for your system: '+!version.os_name
   MESSAGE, /cont, 'please contribute, no test done.'
   return
endif
;
md5lenght=32

for ii=0, N_ELEMENTS(list_of_names)-1 do begin
   SPAWN, md5cmd+list_of_names[ii], result, error
   subresult=STRMID(result,0, 32)
   if (subresult NE list_of_md5[ii]) then begin
      MESSAGE,/cont, 'MD5 checksum error for image : '+list_of_names[ii]
   endif else begin
      if KEYWORD_SET(verbose) then $
         print, 'MD5 checksum OK for image : '+list_of_names[ii]
   endelse
endfor
;
if KEYWORD_SET(test) then STOP
;
end
; -----------------------------------------------------------
;
pro GET_IMAGES, images, download=download, $
                help=help, test=test, verbose=verbose
;
list_of_names=images.name
links_to_images=images.link
;
if KEYWORD_SET(download) then script=TEST_DOWNLOAD_TOOLS()
;
nb_images=N_ELEMENTS(list_of_names)
;
; first turn: if image is missing and we ask for download, we try to download
;
for ii=0, nb_images-1 do begin
   flag=QUERY_IMAGE(list_of_names[ii])
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
   flag=QUERY_IMAGE(list_of_names[ii])
   if (flag EQ 0) then liste_OK[ii]=0
endfor
;
ok=WHERE(liste_OK EQ 1, nb_ok)
if (nb_ok EQ 0) then begin
   MESSAGE, /continue, 'No image found'
   MESSAGE, /continue, '(First time, please use keyword /Download !)'
   EXIT, status=77
endif
if (nb_ok LT nb_images) then begin
   MESSAGE, /continue, 'Expected '+STRING(nb_images)+' images'
   MESSAGE, /continue, 'Only '+STRING(nb_ok)+' images really available'
endif
;
; we remove the missing images
;
images=images[OK]
;
if KEYWORD_SET(verbose) then begin
   print, STRCOMPRESS(STRING(N_ELEMENTS(images))), ' images are really present.'
   PRINT_INFO_IMAGE_STRUCTURE, images
endif
;
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------------------
;
; We compare IDL, GDL results through QUERY_IMAGE with expect known
; results. 
;
pro CHECK_IMAGES_CONSISTANCY, images, $
                              test=test, help=help, verbose=verbose
;
nb_images=N_ELEMENTS(images)
;
DEFSYSV, '!gdl', exists=is_it_gdl
;
; we are not ready for all GDL tests ...
is_it_gdl=0
;
GlobalPB=0
;
for ii=0, nb_images-1 do begin
   ;; "encountered problems" increments ...
   pb=0
   ;;
   if KEYWORD_SET(verbose) then begin
      print, '---------------------'
      print, '* Checking image : ', images[ii].name
   endif
   ;; when we call GDL version, we have extra hidden info to check
   ;; whether GraphicMagick OR ImageMagick work as expected
   ;;
   if (is_it_gdl EQ 1) then begin
      flag=QUERY_IMAGE(images[ii].name, info, gdl_extra_info)
   endif else begin
      flag=QUERY_IMAGE(images[ii].name, info)
   endelse
   ;; do we have a file ?
   if (flag EQ 1) then begin
      if KEYWORD_SET(verbose) then begin
         print, '* Processing image : ', images[ii].name
         ;HELP,/struct, info
      endif
      if images[ii].format NE info.type then begin
         if KEYWORD_SET(verbose) then print, '  --> pb within Type/Format'
         pb=pb+1
      endif
            if images[ii].has_palette NE info.has_palette then begin
         if KEYWORD_SET(verbose) then print, '  --> pb within Has_Palette'
         pb=pb+1
      endif
      if images[ii].channels NE info.channels then begin
         if KEYWORD_SET(verbose) then print, '  --> pb within Channels number'
         pb=pb+1
      endif
      if (is_it_gdl EQ 1) then begin
         print, 'this is not ready'
      endif
      if (pb EQ 0) then print, '  --> OK !'
   endif else begin
      MESSAGE, /continue, 'Missing file : '+images[ii].name
   endelse
   ;;
   if pb GT 0 then GlobalPB=GlobalPB+1
endfor
;
if (GlobalPB GT 0) then begin
   MESSAGE, /continue, 'Number of Images giving problems : '+STRING(GlobalPB)   
endif
;
if KEYWORD_SET(test) then STOP
;
end
; -----------------------------------------------------------
;
; a definition list of "type" can be found here:
; http://www.graphicsmagick.org/Magick++/Image.html#type
;
pro SET_IMAGES_NAMES, images, test=test, help=help, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro SET_IMAGES_NAMES, images, test=test, help=help, verbose=verbose'
endif
;
pref0='http://upload.wikimedia.org/wikipedia/commons/'
pref='http://upload.wikimedia.org/wikipedia/commons/thumb/'
;
; IDL 6.0 and 7.1 gave same results on those test images
;
nb_images=6
;
; Proporties for a given image. 
; When Matte ==1, we have transparency (1+1, 3+1 ...)
image={name: '', link:'', md5:'', format:'', $
       class: 'No', type:'No', matte: 0, has_palette:0, channels:0}
images=REPLICATE(image,nb_images)
;
ii=0
images[ii].link=pref+'9/94/Lubinus_Duchy_of_Pomerania_Map_1618_monocromatic.jpg/640px-Lubinus_Duchy_of_Pomerania_Map_1618_monocromatic.jpg'
images[ii].md5='fe5184f2149460470752e942321dcc8c'
images[ii].format='JPEG'
images[ii].class='PseudoClass'
images[ii].type='GrayScale'
images[ii].matte=0
images[ii].has_palette=0
images[ii].channels=1
;
ii=ii+1
images[ii].link=pref+'4/41/European_Alps.jpg/615px-European_Alps.jpg'
images[ii].md5='37288476231f29b7d54412dce4922999'
images[ii].format='JPEG'
images[ii].class='DirectClass'
images[ii].type='TrueColor'
images[ii].matte=0
images[ii].has_palette=0
images[ii].channels=3
;
ii=ii+1
images[ii].link=pref0+'c/cf/Indexed_palette.png'
images[ii].md5='769fde91867799febf633942c1a07960'
images[ii].format='PNG'
images[ii].class='PseudoClass'
images[ii].type='Palette'
images[ii].matte=0
images[ii].has_palette=1
images[ii].channels=1
;
ii=ii+1
images[ii].link=pref+'4/44/Lavandula_spica_monochrome.png/589px-Lavandula_spica_monochrome.png'
images[ii].md5='2044b4ee7a0f34083cf1bd5a416e6ae5'
images[ii].format='PNG'
images[ii].class='PseudoClass'
; one special case : see as 'grayscale' by Graphics Magick, but
; matte==1 and 2 channels ...
images[ii].type='GrayScaleMatte'
images[ii].matte=1
images[ii].has_palette=0
images[ii].channels=2
;
ii=ii+1
images[ii].link=pref+'2/27/Kafa_gnu.png/432px-Kafa_gnu.png'
images[ii].md5='9865555b28a3013a378edbdd6f03b4ef'
images[ii].format='PNG'
images[ii].class='DirectClass'
images[ii].type='TrueColor'
images[ii].matte=0
images[ii].has_palette=0
images[ii].channels=3
;
ii=ii+1
images[ii].link=pref+'6/64/Gnu_meditate_levitate.png/553px-Gnu_meditate_levitate.png'
images[ii].md5='496bfa9eb810bd76124d59cfb1e8abe6'
images[ii].format='PNG'
images[ii].class='DirectClass'
images[ii].type='TrueColorMatte'
images[ii].matte=1
images[ii].has_palette=0
images[ii].channels=4
;
fins=STRPOS(images.link,'/',/reverse_search)
list_of_names=STRARR(nb_images)
for ii=0, nb_images-1 do begin
   list_of_names[ii]=STRMID(images[ii].link,fins[ii]+1)
endfor
images.name=list_of_names
;
if KEYWORD_SET(verbose) then begin
   print, STRCOMPRESS(STRING(nb_images)), ' images have been initialized:'
   PRINT_INFO_IMAGE_STRUCTURE, images
endif
;
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------------------
;
pro TEST_READ_STANDARD_IMAGES, download=download, delais=delais, $
                               verbose=verbose, $
                               test=test, help=help, debug=debug
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
if N_ELEMENTS(delais) EQ 0 then delais=1
;
; when download > 0, we will try to download missing images
if N_ELEMENTS(download) EQ 0 then download=1
;
; set a (extensible) list of images (name, external link, properties)
SET_IMAGES_NAMES, images, verbose=verbose
;
; verify is images are around or trying to download.
; Number of images may change here
GET_IMAGES, images, download=download, verbose=verbose
;
; here we check we are processing the images we expect to process !!
CHECK_MD5_OF_IMAGES, images, verbose=verbose
;
; Here we check that the know properties of images are in agreement
; with what is extracted from QUERY_IMAGE(image_name, image_info)
CHECK_IMAGES_CONSISTANCY, images, verbose=verbose
;
; after that, we will try to display the images ...
;
nb_images=N_ELEMENTS(images)
list_of_names=images.name
;
if KEYWORD_SET(debug) then STOP
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
