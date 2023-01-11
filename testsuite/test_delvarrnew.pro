pro test_delvarrnew

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
    spawn,'../src/gdl -quiet ./delvarrnew.pro', result & nres=n_elements(result)
  endif else begin
    spawn,'..\src\gdl -quiet delvarrnew.pro', result & nres=n_elements(result)
  endelse
  
  stop
  file_delete,'delvarrnew.pro'
  file_delete,'rnewtest.pro'
  if nres lt 2 then exit, status=1

  if(result[nres-1] eq 'FAILURE') then exit, status=1
  banner_for_testsuite,' DELVAR-RNEW',' works! (from $main)'

end
