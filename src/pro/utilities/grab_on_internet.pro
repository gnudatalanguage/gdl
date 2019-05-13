;
; Alain C., 2019-Jan-15, under GNU GPL v3+
;
; Donwload (using Curl or Wget) a list of URL.
; We test whether we need to overwrite or not ...
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
pro GRAB_ON_INTERNET, list_of_url, quiet=quiet, overwrite=overwrite, $
                      status=status, count=count, $
                      verbose=verbose, test=test, help=help
;
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
   print, 'pro GRAB_ON_INTERNET, list_of_url, overwrite=overwrite, $'
   print, '                      status=status, count=count, $'
   print, '                      verbose=verbose, test=test, help=help'
   return
endif
;
if N_PARAMS() NE 1 then MESSAGE, 'missing input (list of URL)'
;
cmd=WGET_OR_CURL(quiet=quiet)
if (STRLEN(cmd) EQ 0) then MESSAGE, 'Wget or Curl not found ...'
;
if KEYWORD_SET(count) then counting=0
;
if KEYWORD_SET(overwrite) then begin
   for ii=0, N_ELEMENTS(list_of_url)-1 do begin
      cur_file=FILE_BASENAME(list_of_url[ii])
      if FILE_TEST(cur_file) then FILE_DELETE, cur_file
      SPAWN, cmd+list_of_url[ii], exit_status=exit_status
      if (exit_status NE 0) then begin
         print, 'Problem with URL : ', list_of_url[ii]
      endif else begin
         counting++
         if KEYWORD_SET(verbose) then begin
            txt='Downloaded file : '+FILE_BASENAME(list_of_url[ii])
            print, txt+' at URL : '+FILE_DIRNAME(list_of_url[ii])
         endif
      endelse
   endfor
endif else begin
   for ii=0, N_ELEMENTS(list_of_url)-1 do begin
      cur_file=FILE_BASENAME(list_of_url[ii])
      if ~FILE_TEST(cur_file) then begin 
         SPAWN, cmd+list_of_url[ii], exit_status=exit_status
         if (exit_status NE 0) then begin
            print, 'Problem with URL : ', list_of_url[ii]
         endif else begin
            counting++
         endelse
      endif else begin
         if KEYWORD_SET(verbose) then print, 'File '+cur_file+' already present, not overwritten'
      endelse
   endfor
endelse
;
if KEYWORD_SET(count) then print, 'Files downloaded : ', counting
;
if KEYWORD_SET(test) then STOP
;
end
