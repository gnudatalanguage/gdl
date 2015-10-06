pro test_bits_per_pix
filename='Saturn.jpg'
list_of_dirs=STRSPLIT(!PATH, PATH_SEP(/SEARCH_PATH), /EXTRACT)
file=FILE_SEARCH(list_of_dirs+PATH_SEP()+filename)
queryStatus = QUERY_IMAGE(file, imageInfo)
image = READ_IMAGE(file)
SET_PLOT,'PS'

device,filename="test_8bpp_true.ps",/encap,/color,bits_per_pixel=8
tvscl,image,/true
device,/close

device,filename="test_4bpp_true.ps",/encap,/color,bits_per_pixel=4
tvscl,image,/true
device,/close

device,filename="test_2bpp_true.ps",/encap,/color,bits_per_pixel=2
tvscl,image,/true
device,/close

device,filename="test_1bpp_true.ps",/encap,/color,bits_per_pixel=1
tvscl,image,/true
device,/close

device,filename="test_8bpp.ps",/encap,/color,bits_per_pixel=8
tvscl,image[0,*,*]
device,/close

device,filename="test_4bpp.ps",/encap,/color,bits_per_pixel=4
tvscl,image[0,*,*]
device,/close

device,filename="test_2bpp.ps",/encap,/color,bits_per_pixel=2
tvscl,image[0,*,*]
device,/close

device,filename="test_1bpp.ps",/encap,/color,bits_per_pixel=1
tvscl,image[0,*,*]
device,/close

end

