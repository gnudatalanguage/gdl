
igot=fltarr(4) & iget=findgen(4)
help,igot,iget,output=igstr & nstrb=n_elements(igstr)
for k=0,nstrb-1 do print,igstr[k]
delvar,igot,iget,igstr
nval= size(igot,/type) + size(iget,/type) +size(igstr,/type)

if nval ne 0 then begin & $
	print,'FAILURE' &$
	exit & endif
i=1 & j=2 & k=3
.rnew rnewtest
if(size(i,/type) + size(j,/type) +size(k,/type) eq 0) then print,'ALRIGHT' else $
   print,'FAILURE'

exit, status=0
