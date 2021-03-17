;
; Code under GNU GPL v2 or later
;
; Initial version by Chris Lee.
;
; Revised by Alain Coulais, 30 August 2011,
; in order to work on the image provided in testsuite/: "Saturn.jpg"
;
pro IMAGE_TEST, filename=filename, path=path, no_cleaning=no_cleaning, $
                help=help, test=test, debug=debug, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro IMAGE_TEST, filename=filename, path=path, no_cleaning=no_cleaning, $'
   print, '                help=help, test=test, debug=debug, verbose=verbose'
   return
endif
;
if N_ELEMENTS(path) EQ 0 then path=!path
if N_ELEMENTS(filename) EQ 0 then filename='Saturn.jpg'
;
one_file_and_path=FILE_SEARCH_FOR_TESTSUITE(filename, /quiet)
;
if (STRLEN(one_file_and_path) EQ 0) then begin
    MESSAGE, /continue, 'No file founded ...'
    MESSAGE, /continue, 'File : '+filename
    HELP, /PATH
    return
endif
;
if KEYWORD_SET(verbose) then begin
   MESSAGE, /continue, 'reading : '+one_file_and_path
endif
;
;  for IDL compatibility
FORWARD_FUNCTION MAGICK_EXISTS
;
DEFSYSV, '!gdl', exists=is_it_gdl
if (is_it_gdl EQ 1) then begin
    ;; Do we have access to ImageMagick functionnalities ??
    ;;
    if (MAGICK_EXISTS() EQ 0) then begin
        MESSAGE, /continue, "GDL was compiled without ImageMagick support."
        MESSAGE, /con, "You must have ImageMagick support to use this functionaly."
        EXIT, status=77
    endif
endif
;
; Copy of the file to have a simple way to remove all derivatives
; at the end
;
mypath=FILE_DIRNAME(one_file_and_path,/mark_dir)
myname=FILE_BASENAME(one_file_and_path) 
;
DEFSYSV, '!gdl', exists=isGDL
if isGDL then prefix='GDL_' else prefix='IDL_'
prefixe=prefix+'image_test_'
;
old_one_file_and_path=one_file_and_path
one_file_and_path=mypath+prefixe+myname
FILE_COPY, old_one_file_and_path, one_file_and_path, /overwrite
;
if KEYWORD_SET(debug) then STOP
;
upname=STRUPCASE(one_file_and_path)
pos1=STRPOS(upname,'.JPG', /reverse_search)
pos2=STRPOS(upname,'.JPEG', /reverse_search)
if ((pos1 LT 0) AND (pos2 LT 0)) then begin
   MESSAGE, /continue, 'Are you sure to provide a JPEG file as input ?'
endif
if (pos1 GT 0) then no_suffixe=STRMID(one_file_and_path, 0, pos1)
if (pos2 GT 0) then no_suffixe=STRMID(one_file_and_path, 0, pos2)
;
if KEYWORD_SET(debug) then STOP
;
; the JPEG case
; (no conversion needed, we read the image provided)
;
suffixe='.jpg'
READ_JPEG, one_file_and_path, image
MESSAGE, /continue, "internal READING of JPEG TrueColor DONE"
WRITE_JPEG, no_suffixe+'_jpeg.jpeg', image, /TRUE
MESSAGE, /continue, "internal WRITING of JPEG TrueColor DONE"
;
; the following tests need "convert" utility
; On some Linux system (eg Ubuntu) one lib. migth be missing for IDL
; Solution on Ubuntu : sudo apt-get install libjpeg62
; Tested by Alain C., March 2016 on Ubuntu 14.04
;
SPAWN, 'convert -version', resu, error, exit_status=exit_status
;
if (exit_status NE 0) then begin
    MESSAGE, /cont, 'A problem arrises, related to "convert" utility'
    MESSAGE, /cont, 'please check is "convert" and lib. are well install'
    MESSAGE, /cont, 'The error message is :'
    print, format='(%"\N",A,%"\N")', error
    MESSAGE, /cont, 'If error message is related to "libjpeg.so.62"'
    MESSAGE, /cont, 'You have to add it on Ubuntu : sudo apt-get install libjpeg62'
    EXIT, status=77
endif
;
; the BMP case
;
suffixe='.bmp'
SPAWN, 'convert '+one_file_and_path+' '+no_suffixe+suffixe, resu, error
if STRLEN(error) EQ 0 then begin
   MESSAGE, /continue, "external converting BMP from JPG DONE"
   if (QUERY_BMP(no_suffixe+suffixe) EQ 1) then begin
      MESSAGE, /continue, "internal SUCCESSFULL TESTING of BMP format DONE"
   endif else begin
      MESSAGE, /continue, "internal PROBLEM when TESTING of BMP format"
   endelse
   a=READ_BMP(no_suffixe+suffixe)
   MESSAGE, /continue, "internal READING of BMP TrueColor DONE"
   WRITE_BMP, no_suffixe+'_bmp.bmp',a
   MESSAGE, /continue, "internal WRITING of BMP TrueColor DONE"
endif
;
suffixe='.tiff'
SPAWN, 'convert '+one_file_and_path+' '+no_suffixe+suffixe, resu, error
if STRLEN(error) EQ 0 then begin
   MESSAGE, /continue, "external converting TIFF from JPG DONE"
   if (QUERY_TIFF(no_suffixe+suffixe) EQ 1) then begin
      MESSAGE, /continue, "internal SUCCESSFULL TESTING of TIFF format DONE"
   endif else begin
      MESSAGE, /continue, "internal PROBLEM when TESTING of TIFF format"
   endelse
   a=READ_TIFF(no_suffixe+suffixe)
   MESSAGE, /continue, "internal READING of TIFF TrueColor DONE"
   WRITE_BMP, no_suffixe+'_tiff.bmp', a
   MESSAGE, /continue, "internal WRITING of BMP from TIFF TrueColor DONE"
endif
;
suffixe='.png'
SPAWN, 'convert '+one_file_and_path+' '+no_suffixe+suffixe, resu, error
if STRLEN(error) EQ 0 then begin
   MESSAGE, /continue, "external converting PNG from JPG DONE"
   if (QUERY_PNG(no_suffixe+suffixe) EQ 1) then begin
      MESSAGE, /continue, "internal SUCCESSFULL TESTING of PNG format DONE"
   endif else begin
      MESSAGE, /continue, "internal PROBLEM when TESTING of PNG format"
   endelse
   a=READ_PNG(no_suffixe+suffixe)
   MESSAGE, /continue, "internal READING of PNG TrueColor DONE"
   WRITE_PNG, no_suffixe+'_png.png', a
   MESSAGE, /continue, "internal WRITING of PNG TrueColor DONE"
endif
;
suffixe='.pict'
SPAWN, 'convert '+one_file_and_path+' '+no_suffixe+suffixe, resu, error
if STRLEN(error) EQ 0 then begin
   MESSAGE, /continue, "external converting from JPG to PICT DONE"
   if (QUERY_PICT(no_suffixe+suffixe) EQ 1) then begin
      MESSAGE, /continue, "internal SUCCESSFULL TESTING of PICT format DONE"
   endif else begin
      MESSAGE, /continue, "internal PROBLEM when TESTING of PICT format"
   endelse
   READ_PICT, no_suffixe+suffixe, a
   MESSAGE, /continue, "internal READING of PICT TrueColor DONE"
   WRITE_PICT, no_suffixe+'_png.png', a
   MESSAGE, /continue, "internal WRITING of PICT TrueColor DONE"
endif
;
; maybe not doing a cleaning is better to check created Images ?
;
if ~KEYWORD_SET(no_cleaning) then begin
    liste_to_delete=FILE_SEARCH(mypath+prefixe+'*')
    FILE_DELETE, liste_to_delete
endif
;
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------
;

pro OLD_IMAGE_TEST
;
tdir="/home/chris/ei/"
;
print,"Read BMP TrueColor"
a=read_bmp(tdir+"tc24.bmp")
print,"Write BMP"
write_bmp,tdir+"tc24_bmp.bmp",a
;
print,"Read TIFF TrueColor"
a=read_tiff(tdir+"tc24.tiff")
print,"Write BMP"
write_bmp,tdir+"tc24_tiff.bmp",a

print,"Read PNG TrueColor"
a=read_png(tdir+"tc24.png")
print,"Write PNG"
write_png,tdir+"tc24_png.png",a

print,"Read JPG TrueColor"
read_jpeg,tdir+"tc24.jpg",a
print,"Write JPEG"
write_jpeg,tdir+"tc24_jpg.jpg",a

print,"Read PICT TrueColor"
read_jpeg,tdir+"tc24.pict",a
print,"Write PICT"
write_jpeg,tdir+"tc24_pict.pict",a


print,"Read BMP Indexed"
a=read_bmp(tdir+"i256.bmp",r,g,b)
print,"Write BMP"
write_bmp,tdir+"i256_bmp.bmp",a,r,g,b

print,"Read TIFF Indexed"
a=read_tiff(tdir+"i256.tiff",r,g,b)
print,"Write BMP"
write_bmp,tdir+"i256_tiff.bmp",a,r,g,b

print,"Read PNG Indexed"
a=read_png(tdir+"i256.png",r,g,b)
print,"Write PNG"
write_png,tdir+"i256_png.png",a,r,g,b

print,"Read PICT Indexed"
read_jpeg,tdir+"i256.pict",a
print,"Write PICT"
write_jpeg,tdir+"i256_pict.pict",a


a=read_bmp(tdir+"i256_bmp.bmp",r,g,b)
write_bmp,tdir+"i256_bmp100.bmp",a,r,g,b
for i=0, 100 do begin
   print, "Quantizing: ",i
   a=read_bmp(tdir+"i256_bmp100.bmp",r,g,b)
   write_bmp,tdir+"i256_bmp100.bmp",a,r,g,b
endfor
end
