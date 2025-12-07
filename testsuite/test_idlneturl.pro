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
; ---------------------------------------
; Modifications history :
;
; - 2025-Dec-06 : AC. Add a super basic test ...
;  Use the external variable IDLNETURL_VERBOSE=1
;
; ---------------------------------------
;
pro TEST_IDLNETURL_BASIC, cumul_errors, test=test, verbose=verbose
;
; AC 2025 : to activate internal verbosity in IDLnetURL
SETENV, 'IDLNETURL_VERBOSE='
if KEYWORD_SET(verbose) then SETENV, 'IDLNETURL_VERBOSE=1'
;
errors=0
;
remoteFileName='~coulais/GDL/Extra/idlneturl4gdl.txt'
url='lerma.obspm.fr'
;
; Clean-up
;
FILE_DELETE, ['idlneturl4gdl.txt','idl.dat'], /allow_no
;
;
oUrl = OBJ_NEW('IDLnetUrl')
oUrl->SetProperty, URL_PATH = remoteFileName
oUrl->SetProperty, URL_HOSTNAME = url
oUrl->SetProperty, URL_SCHEME = 'https'
;
; check filename
;
retrievedFilePath = oUrl->Get()
cd, cur=cur
if (cur+PATH_SEP()+'idl.dat' NE retrievedFilePath) then $
   ERRORS_ADD, errors, 'bad path & filename !'
if  KEYWORD_SET(verbose) then print, 'Path & File : ', retrievedFilePath
;
; check content
;
retrieved_content = oUrl->Get(/string_array)
if (retrieved_content NE 'AC 22 Feb 2017') then $
   ERRORS_ADD, errors, 'bad content !'
if  KEYWORD_SET(verbose) then print, 'File content : ', retrieved_content
;
; switch OFF verbosity
if KEYWORD_SET(verbose) then SETENV, 'IDLNETURL_VERBOSE='
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_IDLNETURL_BASIC", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------
pro INTERNAL_SEP, indice, debut=debut, fin=fin
separateur='=-=-=-=-=-=-=-'
if KEYWORD_SET(debut) then txt=separateur+' begin '+STRING(indice)
if KEYWORD_SET(fin) then txt=separateur+' end '+STRING(indice)
print, STRCOMPRESS(txt+' '+separateur)
end
; ---------------------------------------
;
pro TEST_IDLNETURL_FULL, cumul_errors, test=test, no_exit=no_exit, verbose=verbose
;
; AC 2025 : to activate internal verbosity in IDLnetURL
if KEYWORD_SET(verbose) then SETENV, 'IDLNETURL_VERBOSE=1'
separateur='=-=-=-=-=-=-=-'
;
errors=0
;
url='lerma.obspm.fr'
FileName='idlneturl4gdl.txt'
remoteFileName='~coulais/GDL/Extra/'+FileName
;
expected_content='AC 22 Feb 2017'
;
localPaths=['','','SubDirNetUrl/','../testsuite/SubDirNetUrl/']
;
for ii=0, N_ELEMENTS(localPaths)-1 do begin
   if KEYWORD_SET(verbose) then INTERNAL_SEP, ii, /debut
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
   oUrl->SetProperty, URL_SCHEME = 'https'
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
         ERRORS_ADD, errors, 'Bad content with /string_array'
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
      if (expected_content NE tmp) then ERRORS_ADD, errors, 'Bad content in file'
      ;;
      ;; final cleaning
      ;;
   endif else begin
      ERRORS_ADD, errors, 'Missing file : '+retrievedFilePath
   endelse
   if KEYWORD_SET(verbose) then INTERNAL_SEP, ii, /fin
endfor
;
; switch OFF verbosity
if KEYWORD_SET(verbose) then SETENV, 'IDLNETURL_VERBOSE='
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_IDLNETURL_FULL", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------
;
pro TEST_IDLNETURL, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_IDLNETURL, help=help, test=test, no_exit=no_exit'
   return
endif
;
; switch ON/OFF verbosity
SETENV, 'IDLNETURL_VERBOSE='
if KEYWORD_SET(verbose) then SETENV, 'IDLNETURL_VERBOSE=1'
;
cumul_errors=0

TEST_IDLNETURL_BASIC, cumul_errors, test=test, verbose=verbose
TEST_IDLNETURL_FULL, cumul_errors, test=test, verbose=verbose

; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_IDLNETURL', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
