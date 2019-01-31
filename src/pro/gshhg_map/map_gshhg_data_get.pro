;
; Alain C., 2019-Jan-07, under GNU GPL v3+
;
; Get for each specified map quality level (or /All)
; the three files (we don't have USA/Canada/Mexico internal
; states borders around : if needed, please contribute)
;
; The individual files are currently store on my server for version 2.3.7
; http://aramis.obspm.fr/~coulais/GDL/GSHHG_2.3.7/
; with a file for all md5sum
;
; Alternatively, you can download all data then extract them for
; source http://www.soest.hawaii.edu/wessel/gshhg/, using the /Source keyword
; (but you we download 150 Mo + high level maps ... is it usefull ?)
;
; --------------------------------------------------------------------------
;
;
pro EXTRACT_MD5SUM, file, list_files, list_md5, test=test
;
infos=READ_ASCII_BASIC(file)
;
ok=WHERE(STRPOS(infos,'.b') GT 0, nb_infos_ok)
if nb_infos_ok GT 0 then begin
   infos_ok=infos[ok]
endif
;
list_md5=STRMID(infos_ok,0,32)
list_files=STRMID(infos_ok,34)
;
if KEYWORD_SET(test) then STOP
;
end


pro GSHHG_MD5SUM_CHECK


end
;
; --------------------------------------------------------------------------
;
pro GET_GSHHG_FROM_SOURCE, overwrite=overwrite, version=version, $
                           alternative=alternative, $
                           help=help, verbose=verbose, test=test
;
; AC 2019-Jan-22 : current version is 2.3.7
;
if KEYWORD_SET(version) then begin
   print, 'only the last (2.3.7) is ready now ...'
endif
;
; to be extended ... later !
;;if ~KEYWORD_SET(version) then begin
;; Version 2.3.7 Released June 15, 2017
md5sum='6f933dfe92878f27d1f0a36aff6f04ad'
file='gshhg-bin-2.3.7.zip'
;;endif
;
;
link='ftp://www.soest.hawaii.edu/wessel/gshhg/'
;
if KEYWORD_SET(alternative) then begin
   link='http://aramis.obspm.fr/~coulais/GDL/GSHHG_2.3.7/'
endif
;
do_download=1
;
if ~KEYWORD_SET(overwrite) then begin
   if FILE_TEST(!GSHHG_DATA_DIR+PATH_SEP()+file) then begin
      if MD5SUM_CHECK(file, md5sum) then begin
         print, 'File is valid, no need to download it again'
         do_download=0
      endif
   endif
endif
;
CD, !GSHHG_DATA_DIR, current=current
;
if do_download then GRAB_ON_INTERNET, link+file
;
if ~MD5SUM_CHECK(file, md5sum) then begin
   print, 'File may be INVALID (bad md5sum), please check'
endif
;
SPAWN, 'unzip '+file, exit_status=exit_status
if (exist_status NE 0) then MESSAGE, /continue, 'May be a problem during UNZIP of : '+file
;
CD, current
;
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------------------------------------
;
pro GET_GSHHG_BY_LEVEL, all=all, crude=crude, low=low, intermediate=intermediate, $
                        high=high, full=full, $
                        md5check=md5check, version=version, $
                        help=help, verbose=verbose, test=test
;
ON_ERROR, 2
;
if ~GSHHG_DATA_DIR_EXISTS() then begin
   MESSAGE, '!GSHHG_DATA_DIR not set. Should be set before. e.g. MAP_GSHHG_PATH_SET, /current'
endif
;
CD, !GSHHG_DATA_DIR, current=current
;
; yes, "gshhs_" here because it is Shorelines ;)
;
radix=['gshhs_','wdb_borders_','wdb_rivers_']
suffix=['c','l','i','h','f']+'.b'
nb_levels=5
;
if KEYWORD_SET(all) then begin
   status=REPLICATE(1,5)
endif else begin
   status=REPLICATE(0,5)
   if KEYWORD_SET(crude) then status[0]=1
   if KEYWORD_SET(low) then status[1]=1
   if KEYWORD_SET(intermediate) then status[2]=1
   if KEYWORD_SET(high) then status[3]=1
   if KEYWORD_SET(full) then status[4]=1
endelse
;
if TOTAL(status) EQ 0 then begin
   MESSAGE, /cont, 'at least one Map Quality level should be set !'
   return
endif
;
; the individual files we will download are store here:
link="http://aramis.obspm.fr/~coulais/GDL/GSHHG_2.3.7/"
;
files=''
for ii=0, nb_levels-1 do begin
   if status[ii] EQ 1 then files=[files,radix+suffix[ii]]
endfor
files=files[1:*]
if KEYWORD_SET(debug) then print, files
;
; downloading the files
for ii=0, N_ELEMENTS(files)-1 do begin
   if ~FILE_TEST(files[ii]) then GRAB_ON_INTERNET, link+files[ii]
endfor
;
; check whether all the files have been donwloaded
; or have been extracted
;
problems=0
for ii=0, N_ELEMENTS(files)-1 do begin
   info=FILE_INFO(files[ii])
   if ~info.exists then begin
      problems++
      print, 'Missing file : '+files[ii]
   endif else begin
      if (info.size LT 10000) then begin
         problems++
         print, 'Please check : Small size for file : '+files[ii]
      endif
   endelse
endfor
;
if (problems GT 0) then begin
   print, 'At least '+STRCOMPRESS(STRING(problems),/remove)+' files may have problems'
endif else begin
   print, 'All files downloaded in '+!GSHHG_DATA_DIR
   if KEYWORD_SET(verbose) then print, files
endelse
;
; check the files integrity : not done if général case :(
; (may be to be change in a local file "md5sum_gshhg231.txt") 
;
if KEYWORD_SET(md5check) then begin
   ;;
   print, 'Check MD5Sum : This option is not fully ready TBD (lack of md5sun files ...)'
   ;;
   file_md5='md5sum_gshhg_2.3.7.txt'
   GRAB_ON_INTERNET, link+file_md5
   EXTRACT_MD5SUM, file_md5, list_all_files, list_all_md5, test=test
   ;;
   ;; removing uneeded lines
   ;;
   okok=REPLICATE(0,N_ELEMENTS(list_all_files))
   for jj=0, N_ELEMENTS(files)-1 do begin
      ok=WHERE(STRPOS(list_all_files,files[jj]) EQ 0)
      print, ok, files[jj]
      okok[ok]=1
   endfor
   print, okok
   ok=WHERE(okok EQ 1)
   list_files=list_all_files[ok]
   list_md5=list_all_md5[ok]
   ;;
   if ~MD5SUM_CHECK(list_files, list_md5) then begin
      print, 'At least one file not OK'
   endif
   ;;   
endif
;
CD, current
;
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------------------------------------
;
pro MAP_GSHHG_DATA_GET, source=source, all=all, crude=crude, low=low, $
                        intermediate=intermediate, high=high, full=full, $
                        md5check=md5check, version=version, overwrite=overwrite, $
                        verbose=verbose, help=help, test=test, debug=debug
;
if KEYWORD_SET(help) then begin
   print, 'pro MAP_GSHHG_GET_DATA, source=source, all=all, crude=crude, low=low, $'
   print, '                        intermediate=intermediate, high=high, full=full, $'
   print, '                        md5check=md5check, version=version, overwrite=overwrite, $'
   print, '                        verbose=verbose, help=help, test=test, debug=debug'
   return
endif
;
CD, current=old_current
;
; Do we have already a valid !GSHHG_DATA_DIR ?
;
GSHHG_DATA_DIR_INIT
;
if (STRLEN(!GSHHG_DATA_DIR) EQ 0) then begin
   print, 'Undefined !GSHHG_DATA_DIR. To be set to current+tmp_gshhg_data_dir'
   tmp_gshhg_data_dir='tmp_gshhg_data_dir'
   if ~FILE_TEST(tmp_gshhg_data_dir) then FILE_MKDIR, tmp_gshhg_data_dir
endif else begin
   if ~FILE_TEST(!GSHHG_DATA_DIR,/dir) then begin
      print, 'Bad defined !GSHHG_DATA_DIR : ', !GSHHG_DATA_DIR
      return
   endif
   tmp_gshhg_data_dir=!GSHHG_DATA_DIR
endelse
;
CD, tmp_gshhg_data_dir
CD, current=full_tmp_gshhg_data_dir
;
if (STRLEN(!GSHHG_DATA_DIR) EQ 0) then begin
   !GSHHG_DATA_DIR=full_tmp_gshhg_data_dir
   print, 'Since now,  !GSHHG_DATA_DIR is set to : ', !GSHHG_DATA_DIR
endif
;
if KEYWORD_SET(source) then begin
   GET_GSHHG_FROM_SOURCE, overwrite=overwrite, version=version, alternative=alternative, $
                          test=test, verbose=verbose
endif else begin
   GET_GSHHG_BY_LEVEL, all=all, crude=crude, low=low, intermediate=intermediate, $
                       high=high, full=full, $
                       md5check=md5check, version=version, $
                       test=test, verbose=verbose
endelse
;
CD, old_current
;
if KEYWORD_SET(test) then STOP
;
end

