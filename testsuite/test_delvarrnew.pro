;
; Long story with the orignal version !
;
; ---------------------------------------
; Modifications history :
;
; - 2024-JAN-08 : AC. Limited cleanup. On Linux, can be run in any place
;
; ---------------------------------------
;
pro TEST_DELVARRNEW, help=help, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_DELVARRNEW, help=help, test=test, no_exit=no_exit'
   return
endif 

;
openw,lunw,/get,'delvarrnew.pro'
printf,lunw,""
printf,lunw,"igot=fltarr(4) & iget=findgen(4)"
printf,lunw,"help,igot,iget,output=igstr & nstrb=n_elements(igstr)"
printf,lunw,"for k=0,nstrb-1 do print,igstr[k]"
printf,lunw,"delvar,igot,iget,igstr"
printf,lunw,"nval= size(igot,/type) + size(iget,/type) +size(igstr,/type)"
printf,lunw,""
printf,lunw,"if nval ne 0 then begin & $"
printf,lunw,"	print,'FAILURE' &$"
printf,lunw,"	exit & endif"
printf,lunw,"i=1 & j=2 & k=3"
printf,lunw,".rnew rnewtest"
printf,lunw,"if(size(i,/type) + size(j,/type) +size(k,/type) eq 0) then print,'ALRIGHT' else print,'FAILURE'"
printf,lunw,""
printf,lunw,"exit, status=0"
free_lun,lunw

openw,lunw,/get,'rnewtest.pro'
printf,lunw,"delvar,i,j,k,igstr"
printf,lunw,"end"
free_lun,lunw

; now that the dependent files are written to the current directory,
; execute them
if !version.os_family ne 'Windows' then begin
   path_to_exe=GET_PATH_TO_EXE()
   SPAWN, path_to_exe+' -quiet ./delvarrnew.pro', result
   nres=N_ELEMENTS(result)
endif else begin
   spawn,'..\src\gdl -quiet delvarrnew.pro', result
   nres=N_ELEMENTS(result)
endelse
;
FILE_DELETE,'delvarrnew.pro'
FILE_DELETE,'rnewtest.pro'
;
nb_errors=0
;
if (nres lt 2) then begin
   ERRORS_ADD, nb_errors, 'nres NT 2'
endif else begin
   if (result[nres-1] eq 'FAILURE') then ERRORS_ADD, nb_errors, 'FAILURE'
endelse
;
if nb_errors EQ 0 then begin
   BANNER_FOR_TESTSUITE,' DELVAR-RNEW',' works! (from $main)',/short
endif
; 
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_DELVARRRNEW', nb_errors, short=short
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
