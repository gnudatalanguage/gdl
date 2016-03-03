;
; Gilles
;
pro TEST_BITS_PER_PIX, filename=filename
;
if ~KEYWORD_SET(filename) then filename='Saturn.jpg'
;
file=FILE_SEARCH_FOR_TESTSUITE(filename)
queryStatus = QUERY_IMAGE(file, imageInfo)
if (queryStatus eq 0) then begin
    MESSAGE, /info, 'Image for test ('+filename+') not found, test aborted'
    return
end
image = READ_IMAGE(file)
;
DEFSYSV, '!gdl', exists=isGDL
if isGDL then prefix='GDL_' else prefix='IDL_'
prefix=prefix+'test_'
;
SET_PLOT,'PS'
;
device,filename=prefix+'8bpp_true.ps',/encap,/color,bits_per_pixel=8
tvscl,image,/true
device,/close

device,filename=prefix+'4bpp_true.ps',/encap,/color,bits_per_pixel=4
tvscl,image,/true
device,/close

device,filename=prefix+'2bpp_true.ps',/encap,/color,bits_per_pixel=2
tvscl,image,/true
device,/close

device,filename=prefix+'1bpp_true.ps',/encap,/color,bits_per_pixel=1
tvscl,image,/true
device,/close

device,filename=prefix+'8bpp.ps',/encap,/color,bits_per_pixel=8
tvscl,image[0,*,*]
device,/close

device,filename=prefix+'4bpp.ps',/encap,/color,bits_per_pixel=4
tvscl,image[0,*,*]
device,/close

device,filename=prefix+'2bpp.ps',/encap,/color,bits_per_pixel=2
tvscl,image[0,*,*]
device,/close

device,filename=prefix+'1bpp.ps',/encap,/color,bits_per_pixel=1
tvscl,image[0,*,*]
device,/close

end

