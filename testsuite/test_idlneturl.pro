;
; AC 22 Feb 2017
;
; Testing the IDLnetURL class with very basic fonctionnalies
;
; WARNING : this code needs a internet connection
; and try to connect to a remote server with logs.
;
; Very very limited but working version :
; improvments needed (no outputs ...)
;
pro TEST_IDLNETURL, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_IDLNETURL, help=help, test=test, no_exit=no_exit'
   return
endif
;
error=0
;
url='aramis.obspm.fr/~coulais/'
FileName='idlneturl4gdl.txt'
remoteFileName='GDL/Extra/'+FileName
;
expected_content='AC 22 Feb 2017'
;
localPaths=['','','SubDirNetUrl/','../testsuite/SubDirNetUrl/']
;
for ii=0, N_ELEMENTS(localPaths)-1 do begin
   localFullPath=localPaths[ii]+FileName
   ;;
   if ii GT 0 then begin
      localFullPath=localFullPath+'_'+GDL_IDL_FL()+'_'+STRING(ii)
      localFullPath=STRCOMPRESS(localFullPath,/remove_all)
   endif
   ;; cleaning local file
   if FILE_TEST(localFullPath) then begin
      print, 'Removing pre-existing file : '+localFullPath
      FILE_DELETE, localFullPath
   endif
   if STRLEN(localPaths[ii]) GT 0 then begin
      if ~FILE_TEST(localPaths[ii],/dir) then begin
         print, 'Path not found : ', localPaths[ii]
         FILE_MKDIR, localPaths[ii]
         if ~FILE_TEST(localPaths[ii],/dir) then break
         print, 'Path created : ', localPaths[ii]
      endif
   endif
   ;;
   oUrl = OBJ_NEW('IDLnetUrl')
   oUrl->SetProperty, URL_SCHEME = 'http'
   oUrl->SetProperty, URL_HOSTNAME = url
   oUrl->SetProperty, URL_PATH = remoteFileName
   oUrl->SetProperty, URL_USERNAME = ''
   oUrl->SetProperty, URL_PASSWORD = ''
   if (ii GT 0) then begin
      retrievedFilePath = oUrl->Get(FILENAME=localFullPath)
   endif else begin
      retrievedFilePath = oUrl->Get() ;; only the basic one
      ;;
      ;; AC 2018-sep-20 : something is not clear here :((
      ;; IDL return undocumented "idl.dat" if we use:
      ;; string=content
      retrieved_content = oUrl->Get(/string_array)
      if (expected_content NE retrieved_content) then $
         ERRORS_ADD, error, 'Bad content with /string_array'
   endelse
   oUrl->CloseConnections
   OBJ_DESTROY, oUrl
   ;;
   ;; is the file really arrived ?!
   ;;
   if KEYWORD_SET(verbose) then begin
      print, "step      : ", ii
      print, "Input file name     :", localFullPath
      print, "Full retrieved file :", retrievedFilePath
   endif
   ;;
   if FILE_TEST(retrievedFilePath) then begin
      ;;
      ;; reading back the file
      ;;
      tmp=''
      OPENR, lun, retrievedFilePath, /get_lun
      READF, lun, tmp
      CLOSE, lun
      FREE_lun, lun
      ;;
      ;; is the content as expected ?
      if (expected_content NE tmp) then ERRORS_ADD, error, 'Bad content in file'
      ;;
      ;; final cleaning
      ;;
   endif else begin
      ERRORS_ADD, error, 'Missing file : '+retrievedFilePath
   endelse
endfor
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_IDLNETURL', error
;
if (error GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
