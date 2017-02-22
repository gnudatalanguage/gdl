;
; Under GNU GPL V3
; Alain C., 22 February 2017
;
; Very preliminary version of IDLnetURL,
; just what is needed by some examples in Coyote lib.
; (which is provided by Debian ...)
; Goal is to be able to run ATV 3.0b8 (with last AstroLib)
;
; Please post improvments on GDL discussion forum or on 
; https://sourceforge.net/p/gnudatalanguage/patches/
;
; Very very limited but working version :
; improvments needed (no outputs ... wget or curl ...)
;
; Usually, Curl is available by default on OSX, 
; but not on most GNU/Linux ... we will try to use Wget too ...
;
pro idlneturl::CloseConnections
end
function idlneturl::Get, filename=filename
;
tmpIDLDir = GETENV('IDL_TMPDIR')
if STRLEN(tmpIDLDir) EQ 0 then tmpIDLDir='/tmp/'
;
id_cmd=''
;
if STRLEN(self.URL_USERNAME) GT 0 then id_cmd=' -u '+self.URL_USERNAME
if STRLEN(self.URL_PASSWORD) GT 0 then id_cmd=id_cmd+':'+self.URL_PASSWORD+' '
;
cmd='curl -O '+id_cmd+self.URL_SCHEME+'://'+self.URL_HOSTNAME
cmd=cmd+self.URL_PATH
;
print, cmd
;
spawn, cmd
;
return, 'toto'
;
end
;
; --------------------------
;
pro idlneturl::SetProperty, URL_SCHEME = URL_SCHEME, $
                            URL_HOSTNAME = URL_HOSTNAME, $
                            URL_PATH = URL_PATH, $
                            URL_PORT = URL_PORT, $
                            URL_USERNAME =  URL_USERNAME, $
                            URL_PASSWORD =  URL_PASSWORD
;
IF KEYWORD_SET(URL_SCHEME) then self.URL_SCHEME=URL_SCHEME
IF KEYWORD_SET(URL_HOSTNAME) then self.URL_HOSTNAME=URL_HOSTNAME
IF KEYWORD_SET(URL_PATH) then self.URL_PATH=URL_PATH
IF KEYWORD_SET(URL_PORT) then self.URL_PORT=URL_PORT
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

