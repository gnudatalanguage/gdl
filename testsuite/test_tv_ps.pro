pro test_tv_ps,encap=encap,port=port
filename='Saturn.jpg'
list_of_dirs=STRSPLIT(!PATH, PATH_SEP(/SEARCH_PATH), /EXTRACT)
file=FILE_SEARCH(list_of_dirs+PATH_SEP()+filename)
queryStatus = QUERY_IMAGE(file, imageInfo)
image = READ_IMAGE(file)
SET_PLOT,'PS'
device,filename="test_tv.ps",encap=encap,port=port,/color
aa=findgen(60)
!P.MULTI=[0,3,2]
for i=0,5 do begin
plot,aa,ticklen=1
TV, image,10*i,10*i,/DATA,/true,xsize=50
end
device,/close
end

