tdir="/home/chris/ei/"

print,"Read BMP TrueColor"
a=read_bmp(tdir+"tc24.bmp")
print,"Write BMP"
write_bmp,tdir+"tc24_bmp.bmp",a

print,"Read TIFF TrueColor"
a=read_tiff(tdir+"tc24.tiff")
print,"Write BMP"
write_bmp,tdir+"tc24_tiff.bmp",a

print,"Read PNG TrueColor"
a=read_png(tdir+"tc24.png")
print,"Write PNG"
write_png,tdir+"tc24_png.png",a

print,"Read JPG TrueColor"
read_jpeg,tdir+"tc24.jpg",a
print,"Write JPEG"
write_jpeg,tdir+"tc24_jpg.jpg",a

print,"Read PICT TrueColor"
read_jpeg,tdir+"tc24.pict",a
print,"Write PICT"
write_jpeg,tdir+"tc24_pict.pict",a


print,"Read BMP Indexed"
a=read_bmp(tdir+"i256.bmp",r,g,b)
print,"Write BMP"
write_bmp,tdir+"i256_bmp.bmp",a,r,g,b

print,"Read TIFF Indexed"
a=read_tiff(tdir+"i256.tiff",r,g,b)
print,"Write BMP"
write_bmp,tdir+"i256_tiff.bmp",a,r,g,b

print,"Read PNG Indexed"
a=read_png(tdir+"i256.png",r,g,b)
print,"Write PNG"
write_png,tdir+"i256_png.png",a,r,g,b

print,"Read PICT Indexed"
read_jpeg,tdir+"i256.pict",a
print,"Write PICT"
write_jpeg,tdir+"i256_pict.pict",a


a=read_bmp(tdir+"i256_bmp.bmp",r,g,b) & $
write_bmp,tdir+"i256_bmp100.bmp",a,r,g,b & $
for i=0, 100 do begin & $
print, "Quantizing: ",i & $
a=read_bmp(tdir+"i256_bmp100.bmp",r,g,b) & $
write_bmp,tdir+"i256_bmp100.bmp",a,r,g,b & $
end
