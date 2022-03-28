pro test_numeric_limits, no_exit=no_exit, test=test
  errnum=0
  dotest=keyword_set(test)
  z=!NULL
; byte
  z=!NULL & a=execute("z='12'xb")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne 18 then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z='123'xb") ;error
  if a ne 0 then begin & errnum++ & if dotest then stop & end
  if z ne !NULL then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z=-127b") ;no error
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne 129 then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z=-1270b") ;error
  if a ne 0 then begin & errnum++ & if dotest then stop & end
  if z ne !NULL then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute('z="077b') ;ok
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute('z="0777b') ;ok
  if a ne 0 then begin & errnum++ & if dotest then stop & end
  if z ne !NULL then begin & errnum++ & if dotest then stop & end
; short
  z=!NULL & a=execute("z='7FFF'xs")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne 32767 then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z='8000'xs")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne -32768 then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z=-32767s")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne -32767 then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z=-32768s")
  if a ne 0 then begin & errnum++ & if dotest then stop & end
  if z ne !NULL then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z='80000'xs")
  if a ne 0 then begin & errnum++ & if dotest then stop & end
  if z ne !NULL then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z=123456789s")
  if a ne 0 then begin & errnum++ & if dotest then stop & end
  if z ne !NULL then begin & errnum++ & if dotest then stop & end
; unsigned short
  z=!NULL & a=execute("z='7FFF'xus")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne 32767 then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z='8000'xus")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne 32768 then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z=-32767us")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne 32769 then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z=-65535us")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne 1 then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z=-65536us")
  if a ne 0 then begin & errnum++ & if dotest then stop & end
  if z ne !NULL then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z='80000'xus")
  if a ne 0 then begin & errnum++ & if dotest then stop & end
  if z ne !NULL then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z=123456789us")
  if a ne 0 then begin & errnum++ & if dotest then stop & end
  if z ne !NULL then begin & errnum++ & if dotest then stop & end
; long
  z=!NULL & a=execute("z='7FFFFFFF'xl")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne 2147483647 then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z='80000000'xl")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne -2147483648 then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z=-2147483647l")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne -2147483647 then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z=-2147483648l")
  if a ne 0 then begin & errnum++ & if dotest then stop & end
  if z ne !NULL then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z='8000000000'xl")
  if a ne 0 then begin & errnum++ & if dotest then stop & end
  if z ne !NULL then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z=12345678999999999l")
  if a ne 0 then begin & errnum++ & if dotest then stop & end
  if z ne !NULL then begin & errnum++ & if dotest then stop & end
; unsigned long
  z=!NULL & a=execute("z='7FFFFFFF'xul")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne 2147483647 then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z='80000000'xul")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne 2147483648 then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z=-4294967295ul")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne 1 then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z=-4294967296ul")
  if a ne 0 then begin & errnum++ & if dotest then stop & end
  if z ne !NULL then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z='8000000000'xul")
  if a ne 0 then begin & errnum++ & if dotest then stop & end
  if z ne !NULL then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z=123456789976754ul")
  if a ne 0 then begin & errnum++ & if dotest then stop & end
  if z ne !NULL then begin & errnum++ & if dotest then stop & end
; long64
  z=!NULL & a=execute("z='7FFFFFFFFFFFFFFF'xll")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne 9223372036854775807 then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z='8000000000000000'xll")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne -9223372036854775808 then begin & errnum++ & if dotest then stop & endif ; !!!!!
  z=!NULL & a=execute("z=-9223372036854775808ll")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne -9223372036854775808 then begin & errnum++ & if dotest then stop & endif ; !!!!!
  z=!NULL & a=execute("z=-9223372036854775808"); promotion
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if ~(isa(z,"Long64")) then begin & errnum++ & if dotest then stop & end
  if z ne -9223372036854775808 then begin & errnum++ & if dotest then stop & endif ; !!!!!
  z=!NULL & a=execute("z=-92233720368547758089"); promotion
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if ~(isa(z,"Long64")) then begin & errnum++ & if dotest then stop & end
  if z ne 1 then begin & errnum++ & if dotest then stop & endif ; !!!!!
  z=!NULL & a=execute("z=92233720368547758089"); promotion
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if ~(isa(z,"Long64")) then begin & errnum++ & if dotest then stop & end
  if z ne -1 then begin & errnum++ & if dotest then stop & endif ; !!!!!
  z=!NULL & a=execute("z='8000000000000000'x") ; promotion
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if ~(isa(z,"Long64")) then begin & errnum++ & if dotest then stop & end
  if z ne -9223372036854775808ull then begin & errnum++ & if dotest then stop & endif ; !!!!
  z=!NULL & a=execute("z='80000000000000001'xll")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne -1 then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z='80000000000000001'x") ; promotion
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if ~(isa(z,"Long64")) then begin & errnum++ & if dotest then stop & end
  if z ne -1 then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z='7FFFFFFFFFFFFFFFFF'xll")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne -1 then begin & errnum++ & if dotest then stop & end
  
; ulong64
  z=!NULL & a=execute("z='8000000000000000'xull")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne 9223372036854775808ull then begin & errnum++ & if dotest then stop & endif ; !!!
  z=!NULL & a=execute("z='7FFFFFFFFFFFFFFF'xull")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne 9223372036854775807ull then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z='FFFFFFFFFFFFFFFF'xull")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne 18446744073709551615ull then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z='80000000000000000'xull")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne 18446744073709551615ull then begin & errnum++ & if dotest then stop & end
  z=!NULL & a=execute("z=92233720368547758099999ull")
  if a ne 1 then begin & errnum++ & if dotest then stop & end
  if z ne 18446744073709551615ull then begin & errnum++ & if dotest then stop & end
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_NUMERIC_LIMITS', errnum
;
if (errnum GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
