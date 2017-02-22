;
; AC 22 Feb 2017
;
; Testing the IDLnetURL class with very basic fonctionnalies
;
; WARNING : this code needs a internet connection
; and try to connect to a remote server with logs.
;
; Very very limited but working version :
;  improvments needed (no outputs ...)
;
pro TEST_IDLNETURL, help=help, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_IDLNETURL, help=help, test=test, no_exit=no_exit'
   return
endif
;
url='aramis.obspm.fr/~coulais/'
localFullPath='idlneturl4gdl.txt'
remoteFileName='GDL/Extra/'+localFullPath
;
; cleaning local file
FILE_DELETE, localFullPath
;
oUrl = OBJ_NEW('IDLnetUrl')
oUrl->SetProperty, URL_SCHEME = 'http'
oUrl->SetProperty, URL_HOSTNAME = url
oUrl->SetProperty, URL_PATH = remoteFileName
oUrl->SetProperty, URL_USERNAME = ''
oUrl->SetProperty, URL_PASSWORD = ''
retrievedFilePath = oUrl->Get(FILENAME=localFullPath)
oUrl->CloseConnections
OBJ_DESTROY, oUrl
;
; reading back the file
;
tmp=''
OPENR, lun, localFullPath, /get_lun
READF, lun, tmp
CLOSE, lun
FREE_lun, lun
;
expected_content='AC 22 Feb 2017'
if (expected_content EQ tmp) then error=0 else error=1
;
BANNER_FOR_TESTSUITE, 'TEST_IDLNETURL', error
;
if (error GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
