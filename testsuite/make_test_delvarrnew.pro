;+
; make_test_delvarrnew.pro
; create the dependent files, delvarrnew.pro and rnewtest.pro, 
; in the directory where test_delvarrnew.pro is run.
;-
openw,lunw,/get,'test_delvarrnew.pro'

openr,lunin,/get,'test_delvarrnew.pp.pro'
sline = ""
readf,lunin,sline

while (signal = strmid(sline,0,4)) ne ";<<=" do begin &$
	printf,lunw,sline &$
	readf,lunin,sline &$
	endwhile

openr,lun2,/get,'delvarrnew.pp.pro'	
printf,lunw," openw,lunw,/get,'delvarrnew.pro'"
nline = file_lines(	'delvarrnew.pp.pro')
sinsert = strarr(nline)
s=""
for k=0,nline-1 do begin & readf,lun2,s & printf,lunw,' printf,lunw,"'+s+'"' & endfor
printf,lunw," free_lun,lunw"
free_lun,lun2

openr,lun2,/get,'rnewtest.pp.pro'	
printf,lunw," openw,lunw,/get,'rnewtest.pro'"
nline = file_lines(	'rnewtest.pp.pro')
sinsert = strarr(nline)
s=""
for k=0,nline-1 do begin & readf,lun2,s & printf,lunw,' printf,lunw,"'+s+'"' & endfor
printf,lunw," free_lun,lunw"
free_lun,lun2

printf,lunw,'; resuming copy of test_delvarrnew.pp.pro'

readf,lunin,sline
while ~eof(lunin) do begin & printf,lunw,sline & readf,lunin,sline & endwhile
printf,lunw,sline
	
	free_lun,lunin,lunw

end
