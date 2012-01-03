$ echo '$ cat libexample.c' 
$ cat libexample.c
$ echo '$ cat CMakeLists.txt'
$ cat CMakeLists.txt 
$ echo '$ cmake .'
$ cmake .|awk '{print (length($0)>50?substr($0,0,50) "...":$0)}'
$ echo
$ echo '$ make' 
$ make
$ echo

img = 'libexample.'+(!VERSION.OS_NAME eq 'darwin'?"dylib":"so")
message, '1d308 vs. a next representable double:', /continue
print, format='(E)', 1d308, $
  call_external(img, 'c_nextafter', 1d308, 2d308, /d_value)

$ make clean
