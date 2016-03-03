;
; very preliminary test cases for PostScript outputs and TV ...
; To be extended
;
pro TEST_TV_PS, encap=encap, port=port
;
DEFSYSV, '!gdl', exists=isGDL
if isGDL then prefix='GDL_' else prefix='IDL_'
;
filename='Saturn.jpg'
file=FILE_SEARCH_FOR_TESTSUITE(filename)
queryStatus = QUERY_IMAGE(file, imageInfo)
if (queryStatus eq 0) then begin
    MESSAGE, /info, "Image for test (Saturn.jpg) not found, test aborted"
    exit, status=77
endif
image = READ_IMAGE(file)
;
SET_PLOT,'PS'
DEVICE, filename=prefix+"test_tv.ps", encap=encap, port=port, /color
;
aa=FINDGEN(60)
!P.MULTI=[0,3,2]
for i=0,5 do begin
    PLOT,aa,ticklen=1
    TV, image,10*i,10*i,/DATA,/true,xsize=50
end
;
DEVICE, /close
;
end

