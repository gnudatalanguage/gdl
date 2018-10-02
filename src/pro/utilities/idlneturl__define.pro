;
; Under GNU GPL V3
; Alain C., 22 February 2017
;
; Very preliminary version of IDLnetURL,
; just what is needed by some examples in Coyote lib.
; (which is provided by Debian ...)
; Goal is to be able to run ATV 3.0b8 (with last AstroLib)
;
; Please post improvments or comments on:
; https://github.com/gnudatalanguage/gdl/issues
;
; Very very limited but working version :
; improvments needed (no outputs ... wget or curl ...)
;
; Usually, Curl is available by default on OSX, 
; but not on most GNU/Linux ... we will try to use Wget too ...
;
; ----------------------------------------------------
; Modifications history :
;
; 2018-Sep-10 : AC.
; -- add self.URL_QUERY
; -- add "-L" for Curl
; -- better management of keyword filename= & string_array=
; -- tested with current (in 2018) QuerySimbad,'GAL045.45+00.06' in astrolib
;
; ----------------------------------------------------
;
;
pro idlneturl::CloseConnections
end
function idlneturl::Get, filename=filename, string_array=string_array, $
                         test=test, verbose=verbose
;
;; AC 2018-spet-27 : no more useful ?!
;; tmpIDLDir = GETENV('IDL_TMPDIR')
;; if STRLEN(tmpIDLDir) EQ 0 then tmpIDLDir='/tmp/'
;
id_cmd=''
;
if STRLEN(self.URL_USERNAME) GT 0 then id_cmd=' -u '+self.URL_USERNAME
if STRLEN(self.URL_PASSWORD) GT 0 then id_cmd=id_cmd+':'+self.URL_PASSWORD+' '
id_cmd=id_cmd+self.URL_SCHEME+'://'+self.URL_HOSTNAME
if STRLEN(self.URL_PORT) GT 0 then id_cmd=id_cmd+':'+self.URL_PORT
id_cmd=id_cmd+self.URL_PATH
if STRLEN(self.URL_QUERY) GT 0 then id_cmd=id_cmd+'?'+self.URL_QUERY
;
; AC, 2018-sep-20 : option -L in curl
; in vizier, the /viz-bin was moved into a /cgi-bin ...
;
curl_cmd='curl -L '
;
if ~KEYWORD_SET(string_array) then begin
   if (STRLEN(self.URL_QUERY) GT 0) then begin
      curl_cmd=curl_cmd+'-O '
   endif else begin
      if ~KEYWORD_SET(filename) then filename='idl.dat'
      curl_cmd=curl_cmd+'-o '+filename+' '
   endelse
endif
;
cmd=curl_cmd+id_cmd
if KEYWORD_SET(verbose) then print, cmd
;
SPAWN, cmd, result, blahblah
if KEYWORD_SET(verbose) then begin
   print, 'Result : ', result
   print, 'blahblah : ', blahblah
endif
;
if KEYWORD_SET(test) then STOP
;
; order of Keywords is : STRING, BUFFER, FILENAME
if KEYWORD_SET(string_array) then return, result
if KEYWORD_SET(buffer) then print, 'what is a BUFFER ?!'
if KEYWORD_SET(filename) then return, FILE_SEARCH(filename, /full)
;
; AC 2018-sep-20 : somethings not clear here because I succeed
; to got "idl.dat" !
;
; return an empty string if none of the previous 3 keywords !
return, ''
;
end
;
; --------------------------
;
pro idlneturl::SetProperty, URL_SCHEME = URL_SCHEME, $
                            URL_HOSTNAME = URL_HOSTNAME, $
                            URL_PATH = URL_PATH, $
                            URL_PORT = URL_PORT, $
                            URL_QUERY= URL_QUERY, $
                            URL_USERNAME =  URL_USERNAME, $
                            URL_PASSWORD =  URL_PASSWORD
;
IF KEYWORD_SET(URL_SCHEME) then self.URL_SCHEME=URL_SCHEME
IF KEYWORD_SET(URL_HOSTNAME) then self.URL_HOSTNAME=URL_HOSTNAME
IF KEYWORD_SET(URL_PATH) then self.URL_PATH=URL_PATH
IF KEYWORD_SET(URL_PORT) then self.URL_PORT=URL_PORT
IF KEYWORD_SET(URL_QUERY) then self.URL_QUERY=URL_QUERY
IF KEYWORD_SET(URL_USERNAME) then self.URL_USERNAME=URL_USERNAME
IF KEYWORD_SET(URL_PASSWORD) then self.URL_PASSWORD=URL_PASSWORD
;
end
;
; --------------------------
;
function idlneturl::Init, filename, VERBOSE=verbose
;
return, 1
;
end

pro idlneturl__define, struct
struct = {idlneturl, $
          IDLNETURL_TOP : 0l, $
          CURLPTR       : 0L, $
          URL_SCHEME    : 'http' , $
          URL_HOSTNAME  : '',$
          URL_PATH      : '',$
          URL_PORT      : '80',$
          URL_QUERY     : '',$
          URL_USERNAME  : '',$
          URL_PASSWORD  : '',$
          IDLNETURL_BOTTOM: 0L}
end

