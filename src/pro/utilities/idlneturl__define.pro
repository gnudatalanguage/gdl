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
; 2020-Jan-10 : GD  support most options for Get() etc.
; 2025-Feb-20 : GD  Added PUT
; ----------------------------------------------------
;
;
function get_percent
  openr,lun,"/tmp/pb",/get
  a=''
  readf,lun,a,format='(A)'
  percent=0.01*strmid(a,5,5,/rever)
  free_lun,lun
  return, percent
end

function idlneturl::format_response_header,stringarray,response_header_size
    linefeed=string(10b) ; HTTP/1.1 defines the sequence CR LF as the end-of-line marker for all protocol elements except the entity-body
; we have the header size in bytes.
;find the number of lines in the stringarray that give
;response_header_size bytes. A zero-length response header does not
;necessary mean no-response, as FTP transactions do not give it.
  i=0 &  n=0 & self.response_header=''
  while i lt response_header_size do begin
     i+=(n_bytes(stringarray[n])+1) ;  since the newline is absent in result array
     self.response_header+=stringarray[n]
     if (i lt response_header_size) then self.response_header+=linefeed ;header is the concatenation of the n first lines
     n++                                                                ; next line
  end
  if n_elements(stringarray) gt 4+n then response=stringarray[n:-5] else response=''
  return, response
end

pro idlneturl::copy_header_to_response_header,header_filename
;;;    linefeed=string([10b,13b]) ; HTTP/1.1 defines the sequence CR LF as the end-of-line marker for all protocol elements except the entity-body
  self.response_header=''
     openr,lun,header_filename,/get
     sz=(fstat(lun)).size
     response=bytarr(sz)
     readu, lun, response
     free_lun,lun
     self.response_header=string(response)
     file_delete,header_filename
end

function idlneturl::Get,       BUFFER=buffer, FILENAME=filename,            STRING_ARRAY=string_array, FTP_EXPLICIT_SSL=ftp_explicit_ssl, URL=url
  if n_elements(filename) eq 0  then filename='idl.dat'
  if (STRLEN(self.URL_QUERY) GT 0) then filename=self.URL_QUERY

  scheme=self.URL_SCHEME
  if strlen(scheme) eq 0 then scheme='http'

  userpass=""
  if STRLEN(self.URL_USERNAME) GT 0 then userpass=' -u '+self.URL_USERNAME
  if STRLEN(self.URL_PASSWORD) GT 0 then userpass+=':'+self.URL_PASSWORD
  userpass+=' '

  id_cmd=userpass
  if n_elements(url) gt 0 then begin
     id_cmd+=url
  endif else begin
     id_cmd+=self.URL_SCHEME+'://'+self.URL_HOSTNAME
     if STRLEN(self.URL_PORT) GT 0 then id_cmd+=':'+strtrim(self.URL_PORT,2)
     where=self.URL_PATH
     if strlen(where) gt 0 then id_cmd+='/'+where
     if STRLEN(self.URL_QUERY) GT 0 then id_cmd=id_cmd+'?'+strtrim(self.URL_QUERY,2)
  endelse
;
; AC, 2018-sep-20 : added -L to follow redirections
;
; string curl_cmd contains all the common parameters between the to approaches:
  curl_cmd=""
; authentification
  if self.authentication GT 0 then begin
     case self.authentication of
        1:  curl_cmd+='--basic ' 
        2:  curl_cmd+='--digest ' 
        3:  curl_cmd+='-u --negotiate ' 
     endcase
  endif
; connect timeout  
  if self.connect_timeout gt 0 then curl_cmd+='--connect-timeout '+strtrim(self.connect_timeout,2)+' ' 
  
; encode
  if self.encode GT 0 then curl_cmd+='--tr-encoding ' 
  if self.ftp_connection_mode EQ 0 then curl_cmd+='--ftp-pasv '
; headers
  if ptr_valid(self.headers) and n_elements(*(self.headers)) gt 0 then begin
     for i=0,n_elements(*(self.headers))-1 do begin ; note below curl under Windows accept only "-quote see #1465
        if strlen((*self.headers)[i]) gt 0 then curl_cmd+='-H "'+strtrim((*self.headers)[i],2)+'" ' ; was: if strlen((*self.headers)[i]) gt 0 then curl_cmd+="-H '"+strtrim((*self.headers)[i],2)+"' "
     endfor
  endif

; proxy hostname, untested.
  proxy_cmd=''
  if STRLEN(self.PROXY_HOSTNAME) GT 0 then begin
     proxy_cmd=' -x '+strtrim(self.PROXY_HOSTNAME,2)
     if self.PROXY_PORT ne 80 then proxy_cmd+=':'+strtrim(self.PROXY_PORT,2)+' ' else proxy_cmd+=' '
     if STRLEN(self.PROXY_USERNAME) GT 0 then proxy_cmd+=' --proxy-user '+strtrim(self.PROXY_USERNAME,2)
     if STRLEN(self.PROXY_PASSWORD) GT 0 then proxy_cmd+=':'+strtrim(self.PROXY_PASSWORD,2)
     proxy_cmd+=' '
; proxy authentication
     if self.proxy_authentication GT 0 then begin
        case self.proxy_authentication of
           1:  proxy_cmd+='--proxy-basic ' 
           2:  proxy_cmd+='--proxy-digest ' 
           3:  proxy_cmd+='--proxy-anyauth ' 
        endcase
     endif
     curl_cmd+=proxy_cmd
  endif
; timeout
  if self.timeout gt 0 then curl_cmd+='--max-time '+strtrim(self.timeout,2)+' '

  if strlen(self.CALLBACK_FUNCTION) gt 0 then begin
; GD:
; first, get the response header and treat it, eventually sending some
; data to the callback. This because we cannot send the headers
; asynchronously, we would have to wait until the entire file has
; been downloaded. When we write this function directly in GDL using
; libcurl, things will be way esaier.
  curl_asyn_get_headers='curl -LI --silent --show-error --include '                                                             ; will get only headers
  curl_asyn_get_headers+='--write-out "%{size_header}\n%{content_type}\n%{response_code}\n%{size_download}\n" '    ; we get some useful values
  cmd=curl_asyn_get_headers+id_cmd+'; echo $? '
  if self.verbose then print, cmd
  SPAWN, cmd, result, errResult
  return_code=long(result[-1])
  if return_code ne 0 then begin ; something went wrong, get the curl return value "$?"
     self.content_type=""
     self.response_code=return_code
     self.response_filename=""
     return,""
  endif
; first 4 lines of result are the header size, the content_type, response_code and size_download respectively
  response_header_size=result[-5]
  if response_header_size eq 0 then begin ; in error and errResult is curl's error, throw on it
      message,"CCurlException:  Error: Http Get Request Failed. Error ="+errResult
  endif
  self.content_type=result[-4]
  self.response_code=long(result[-3])
  downloaded=long64(result[-2]) ; should be zero as this is the data downloaded, not the header.
                                ; header of query is response without the last 4 lines
  if n_elements(result) gt 5 then headers=result[0:-6] else headers=''     
                                ; set response header property
  response=self->idlneturl::format_response_header(result,response_header_size)
  
; retrieve content_length if present in headers
  content_length_str=STREGEX(self.response_header, 'Content-Length: *[0-9]+', length=len, /extra)
  if (len gt 0) then begin
     content_length_substr=strsplit(content_length_str,":",/extr)
     content_length=long(content_length_substr[1])
  endif else content_length=0
  
;if content length is zero, it will be impossible to mimic the
;callback by looking at the "progressbar"
  if content_length eq 0 then goto, no_callback
  
  ; insure callback here
     if ptr_valid(self.Callback_Data) then callback_data= *(self.Callback_Data)

;set progressinfo and pass to callback, it's up to it to
;continue or not, we have downloaded 0
     
     StatusInfo="Header"
     ProgressInfo=[1LL,content_length,downloaded,0LL,0LL]
     if call_function(self.CALLBACK_FUNCTION, StatusInfo, ProgressInfo, Callback_Data) eq 0 then return,''
     
; if we are ok, get the real thing:
; cmd will be a spawned commande.
; we need, alongside the data, the CONTENT_TYPE, the RESPONSE_CODE
; (200=success) , the RESPONSE_HEADER and preferably the
; content_length that should be the value of progress_info[1] and the
; number of bytes downloaded at each instant, aka progress_info[2]
; of course if the transfer encoding is not 'chunked'.

     curl_asyn_get_all='curl -L --progress-bar -o "'+filename+'" ' ; will use progressbar values, but alas need output in external file.
; remove /tmp/pb
     file_delete,'/tmp/pb',/allow_nonexistent,/quiet,/noexpand_path
     
     cmd=curl_asyn_get_all+id_cmd+" 2>/tmp/pb &"
; will send data to filename and have the progressbar in /tmp/pb
     if self.verbose then  print, cmd
     
     SPAWN, cmd 
     StatusInfo="Downloading..."
     time=0d
     while (file_test('/tmp/pb') eq 0 and time le 1 ) do begin
        wait,0.1
        time+=0.1
     end
     if (time ge 1) then goto,done ; curl did not even create the progressbar (? to fast or problem?) , skip callback
     ProgressInfo=[1LL,content_length,0LL,0LL,0LL]
     while 1 do begin
        wait,1
        percent=get_percent()
        if percent ge 1 then ProgressInfo[2]=content_length else ProgressInfo[2]=percent*content_length
        if call_function(self.CALLBACK_FUNCTION, StatusInfo, ProgressInfo, Callback_Data ) eq 0 then break ;
        if  percent ge 1 then break                                                                        ;
     end
done:
  endif else begin
no_callback:
; GD:
; if there is no callback function, directly get the file as we do not
; need to be asynchronous. Best is to create the 'filename' directly.

     curl_syn='curl -L --dump-header "/tmp/idlneturl_header.txt" --silent --show-error -o "'+filename+'" ' 
     curl_syn+='--write-out "%{content_type}\n%{response_code}\n" '    ; we get some useful values
     cmd=curl_syn+curl_cmd+id_cmd+'; echo $? '
     if self.verbose then print, cmd
     SPAWN, cmd, result, errResult, count=nblines
     return_code=long(result[-1])
     if return_code ne 0 then begin ; something went wrong, get the curl return value "$?"
        self.content_type=""
        self.response_code=return_code
        self.response_filename=""
        return,""
     endif
     self.content_type=result[0]
     self.response_code=long(result[1])
     self->idlneturl::copy_header_to_response_header,"/tmp/idlneturl_header.txt" ; will be destroyed when this procedure is run
  endelse
  
; clear headers property as specified in documentation
  self->idlneturl::SetProperty,HEADERS = ''
     
; order of Keywords is : STRING, BUFFER, FILENAME
; filename exists, so the filename case is easy.
  if ~KEYWORD_SET(string_array) and ~KEYWORD_SET(buffer) then return,FILE_SEARCH(filename, /full)
; else, read, convert, delete          
  if KEYWORD_SET(buffer) then begin
     openr,lun,filename,/get
     sz=(fstat(lun)).size
     response=bytarr(sz)
     readu, lun, response
     free_lun,lun
     file_delete,filename
     return, response
  endif else if KEYWORD_SET(string_array) then begin
     n=file_lines(filename)
     s="" & resp=strarr(n)
     openr,lun,filename,/get
     for i=0,n-1 do begin & readf,lun,s & resp[i]=s & end
     free_lun,lun
     file_delete,filename
     return, resp
  endif

  return,"" ; not happening anyway
;
end
;
; --------------------------
;
PRO idlneturl::Cleanup
  OBJ_DESTROY, self
end
PRO idlneturl::CloseConnections
  message,/info,"idlneturl::CloseConnexions not yet implemented, FIXME"
end
PRO idlneturl::Delete, URL=URL
  message,/info,"idlneturl::Delete not yet implemented, FIXME"
end
function idlneturl::FtpCommand,command, FTP_EXPLICIT_SSL=z , URL=url
  message,/info,"idlneturl::FtpCommand not yet implemented, FIXME"
end
function idlneturl::GetFtpDirList, FTP_EXPLICIT_SSL=z , URL=url, SHORT=short
  message,/info,"idlneturl::GetFtpDirList not yet implemented, FIXME"
end





















function idlneturl::Put, data, BUFFER=buffer, FILENAME=filename, POST=post, STRING_ARRAY=string_array, FTP_EXPLICIT_SSL=ftp_explicit_ssl, URL=url
; data is?
  action=" "                                                                                   ; send raw data
  if ~KEYWORD_SET(post) then action=" --request PUT "+action ; POST is by default
  if KEYWORD_SET(buffer) then begin
     if ISA(data,/string) then begin
        if n_elements(data) eq 1 then action+="--data '"+data+"' "  else begin
           linefeed=string(10b)
           action+="--data '"+data[0]
           for i=0,n_elements(data-2) do action+=linefeed+data[i]
           action+="' "
         endelse
     endif else begin
        openw,lun,"/tmp/binary",/get
        writeu,lun,data
        free_lun,lun
        action+=' -H "Content-Type: application/octet-stream" --data-binary '
        action+="@/tmp/binary" ; send as filename
     endelse
  endif else begin
     action+="'"+"@"+string(data)+"' " ; send as filename or not
  endelse
  ; if string_array is not set, we'll get the data in this file
  oufile='/tmp/idl.dat'
  if (STRLEN(self.URL_QUERY) GT 0) then oufile=self.URL_QUERY
  
  if n_elements(url) gt 0 then begin
     id_cmd=url
  endif else begin
     id_cmd=''
     if STRLEN(self.URL_USERNAME) GT 0 then id_cmd=' -u '+self.URL_USERNAME
     if STRLEN(self.URL_PASSWORD) GT 0 then id_cmd=id_cmd+':'+self.URL_PASSWORD+' '
     id_cmd=id_cmd+self.URL_SCHEME+'://'+self.URL_HOSTNAME
     if STRLEN(self.URL_PORT) GT 0 then id_cmd=id_cmd+':'+strtrim(self.URL_PORT,2)
     where=self.URL_PATH
     if strlen(where) gt 0 then id_cmd+='/'+where
     if STRLEN(self.URL_QUERY) GT 0 then id_cmd=id_cmd+'?'+strtrim(self.URL_QUERY,2)
  endelse
;
; string curl_cmd contains all the common parameters between the to approaches:
  curl_cmd=""
; authentification
  if self.authentication GT 0 then begin
     case self.authentication of
        1:  curl_cmd+='--basic ' 
        2:  curl_cmd+='--digest ' 
        3:  curl_cmd+='-u --negotiate ' 
     endcase
  endif
; connect timeout  
  if self.connect_timeout gt 0 then curl_cmd+='--connect-timeout '+strtrim(self.connect_timeout,2)+' ' 
  
; encode
  if self.encode GT 0 then curl_cmd+='--tr-encoding ' 
  if self.ftp_connection_mode EQ 0 then curl_cmd+='--ftp-pasv '
; headers
  if ptr_valid(self.headers) and n_elements(*(self.headers)) gt 0 then begin
     for i=0,n_elements(*(self.headers))-1 do begin ; note below curl under Windows accept only "-quote see #1465
        if strlen((*self.headers)[i]) gt 0 then curl_cmd+='-H "'+strtrim((*self.headers)[i],2)+'" ' ; was: if strlen((*self.headers)[i]) gt 0 then curl_cmd+="-H '"+strtrim((*self.headers)[i],2)+"' "
     endfor
  endif
; proxy hostname, untested.
  proxy_cmd=''
  if STRLEN(self.PROXY_HOSTNAME) GT 0 then begin
     proxy_cmd=' -x '+strtrim(self.PROXY_HOSTNAME,2)
     if self.PROXY_PORT ne 80 then proxy_cmd+=':'+strtrim(self.PROXY_PORT,2)+' ' else proxy_cmd+=' '
     if STRLEN(self.PROXY_USERNAME) GT 0 then proxy_cmd+=' --proxy-user '+strtrim(self.PROXY_USERNAME,2)
     if STRLEN(self.PROXY_PASSWORD) GT 0 then proxy_cmd+=':'+strtrim(self.PROXY_PASSWORD,2)
     proxy_cmd+=' '
; proxy authentication
     if self.proxy_authentication GT 0 then begin
        case self.proxy_authentication of
           1:  proxy_cmd+='--proxy-basic ' 
           2:  proxy_cmd+='--proxy-digest ' 
           3:  proxy_cmd+='--proxy-anyauth ' 
        endcase
     endif
     curl_cmd+=proxy_cmd
  endif
; timeout
  if self.timeout gt 0 then curl_cmd+='--max-time '+strtrim(self.timeout,2)+' '

  curl_syn='curl -L --dump-header "/tmp/idlneturl_header.txt" --silent --show-error '                                                        ; will get only headers
  curl_syn+='--write-out "%{content_type}\n%{response_code}\n" '    ; we get some useful values
  curl_syn+=' -o "'+oufile+'" ' ; we always ask for a file to separate contents from '--write-out' header
  cmd=curl_syn+curl_cmd+action+id_cmd+'; echo $? '
  if self.verbose then print, cmd
  SPAWN, cmd, result, errResult
  return_code=long(result[-1])
  if return_code ne 0 then begin ; something went wrong, get the curl return value "$?"
     self.content_type=""
     self.response_code=return_code
     self.response_filename=""
     return,""
  endif
  self.content_type=result[0]
  self.response_code=long(result[1])
  self->idlneturl::copy_header_to_response_header,"/tmp/idlneturl_header.txt" ; will be destroyed when this procedure is run
; clear headers property as specified in documentation
  self->idlneturl::SetProperty,HEADERS = ''
; return:     
  if KEYWORD_SET(string_array) then begin
     n=file_lines(oufile)
     resp=strarr(n) & s=''
     openr,lun,oufile,/get
     for i=0,n-1 do readf,lun,s & resp[i]=s
     free_lun,lun
     file_delete,oufile
     return, resp
   endif else return,FILE_SEARCH(oufile, /full)
end
function idlneturl::URLDecode,String
  message,/info,"idlneturl::URLDecode not yet implemented, FIXME"
end
function idlneturl::URLEncode,String
  message,/info,"idlneturl::URLEncode not yet implemented, FIXME"
end

pro idlneturl::SetProperty,$
   URL_SCHEME = URL_SCHEME, $
   URL_HOSTNAME = URL_HOSTNAME, $
   URL_PATH = URL_PATH, $
   URL_PORT = URL_PORT, $
   URL_QUERY= URL_QUERY, $
   URL_USERNAME =  URL_USERNAME, $
   URL_PASSWORD =  URL_PASSWORD, $
   VERBOSE=VERBOSE, $
   AUTHENTICATION=AUTHENTICATION,$
   CALLBACK_DATA=CALLBACK_DATA,$
   CALLBACK_FUNCTION=CALLBACK_FUNCTION,$
   CONNECT_TIMEOUT=CONNECT_TIMEOUT,$
   ENCODE=ENCODE,$
   FTP_CONNECTION_MODE=FTP_CONNECTION_MODE,$
   HEADERS=HEADERS,$
   PROXY_AUTHENTICATION=PROXY_AUTHENTICATION,$
   PROXY_HOSTNAME=PROXY_HOSTNAME,$
   PROXY_PASSWORD=PROXY_PASSWORD,$
   PROXY_PORT=PROXY_PORT,$
   PROXY_USERNAME=PROXY_USERNAME,$
   SSL_CERTIFICATE_FILE=SSL_CERTIFICATE_FILE,$
   SSL_VERIFY_HOST=SSL_VERIFY_HOST,$
   SSL_VERIFY_PEER=SSL_VERIFY_PEER,$
   SSL_VERSION=SSL_VERSION,$
   TIMEOUT=TIMEOUT,_extra=extra
;
IF N_ELEMENTS(URL_SCHEME) gt 0 then self.URL_SCHEME=URL_SCHEME
IF N_ELEMENTS(URL_HOSTNAME) gt 0 then self.URL_HOSTNAME=URL_HOSTNAME
IF N_ELEMENTS(URL_PATH) gt 0 then self.URL_PATH=URL_PATH
IF N_ELEMENTS(URL_PORT) gt 0 then self.URL_PORT=URL_PORT
IF N_ELEMENTS(URL_QUERY) gt 0 then self.URL_QUERY=URL_QUERY
IF N_ELEMENTS(URL_USERNAME) gt 0 then self.URL_USERNAME=URL_USERNAME
IF N_ELEMENTS(URL_PASSWORD) gt 0 then self.URL_PASSWORD=URL_PASSWORD
IF N_ELEMENTS(VERBOSE) gt 0 then self.VERBOSE=VERBOSE
IF N_ELEMENTS(AUTHENTICATION) gt 0 then self.AUTHENTICATION=AUTHENTICATION

IF N_ELEMENTS(CALLBACK_DATA) gt 0 then begin
   if ptr_valid(self.callback_data) gt 0 then ptr_free,self.callback_data
   self.CALLBACK_DATA=ptr_new(CALLBACK_DATA)
endif

IF N_ELEMENTS(CALLBACK_FUNCTION) gt 0 then self.CALLBACK_FUNCTION=CALLBACK_FUNCTION
IF N_ELEMENTS(CONNECT_TIMEOUT) gt 0 then self.CONNECT_TIMEOUT=CONNECT_TIMEOUT
IF N_ELEMENTS(ENCODE) gt 0 then self.ENCODE=ENCODE
IF N_ELEMENTS(FTP_CONNECTION_MODE) gt 0 then self.FTP_CONNECTION_MODE=FTP_CONNECTION_MODE

IF N_ELEMENTS(HEADERS) gt 0 then begin
  if ptr_valid(self.headers) gt 0 then ptr_free,self.HEADERS
  self.HEADERS=ptr_new(HEADERS)
endif

IF N_ELEMENTS(PROXY_AUTHENTICATION) gt 0 then self.PROXY_AUTHENTICATION=PROXY_AUTHENTICATION
IF N_ELEMENTS(PROXY_HOSTNAME) gt 0 then self.PROXY_HOSTNAME=PROXY_HOSTNAME
IF N_ELEMENTS(PROXY_PASSWORD) gt 0 then self.PROXY_PASSWORD=PROXY_PASSWORD
IF N_ELEMENTS(PROXY_PORT) gt 0 then self.PROXY_PORT=PROXY_PORT
IF N_ELEMENTS(PROXY_USERNAME) gt 0 then self.PROXY_USERNAME=PROXY_USERNAME
IF N_ELEMENTS(SSL_CERTIFICATE_FILE) gt 0 then self.SSL_CERTIFICATE_FILE=SSL_CERTIFICATE_FILE
IF N_ELEMENTS(SSL_VERIFY_HOST) gt 0 then self.SSL_VERIFY_HOST=SSL_VERIFY_HOST
IF N_ELEMENTS(SSL_VERIFY_PEER) gt 0 then self.SSL_VERIFY_PEER=SSL_VERIFY_PEER
IF N_ELEMENTS(SSL_VERSION) gt 0 then self.SSL_VERSION=SSL_VERSION
IF N_ELEMENTS(TIMEOUT) gt 0 then self.TIMEOUT=TIMEOUT
;
end

pro idlneturl::GetProperty,$
   URL_SCHEME = URL_SCHEME, $
   URL_HOSTNAME = URL_HOSTNAME, $
   URL_PATH = URL_PATH, $
   URL_PORT = URL_PORT, $
   URL_QUERY= URL_QUERY, $
   URL_USERNAME =  URL_USERNAME, $
   VERBOSE=VERBOSE, $
   AUTHENTICATION=AUTHENTICATION,$
   CALLBACK_DATA=CALLBACK_DATA,$
   CALLBACK_FUNCTION=CALLBACK_FUNCTION,$
   CONNECT_TIMEOUT=CONNECT_TIMEOUT,$
   ENCODE=ENCODE,$
   FTP_CONNECTION_MODE=FTP_CONNECTION_MODE,$
   HEADERS=HEADERS,$
   PROXY_AUTHENTICATION=PROXY_AUTHENTICATION,$
   PROXY_HOSTNAME=PROXY_HOSTNAME,$
   PROXY_PORT=PROXY_PORT,$
   PROXY_USERNAME=PROXY_USERNAME,$
   SSL_CERTIFICATE_FILE=SSL_CERTIFICATE_FILE,$
   SSL_VERIFY_HOST=SSL_VERIFY_HOST,$
   SSL_VERIFY_PEER=SSL_VERIFY_PEER,$
   SSL_VERSION=SSL_VERSION,$
   TIMEOUT=TIMEOUT, $
   CONTENT_TYPE=CONTENT_TYPE, $
   RESPONSE_CODE=RESPONSE_CODE,$
   RESPONSE_FILENAME=RESPONSE_FILENAME,$
   RESPONSE_HEADER=RESPONSE_HEADER,_extra=extra
;
IF ARG_PRESENT(URL_SCHEME) then URL_SCHEME=self.URL_SCHEME
IF ARG_PRESENT(URL_HOSTNAME) then URL_HOSTNAME=self.URL_HOSTNAME
IF ARG_PRESENT(URL_PATH) then URL_PATH=self.URL_PATH
IF ARG_PRESENT(URL_PORT) then URL_PORT=self.URL_PORT
IF ARG_PRESENT(URL_QUERY) then URL_QUERY=self.URL_QUERY
IF ARG_PRESENT(URL_USERNAME) then URL_USERNAME=self.URL_USERNAME
IF ARG_PRESENT(VERBOSE) then VERBOSE=self.VERBOSE
IF ARG_PRESENT(AUTHENTICATION) then AUTHENTICATION=self.AUTHENTICATION

IF ARG_PRESENT(CALLBACK_DATA) and ptr_valid(self.callback_data) then CALLBACK_DATA=*(self.CALLBACK_DATA)

IF ARG_PRESENT(CALLBACK_FUNCTION) then CALLBACK_FUNCTION=self.CALLBACK_FUNCTION
IF ARG_PRESENT(CONNECT_TIMEOUT) then CONNECT_TIMEOUT=self.CONNECT_TIMEOUT
IF ARG_PRESENT(ENCODE) then ENCODE=self.ENCODE
IF ARG_PRESENT(FTP_CONNECTION_MODE) then FTP_CONNECTION_MODE=self.FTP_CONNECTION_MODE
IF ARG_PRESENT(HEADERS) then begin
  if ptr_valid(self.headers) then HEADERS=*(self.HEADERS)
  if n_elements(HEADERS) LE 1 then HEADERS=HEADERS[0] ; return a
                                ; single value
  endif
IF ARG_PRESENT(PROXY_AUTHENTICATION) then PROXY_AUTHENTICATION=self.PROXY_AUTHENTICATION
IF ARG_PRESENT(PROXY_HOSTNAME) then PROXY_HOSTNAME=self.PROXY_HOSTNAME
IF ARG_PRESENT(PROXY_PORT) then PROXY_PORT=self.PROXY_PORT
IF ARG_PRESENT(PROXY_USERNAME) then PROXY_USERNAME=self.PROXY_USERNAME
IF ARG_PRESENT(SSL_CERTIFICATE_FILE) then SSL_CERTIFICATE_FILE=self.SSL_CERTIFICATE_FILE
IF ARG_PRESENT(SSL_VERIFY_HOST) then SSL_VERIFY_HOST=self.SSL_VERIFY_HOST
IF ARG_PRESENT(SSL_VERIFY_PEER) then SSL_VERIFY_PEER=self.SSL_VERIFY_PEER
IF ARG_PRESENT(SSL_VERSION) then SSL_VERSION=self.SSL_VERSION
IF ARG_PRESENT(TIMEOUT) then TIMEOUT=self.TIMEOUT
IF ARG_PRESENT(CONTENT_TYPE) then CONTENT_TYPE=self.CONTENT_TYPE
IF ARG_PRESENT(RESPONSE_CODE) then RESPONSE_CODE=self.RESPONSE_CODE
IF ARG_PRESENT(RESPONSE_FILENAME) then RESPONSE_FILENAME=self.RESPONSE_FILENAME
IF ARG_PRESENT(RESPONSE_HEADER) then RESPONSE_HEADER=self.RESPONSE_HEADER
;
end
;
; --------------------------
;
function idlneturl::Init,$
   URL_SCHEME = URL_SCHEME, $
   URL_HOSTNAME = URL_HOSTNAME, $
   URL_PATH = URL_PATH, $
   URL_PORT = URL_PORT, $
   URL_QUERY= URL_QUERY, $
   URL_USERNAME =  URL_USERNAME, $
   URL_PASSWORD =  URL_PASSWORD, $
   VERBOSE=VERBOSE, $
   AUTHENTICATION=AUTHENTICATION,$
   CALLBACK_DATA=CALLBACK_DATA,$
   CALLBACK_FUNCTION=CALLBACK_FUNCTION,$
   CONNECT_TIMEOUT=CONNECT_TIMEOUT,$
   ENCODE=ENCODE,$
   FTP_CONNECTION_MODE=FTP_CONNECTION_MODE,$
   HEADERS=HEADERS,$
   PROXY_AUTHENTICATION=PROXY_AUTHENTICATION,$
   PROXY_HOSTNAME=PROXY_HOSTNAME,$
   PROXY_PASSWORD=PROXY_PASSWORD,$
   PROXY_PORT=PROXY_PORT,$
   PROXY_USERNAME=PROXY_USERNAME,$
   SSL_CERTIFICATE_FILE=SSL_CERTIFICATE_FILE,$
   SSL_VERIFY_HOST=SSL_VERIFY_HOST,$
   SSL_VERIFY_PEER=SSL_VERIFY_PEER,$
   SSL_VERSION=SSL_VERSION,$
   TIMEOUT=TIMEOUT,_extra=extra

IF ~KEYWORD_SET(URL_SCHEME) then URL_SCHEME='http'
IF ~KEYWORD_SET(URL_PORT) then URL_PORT=80L
IF ~KEYWORD_SET(VERBOSE) then VERBOSE=0L
IF ~KEYWORD_SET(CONNECT_TIMEOUT) then CONNECT_TIMEOUT=180L
IF ~KEYWORD_SET(ENCODE) then ENCODE=3L
IF ~KEYWORD_SET(FTP_CONNECTION_MODE) then FTP_CONNECTION_MODE=1L
IF ~KEYWORD_SET(PROXY_AUTHENTICATION) then PROXY_AUTHENTICATION=0L
IF ~KEYWORD_SET(PROXY_PORT) then PROXY_PORT=80L
;IF ~KEYWORD_SET(SSL_CERTIFICATE_FILE) then SSL_CERTIFICATE_FILE='/usr/share/openvpn/sample-keys/client.crt'
IF ~KEYWORD_SET(SSL_VERIFY_HOST) then SSL_VERIFY_HOST=1L
IF ~KEYWORD_SET(SSL_VERIFY_PEER) then SSL_VERIFY_PEER=1L
IF ~KEYWORD_SET(SSL_VERSION) then SSL_VERSION=0L
IF ~KEYWORD_SET(TIMEOUT) then TIMEOUT=1800L
IF ~KEYWORD_SET(HEADERS) then HEADERS=''
  
;
  self->idlneturl::SetProperty,URL_SCHEME = URL_SCHEME, $
                    URL_HOSTNAME = URL_HOSTNAME, $
                    URL_PATH = URL_PATH, $
                    URL_PORT = URL_PORT, $
                    URL_QUERY= URL_QUERY, $
                    URL_USERNAME =  URL_USERNAME, $
                    URL_PASSWORD =  URL_PASSWORD, $
                    VERBOSE=VERBOSE, $
                    AUTHENTICATION=AUTHENTICATION,$
                    CALLBACK_DATA=CALLBACK_DATA,$
                    CALLBACK_FUNCTION=CALLBACK_FUNCTION,$
                    CONNECT_TIMEOUT=CONNECT_TIMEOUT,$
                    ENCODE=ENCODE,$
                    FTP_CONNECTION_MODE=FTP_CONNECTION_MODE,$
                    HEADERS=HEADERS,$
                    PROXY_AUTHENTICATION=PROXY_AUTHENTICATION,$
                    PROXY_HOSTNAME=PROXY_HOSTNAME,$
                    PROXY_PASSWORD=PROXY_PASSWORD,$
                    PROXY_PORT=PROXY_PORT,$
                    PROXY_USERNAME=PROXY_USERNAME,$
                    SSL_CERTIFICATE_FILE=SSL_CERTIFICATE_FILE,$
                    SSL_VERIFY_HOST=SSL_VERIFY_HOST,$
                    SSL_VERIFY_PEER=SSL_VERIFY_PEER,$
                    SSL_VERSION=SSL_VERSION,$
                    TIMEOUT=TIMEOUT,_extra=extra
return, 1

end

pro idlneturl__define
  struct = {IDLNETURL, $
            IDLNETURL_TOP : 0ll, $
            CURLPTR       : 0Ll, $
            URL_SCHEME: 'http',$
            URL_HOSTNAME: '',$
            URL_PATH: '',$
            URL_PORT: '80',$
            URL_QUERY: '',$
            URL_USERNAME: '',$
            IDLNETURL_BOTTOM : 0Ll,$
            URL_PASSWORD  : '',$
            AUTHENTICATION: 3L,$
            CALLBACK_DATA: ptr_new(/allo),$
            CALLBACK_FUNCTION: '',$
            CONNECT_TIMEOUT: 180L,$
            CONTENT_TYPE: '',$
            ENCODE: 0L,$
            FTP_CONNECTION_MODE: 1L,$
            HEADERS : ptr_new(/ALLO),$   ;HEADERS: '',$
            PROXY_AUTHENTICATION: 3L,$
            PROXY_HOSTNAME: '',$
            PROXY_PASSWORD : '',$
            PROXY_PORT: '80',$
            PROXY_USERNAME: '',$
            RESPONSE_CODE: 0L,$
            RESPONSE_FILENAME: '',$
            RESPONSE_HEADER: '',$
            SSL_CERTIFICATE_FILE: '',$
            SSL_VERIFY_HOST: 1L,$
            SSL_VERIFY_PEER: 1L,$
            SSL_VERSION: 0L,$
            TIMEOUT: 1800L,$
            VERBOSE: 0L}
  return
end

