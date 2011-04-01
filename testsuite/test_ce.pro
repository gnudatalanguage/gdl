;
; initial version written by Sylwester.
;
; Since this version is supposed to be used within "make check" where
; the lib is compiled "in fligth"
; it is NOT checked wether the "libtest_ce.so" is available or not.
;
; inline compiling of lib by Alain (Linux version only now)
; (can be used in interactive mode OR in Cmake call to testsuite with
; "make test")
;
;;-----------------------------------------------------------------------
;; test suite for IDL/GDL call_external
;;
;; should be used together with test_ce.so compiled from test_ce.c
;;-----------------------------------------------------------------------
;
pro COMPILING_LIBTEST_CE, file=file,  image=image, path=path, $
                          test=test,  debug=debug
;
IF (N_ELEMENTS(image) EQ 0) THEN BEGIN
    image = 'libtest_ce.' + (!VERSION.OS_NAME eq 'Darwin' ? "dylib" : "so")
ENDIF
IF (N_ELEMENTS(file) EQ 0) THEN BEGIN
    file = 'libtest_ce.cpp'
endif
IF (N_ELEMENTS(path) EQ 0) THEN BEGIN
    path2lib = '.libs/'
ENDIF ELSE BEGIN
    path2lib = path
endelse
;
image_name = path2lib+image

fullname = FILE_WHICH(file)
path2file = STRSPLIT(fullname, file, /extract, /regex)

new_dir = path2file+path2lib
;FILE_MKDIR, new_dir,  /noexpand
CD, path2file, CURRENT=old_dir
SPAWN, 'mkdir '+path2lib,  status

IF ((FILE_INFO(new_dir)).exists EQ 0) THEN STOP

IF KEYWORD_SET(debug) THEN BEGIN
    print, 'Ref Dir: ', old_dir
    print, 'New Dir: ', path2file
endif

command = ""
IF STRLOWCASE(!version.os) EQ 'linux' THEN begin
   command = "cc -o "+image +" "+ fullname +" -shared -fPIC"
endif
IF STRLOWCASE(!version.os) EQ 'darwin' THEN begin
   command = "cc -o "+image +" -c "+ fullname +" -shared -fPIC"
endif
IF STRLOWCASE(!version.os) EQ 'sunos' THEN begin
   command = "cc -o "+image +" "+ fullname +" -G -KPIC"
endif

if (STRLEN(command) GT 0) then begin
   CD, path2lib
   SPAWN, command,  status
   print, status
   CD,  '..'
ENDIF ELSE BEGIN
    MESSAGE,  "only linux, OSX Darwin and SunOS versions ready now"
endelse
;
CD, old_dir
;
END
;
; -------------------------------------------------
;
FUNCTION CHECK_LIBTEST_CE, file=file,  image=image, path=path, $
                           test=test,  debug=debug
;
IF (N_ELEMENTS(image) EQ 0) THEN BEGIN
    image = 'libtest_ce.' + (!VERSION.OS_NAME eq 'Darwin' ? "dylib" : "so")
ENDIF
IF (N_ELEMENTS(file) EQ 0) THEN BEGIN
    file = 'libtest_ce.cpp'
endif
IF (N_ELEMENTS(path) EQ 0) THEN BEGIN
    path2lib = '.libs/'
ENDIF ELSE BEGIN
    path2lib = path
endelse
;
image_name = path2lib+image
;
; We look for the CPP code in the !PATH
;
fullname = FILE_WHICH(file)
;
IF (STRLEN(fullname) EQ 0) THEN BEGIN
    MESSAGE,  "Cpp Code <<"+file+">> not found in !PATH"
    return,  -1
endif
;
; We compute the PATH to the CPP code
;
path2file = STRSPLIT(fullname, file, /extract, /regex)
;
; WE go into this Directory, since Image is in a sub-dir of this
; (testsuite) Dir.
;
CD, path2file, CURRENT=old_dir
;
; does the lib. exist (was it compiled before ?)
;
check = (FILE_INFO(image_name)).exists
;
IF KEYWORD_SET(test) THEN BEGIN
    print, 'Ref Dir: ', old_dir
    print, 'New Dir: ', path2file
    print, 'Lib does exist ? '+(check eq 1 ? "YES" : "NO")
endif
;
CD, old_dir
;
IF KEYWORD_SET(debug) THEN STOP
;
return, check
;
end
;
; -------------------------------------------------
;
pro TEST_CE, file=file, image=image, path=path, overwrite=overwrite, $
             test=test, help=help
;
IF KEYWORD_SET(help) THEN BEGIN
    print, 'pro TEST_CE, file=file, image=image, path=path, overwrite=overwrite, $'
    print, '             test=test, help=help'
endif
;
IF (N_ELEMENTS(image) EQ 0) THEN BEGIN
    image_name = 'libtest_ce.' + (!VERSION.OS_NAME eq 'Darwin' ? "dylib" : "so")
endif ELSE BEGIN
    image_name = image
endelse
;
IF (N_ELEMENTS(path) EQ 0) THEN BEGIN
   image_path = '.libs/'
endif ELSE BEGIN
   if (STRMID(path, STRLEN(path)-1,1) NE '/') then path=path+PATH_SEP()
    image_path = path
endelse
;
IF (N_ELEMENTS(file) EQ 0) THEN BEGIN
    file = 'libtest_ce.cpp'
ENDIF
;;
;; First we checked whether the lib is available or not
;;
do_compil = 0
IF (CHECK_LIBTEST_CE(file=file, image=image, path=image_path) EQ 0) THEN BEGIN
    print, "we will try to make an inline compilation of the lib."
    print, "You may have problem related to you OS/compiler, please report pbs"
    do_compil = 1
ENDIF
IF KEYWORD_SET(overwrite) THEN BEGIN
    print, "we will overwrite the lib."
    print, "You may have problem related to you OS/compiler, please report pbs"
    do_compil = 1
endif
IF (do_compil EQ 1) THEN begin
    COMPILING_LIBTEST_CE, file=file, image=image, path=image_path
    ;;
    IF (CHECK_LIBTEST_CE(file=file, image=image, path=image_path) EQ 0) THEN BEGIN
        MESSAGE,  "inline compilation of the lib. failed ..."
    ENDIF
endif
;;
;; we go to the root dir to the lib
fullname = FILE_WHICH(file)
path = STRSPLIT(fullname, file, /extract, /regex)
CD, path, CURRENT=old_dir
;;
;;-----------------------------------------------------------------------
;; test suite for IDL/GDL call_external
;;
;; should be used together with test_ce.so compiled from test_ce.c
;;-----------------------------------------------------------------------

;; from here to the End, the "image" meaning and value change
;; to the initial meaning (path+libname)
;;
image = image_path + image_name
error = 0

if (FILE_TEST(image) EQ 0) then begin
    MESSAGE,  /CONTINUE, 'the lib file was not found'
    exit, status=77
endif

;; --------------------------------------------------------------------------
;; Byte 

entry = 'testce_byte'

p1 = 1B
p2 = [2B,3B]
p3 = 4B

print, 'Starting test Byte'
ret1 = CALL_EXTERNAL(image, entry, p1, p2, p3, value=[0,0,1], return_type=1)

if (size(ret1, /type) ne 1        $
    or  ret1 ne 14B                         $
    or  p1  ne 11B                         $
    or  p2[0] ne 12B                       $
    or  p2[1] ne 13B                       $
    or  p3    ne 4B                        $
   ) then begin
    print, 'back in IDL: Error:'
    print, 'Return value:  Got: ', ret1
    print, '          Expected: ', 14B
    print, 'First arg:     Got: ', p1
    print, '          Expected: ', 11B
    print, 'Second arg     Got: ', p2
    print, '          Expected: ', [12B, 13B]
    print, 'Third arg      Got: ', p3
    print, '          Expected: ', 4B
    error = error + 1
    ;;stop
endif else begin
    print, 'back in IDL: OK'
    print
endelse


;; --------------------------------------------------------------------------
;; Int  

entry = 'testce_int'

p1 = 1
p2 = [2,3]
p3 = 4

print, 'Starting test Int'
ret2 = call_external(image, entry, p1, p2, p3, value=[0,0,1], return_type=2)

if (    size(ret2, /type) ne 2              $
        or  ret2 ne 14            	$
        or  p1  ne -11            	$
        or  p2[0] ne 12          	$
        or  p2[1] ne 13          	$
        or  p3    ne 4           	$
   ) then begin
    print, 'back in IDL: Error:'
    print, 'Return value:  Got: ', ret2
    print, '          Expected: ', 14
    print, 'First arg:     Got: ', p1
    print, '          Expected: ', -11
    print, 'Second arg     Got: ', p2
    print, '          Expected: ', [12, 13]
    print, 'Third arg      Got: ', p3
    print, '          Expected: ', 4
    error = error + 1
    ;;stop
endif else begin
    print, 'back in IDL: OK'
    print
endelse

;; --------------------------------------------------------------------------
;; Long 

entry = 'testce_long'

p1 = 1L
p2 = [2L,3L]
p3 = 4L

print, 'Starting test Long'
ret3 = call_external(image, entry, p1, p2, p3, value=[0,0,1], return_type=3)

if (    size(ret3, /type) ne 3	$
        or  ret3 ne 14L           	$
        or  p1  ne -11L           	$
        or  p2[0] ne 12L         	$
        or  p2[1] ne 13L         	$
        or  p3    ne 4L          	$
   ) then begin
    print, 'back in IDL: Error:'
    print, 'Return value:  Got: ', ret3
    print, '          Expected: ', 14L
    print, 'First arg:     Got: ', p1
    print, '          Expected: ', -11L
    print, 'Second arg     Got: ', p2
    print, '          Expected: ', [12L, 13L]
    print, 'Third arg      Got: ', p3
    print, '          Expected: ', 4L
    error = error + 1
    ;;stop
endif else begin
    print, 'back in IDL: OK'
    print
endelse


;; --------------------------------------------------------------------------
;; Float

entry = 'testce_float'

p1 = 1.11
p2 = [2.22,3.33]
p3 = 4.44

print, 'Starting test Float'
ret4 = call_external(image, entry, p1, p2, p3, value=[0,0,1], return_type=4)

EPSILON = 1e-4

if (    size(ret4, /type) ne 4	$
        or  abs(ret4 - 14.14) gt EPSILON        $
        or  abs(p1  - 11.11) gt EPSILON        $
        or  abs(p2[0] - 12.12) gt EPSILON      $
        or  abs(p2[1] - 13.13) gt EPSILON      $
        or  abs(p3    - 4.44) gt EPSILON        $
   ) then begin
    print, 'back in IDL: Error:'
    print, 'Return value:  Got: ', ret4
    print, '          Expected: ', 14.14
    print, 'First arg:     Got: ', p1
    print, '          Expected: ', 11.11
    print, 'Second arg     Got: ', p2
    print, '          Expected: ', [12.12, 13.13]
    print, 'Third arg      Got: ', p3
    print, '          Expected: ', 4.44
    error = error + 1
    ;;stop
endif else begin
    print, 'back in IDL: OK'
    print
endelse


;; --------------------------------------------------------------------------
;; Double (no passing by value here, see later 32/64 bit)

entry = 'testce_double'

p1 = 1.11D
p2 = [2.22D,3.33D]

print, 'Starting test double'
ret5 = call_external(image, entry, p1, p2, return_type=5)

EPSILON = 1d-8

if (    size(ret5, /type) ne 5	$
        or  abs(ret5 - 14.14D) gt EPSILON       $
        or  abs(p1  - 11.11D) gt EPSILON        $
        or  abs(p2[0] - 12.12D) gt EPSILON      $
        or  abs(p2[1] - 13.13D) gt EPSILON      $
   ) then begin
    print, 'back in IDL: Error:'
    print, 'Return value:  Got: ', ret5
    print, '          Expected: ', 14.14D
    print, 'First arg:     Got: ', p1
    print, '          Expected: ', 11.11D
    print, 'Second arg     Got: ', p2
    print, '          Expected: ', [12.12D, 13.13D]
    error = error + 1
    ;;stop
endif else begin
    print, 'back in IDL: OK'
    print
endelse


;; --------------------------------------------------------------------------
;; UInt  

entry = 'testce_uint'

p1 = 1U
p2 = [2U,3U]
p3 = 4U

print, 'Starting test Uint'
ret12 = call_external(image, entry, p1, p2, p3, value=[0,0,1], return_type=12)

if (    size(ret12, /type) ne 12	$
        or  ret12 ne 14U            	$
        or  p1  ne 65535U            	$
        or  p2[0] ne 12U          	$
        or  p2[1] ne 13U          	$
        or  p3    ne 4U           	$
   ) then begin
    print, 'back in IDL: Error:'
    print, 'Return value:  Got: ', ret12
    print, '          Expected: ', 14U
    print, 'First arg:     Got: ', p1
    print, '          Expected: ', 65535U
    print, 'Second arg     Got: ', p2
    print, '          Expected: ', [12U, 13U]
    print, 'Third arg      Got: ', p3
    print, '          Expected: ', 4U
    error = error + 1
    ;;stop
endif else begin
    print, 'back in IDL: OK'
    print
endelse

;; --------------------------------------------------------------------------
;; ULong 

entry = 'testce_ulong'

p1 = 1UL
p2 = [2UL,3UL]
p3 = 4UL

print, 'Starting test Ulong'
ret13 = call_external(image, entry, p1, p2, p3, value=[0,0,1], return_type=13)

if (    size(ret13, /type) ne 13	$
        or  ret13 ne 14UL           	$
        or  p1  ne 4294967295UL           	$
        or  p2[0] ne 12UL         	$
        or  p2[1] ne 13UL         	$
        or  p3    ne 4UL          	$
   ) then begin
    print, 'back in IDL: Error:'
    print, 'Return value:  Got: ', ret13
    print, '          Expected: ', 14UL
    print, 'First arg:     Got: ', p1
    print, '          Expected: ', 4294967295UL
    print, 'Second arg     Got: ', p2
    print, '          Expected: ', [12UL, 13UL]
    print, 'Third arg      Got: ', p3
    print, '          Expected: ', 4UL
    error = error + 1
    ;;stop
endif else begin
    print, 'back in IDL: OK'
    print
endelse


;; --------------------------------------------------------------------------
;; Long64 

entry = 'testce_long64'

p1 = 1LL
p2 = [2LL,3LL]

print, 'Starting test Long64'
ret14 = call_external(image, entry, p1, p2, return_type=14)

if (    size(ret14, /type) ne 14            $
        or  ret14 ne 14LL         	$
        or  p1  ne -11LL           	$
        or  p2[0] ne 12LL         	$
        or  p2[1] ne 13LL         	$
   ) then begin
    print, 'back in IDL: Error:'
    print, 'Return value:  Got: ', ret14
    print, '          Expected: ', 14LL
    print, 'First arg:     Got: ', p1
    print, '          Expected: ', -11LL
    print, 'Second arg     Got: ', p2
    print, '          Expected: ', [12LL, 13LL]
    error = error + 1
    ;;stop
endif else begin
    print, 'back in IDL: OK'
    print
endelse



;; --------------------------------------------------------------------------
;; ULong64 

entry = 'testce_ulong64'

p1 = 1ULL
p2 = [2ULL,3ULL]

print, 'Starting test Long64'
ret15 = call_external(image, entry, p1, p2, return_type=15)

if (    size(ret15, /type) ne 15            $
        or  ret15 ne 14ULL          	$
        or  p1  ne 18446744073709551615ULL           	$
        or  p2[0] ne 12ULL         	$
        or  p2[1] ne 13ULL         	$
   ) then begin
    print, 'back in IDL: Error:'
    print, 'Return value:  Got: ', ret15
    print, '          Expected: ', 14ULL
    print, 'First arg:     Got: ', p1
    print, '          Expected: ', 18446744073709551615ULL
    print, 'Second arg     Got: ', p2
    print, '          Expected: ', [12ULL, 13ULL]
    error = error + 1
    ;;stop
endif else begin
    print, 'back in IDL: OK'
    print
endelse



;; --------------------------------------------------------------------------
;; String 

entry = 'testce_string'

p1 = "One  "
p2 = ["Two  ", "Three"]
p3 = "Four "

c1 = "One  "

print, 'Starting test String'
ret7 = call_external(image, entry, p1, p2, p3, value=[0, 0, 1],  /s_value)

if (    size(ret7, /type) ne 7            $
        or  ret7 ne "Fourteen"  	$
        or  p1  ne "  enO"      	$
        or  p2[0] ne "  owT"    	$
        or  p2[1] ne "eerhT"    	$
   ) then begin
    print, 'back in IDL: Error:'
    print, 'Return value:  Got: ', ret7
    print, '          Expected: ', "Fourteen"
    print, 'First arg:     Got: ', p1
    print, '          Expected: ', "  enO"
    print, 'Second arg     Got: ', p2
    print, '          Expected: ', ["  owT", "eerhT"]
    error = error + 1
    ;;stop
endif else begin
    print, 'back in IDL: OK'
    if (c1 ne "One"+"  ") then begin
        print, "(But constant string changed as well: '", c1, "' insetad of 'One  ')"
    endif
    print
endelse

;; --------------------------------------------------------------------------
;; Struct

entry = 'testce_struct'

p1 = [ {outer, l1:1L, si:{inner, c:1B, d:2LL, s:["One", "Two"]}, l2:2L, c:2B}, $
       {outer, l1:3L, si:{inner, c:3B, d:4LL, s:["Thr", "Fou"]}, l2:4L, c:4B} ]

print, 'Starting test Stuct'
ret = call_external(image, entry, p1)

if (    p1[0].l1      ne 11          	$
        or  p1[1].si.s[1] ne "Fiv"		$
   ) then begin
    print, 'back in IDL: Error:'
endif else begin
    print, 'back in IDL: OK'
    print
endelse

;; --------------------------------------------------------------------------
;; Complex

entry = 'testce_complex'

p1 = complex(1.1, 2.2)
p2 = [complex(3.3,4.4), complex(5.5,6.6)]

print, 'Starting test Complex'
ret = call_external(image, entry, p1, p2, /f_value)

EPSILON = 1e-4

if (    size(ret, /type) ne 4	$
        or  ret ne 14.14			$
        or  abs( real_part(p1) - 1.1) gt EPSILON $
        or  abs( imaginary(p1) - 22.22) gt EPSILON $
        or  abs( real_part(p2[0]) - 3.3) gt EPSILON $
        or  abs( imaginary(p2[0]) - 4.4) gt EPSILON $
        or  abs( real_part(p2[1]) - 55.55) gt EPSILON $
        or  abs( imaginary(p2[1]) - 6.6) gt EPSILON $
   ) then begin
    print, 'back in IDL: Error:'
    print, 'Return value:  Got: ', ret
    print, '          Expected: ', 14.14
    print, 'First arg:     Got: ', p1
    print, '          Expected: ', complex(1.1, 22.22)
    print, 'Second arg     Got: ', p2
    print, '          Expected: ', [complex(3.3,4.4), complex(55.55, 6.6)]
    error = error + 1
    ;;stop
endif else begin
    print, 'back in IDL: OK'
    print
endelse


;; --------------------------------------------------------------------------
;; DComplex

entry = 'testce_dcomplex'

p1 = dcomplex(1.1d, 2.2d)
p2 = [dcomplex(3.3d,4.4d), dcomplex(5.5d,6.6d)]

print, 'Starting test DComplex'
ret = call_external(image, entry, p1, p2, /d_value)

EPSILON = 1d-8

if (    size(ret, /type) ne 5	$
        or  ret ne 14.14D			$
        or  abs( real_part(p1) - 1.1D) gt EPSILON $
        or  abs( imaginary(p1) - 22.22D) gt EPSILON $
        or  abs( real_part(p2[0]) - 3.3D) gt EPSILON $
        or  abs( imaginary(p2[0]) - 4.4D) gt EPSILON $
        or  abs( real_part(p2[1]) - 55.55D) gt EPSILON $
        or  abs( imaginary(p2[1]) - 6.6D) gt EPSILON $
   ) then begin
    print, 'back in IDL: Error:'
    print, 'Return value:  Got: ', ret
    print, '          Expected: ', 14.14D
    print, 'First arg:     Got: ', p1
    print, '          Expected: ', dcomplex(1.1D, 22.22D)
    print, 'Second arg     Got: ', p2
    print, '          Expected: ', [dcomplex(3.3D,4.4D), dcomplex(55.55D, 6.6D)]
    error = error + 1
    ;;stop
endif else begin
    print, 'back in IDL: OK'
    print
endelse


;; --------------------------------------------------------------------------
;; Long args by value

if (!version.memory_bits gt 32) then begin
    entry = 'testce_64'

    p2 = complex(1.1, 2.2)

    print, 'Starting test 64bit'
    ret = call_external(image, entry, 3.3D, p2, /d_value, value=[1,1])

    EPSILON = 1d-8

    if (    size(ret, /type) ne 5	$
            or  ret ne 15.15D			$
       ) then begin
        print, 'back in IDL: Error:'
        print, 'Return value:  Got: ', ret
        print, '          Expected: ', 15.15D
        error = error + 1
        ;;stop
    endif else begin
        print, 'back in IDL: OK'
        print
    endelse

endif else begin
    print, 'Skipping test on 32 bit system'
endelse

;; --------------------------------------------------------------------------
;; Some tests that should fail

print, '=================================================================='
print, 'The following tests SHOULD FAIL. The result will be caught,'
print, 'but error messages may be written to the console'
print

;; --------------------------------------------------------------------------
;; Non-existing image or entry

print, 'Non-existing image:'
image_nonsense = 'reallyNonsense'
r = execute( 'ret = call_external(image_nonsense, entry)' )
if (r eq 0) then begin
    print, 'Test failed as expected. OK!'
    print
endif else begin
    print, 'ERROR: Test did not fail.'
    error = error + 1
    ;;stop
endelse

print, 'Non-existing entry:'
entry_nonsense = 'reallyNonsense'
r = execute( 'ret = call_external(image, entry_nonsense)' )
if (r eq 0) then begin
    print, 'Test failed as expected. OK!'
    print
endif else begin
    print, 'ERROR: Test did not fail.'
    error = error + 1
    ;;stop
endelse

;; --------------------------------------------------------------------------
;; Illegal object for passing by value

entry = 'doNothing'
array_by_ref = [1,2,3]
array_by_val = [4,5,6]

print, 'Trying to pass non-scalar by value:'
r = execute( 'ret = call_external(image, entry, array_by_ref, array_by_val, value=[0,1])' )
if (r eq 0) then begin
    print, 'Test failed as expected. OK!'
    print
endif else begin
    print, 'ERROR: Test did not fail.'
    error = error + 1
    ;;stop
endelse


;; --------------------------------------------------------------------------
;; Too large object for passing by value

if (!version.memory_bits le 64) then begin
    entry = 'doNothing'
    dc = dcomplex(1D, 1D)

    print, 'Trying to pass  too large scalar by value:'
    r = execute( 'ret = call_external(image, entry, dc, /all_value )' )
    if (r eq 0) then begin
        print, 'Test failed as expected. OK!'
        print
    endif else begin
        print, 'ERROR: Test did not fail.'
        error = error + 1
        ;;stop
    endelse
endif else begin
    print, 'Test skipped , !version.memory_bits > 64'
endelse

;; --------------------------------------------------------------------------
;; Done

;; we go back to initial Dir.
CD, old_dir

;;
if (error eq 0) then begin
    print, "All tests OK"
endif else begin
    print, error, " tests failed\n"
    exit, status=1
endelse

end
