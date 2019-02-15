;
; Alain C., 2019-Jan-07, under GNU GPL v3+
;
; Do we have WGET or CURL around ?
;
; I don' know whether this code works on MSwin, please report
; or contribute.
;
; ----------------------------------------------------
; Modifications history :
;
; 2019-Jan-15 : AC. Creation
;
; ---------------------------------------------------
;
function WGET_OR_CURL, quiet=quiet, verbose=verbose, test=test
;
command=''
;
wget_ok=0
SPAWN, 'which wget', res
if STRLEN(res) GT 0 then wget_ok=1
wget_cmd='wget -nc '
if KEYWORD_SET(quiet) then wget_cmd=wget_cmd+'-q '
;
curl_ok=0
SPAWN, 'which curl', res
if STRLEN(res) GT 0 then curl_ok=1
; AC, 2018-sep-20 : option -L in curl
curl_cmd='curl -L -O '
if KEYWORD_SET(quiet) then curl_cmd=curl_cmd+'-s '
;
if ((wget_ok EQ 0) AND (curl_ok EQ 0)) then begin
   MESSAGE, /continue, 'No download tool (wget, curl) found'
   MESSAGE, /continue, 'If need you can download by hand or add one of those tools'
endif
;
if KEYWORD_SET(verbose) then begin
   if curl_ok then MESSAGE, /continue, 'CURL command found : '+curl_cmd
   if wget_ok then MESSAGE, /continue, 'WGET command found : '+wget_cmd
endif
;
; if both detected, we prefer to use Wget than Curl ...
if curl_ok then command=curl_cmd
if wget_ok then command=wget_cmd
;
if KEYWORD_SET(test) then STOP
;
return, command
;
end
;
