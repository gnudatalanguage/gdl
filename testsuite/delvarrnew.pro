
igot=fltarr(4) & iget=findgen(4)
help,igot,iget,output=igstr & nstrb=n_elements(igstr)
for k=0,nstrb-1 do print,igstr[k]
delvar,igot,iget,igstr
help,igot,iget,output=igstr & nstre=n_elements(igstr)
if(nstrb-nstre) ne 2 then begin & $
	print,'FAILURE' &$
	exit & endif
i=1 & j=2 & k=3
.rnew rnewtest
help,i,j,k,output=igstr & if(n_elements(igstr) eq 0) then print,'ALRIGHT' else $
   print,'FAILURE'

exit, status=0
