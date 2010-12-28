pro test_bug_1779553 

  err = 0
  i = 0
  n = 100
  v = ptrarr(n)
  f = strarr(n)
  a = strarr(n)

  ; OutFixed
  v[i] = ptr_new( 1) & f[i] = '(F020)' & a[i] = '001.0000000000000000' & ++i
  v[i] = ptr_new( 1) & f[i] = '(F20)'  & a[i] = '  1.0000000000000000' & ++i
  v[i] = ptr_new(-1) & f[i] = '(F020)' & a[i] = '-01.0000000000000000' & ++i
  v[i] = ptr_new(-1) & f[i] = '(F20)'  & a[i] = ' -1.0000000000000000' & ++i
  v[i] = ptr_new( 0) & f[i] = '(F020)' & a[i] = '000.0000000000000000' & ++i
  v[i] = ptr_new( 0) & f[i] = '(F20)'  & a[i] = '  0.0000000000000000' & ++i

  ; OutScientific
  v[i] = ptr_new( 1) & f[i] = '(E025)' & a[i] = '0001.0000000000000000E+00' & ++i
  v[i] = ptr_new( 1) & f[i] = '(E25)'  & a[i] = '   1.0000000000000000E+00' & ++i
  v[i] = ptr_new(-1) & f[i] = '(E025)' & a[i] = '-001.0000000000000000E+00' & ++i
  v[i] = ptr_new(-1) & f[i] = '(E25)'  & a[i] = '  -1.0000000000000000E+00' & ++i
  v[i] = ptr_new( 0) & f[i] = '(E025)' & a[i] = '0000.0000000000000000E+00' & ++i
  v[i] = ptr_new( 0) & f[i] = '(E25)'  & a[i] = '   0.0000000000000000E+00' & ++i

  ; OutAuto
  v[i] = ptr_new( 1) & f[i] = '(G025)' & a[i] = '000000001.000000000000000' & ++i
  v[i] = ptr_new( 1) & f[i] = '(G25)'  & a[i] = '        1.000000000000000' & ++i
  v[i] = ptr_new(-1) & f[i] = '(G025)' & a[i] = '-00000001.000000000000000' & ++i
  v[i] = ptr_new(-1) & f[i] = '(G25)'  & a[i] = '       -1.000000000000000' & ++i
  v[i] = ptr_new( 0) & f[i] = '(G025)' & a[i] = '000000000.000000000000000' & ++i
  v[i] = ptr_new( 0) & f[i] = '(G25)'  & a[i] = '        0.000000000000000' & ++i

  ; OFmtI (DEC, HEX, BIN, OCT checked with C formats)
  v[i] = ptr_new( 300) & f[i] = '(I8)'    & a[i] = '     300' & ++i
  v[i] = ptr_new( 300) & f[i] = '(I08)'   & a[i] = '00000300' & ++i
  v[i] = ptr_new(-300) & f[i] = '(I08)'   & a[i] = '-0000300' & ++i
  v[i] = ptr_new( 300) & f[i] = '(I08.2)' & a[i] = '     300' & ++i
  v[i] = ptr_new( 300) & f[i] = '(I08.5)' & a[i] = '   00300' & ++i
  v[i] = ptr_new(-300) & f[i] = '(I08.5)' & a[i] = '  -00300' & ++i
  v[i] = ptr_new(-300) & f[i] = '(I8.05)' & a[i] = '  -00300' & ++i
  v[i] = ptr_new( 300) & f[i] = '(I8.8)'  & a[i] = '00000300' & ++i
  v[i] = ptr_new(-300) & f[i] = '(I8.8)'  & a[i] = '********' & ++i

  ; OFmtA
  v[i] = ptr_new(-3) & f[i] = '(A3)' & a[i] = '   '      & ++i
  v[i] = ptr_new(-3) & f[i] = '(A8)' & a[i] = '      -3' & ++i

  ; C formats
  v[i] = ptr_new(  300) & f[i] = '(%"%05d")'   & a[i] = '00300'     & ++i
  v[i] = ptr_new( -300) & f[i] = '(%"%05d")'   & a[i] =  '-0300'    & ++i
  v[i] = ptr_new(  300) & f[i] = '(%"%5d")'    & a[i] = '  300'     & ++i
  v[i] = ptr_new(  300) & f[i] = '(%"%07.1f")' & a[i] = '00300.0'   & ++i
  v[i] = ptr_new(  300) & f[i] = '(%"%08.1E")' & a[i] = '03.0E+02'  & ++i
  v[i] = ptr_new(  300) & f[i] = '(%"%08i")'   & a[i] = '00000300'  & ++i
  v[i] = ptr_new(  300) & f[i] = '(%"%08.4i")' & a[i] = '    0300'  & ++i
  v[i] = ptr_new(  300) & f[i] = '(%"%08.2g")' & a[i] = '03.0e+02'  & ++i
  v[i] = ptr_new( -300) & f[i] = '(%"%09.2g")' & a[i] = '-03.0e+02' & ++i
  v[i] = ptr_new(    3) & f[i] = '(%"%09o")'   & a[i] = '000000003' & ++i
  v[i] = ptr_new(   3l) & f[i] = '(%"%09o")'   & a[i] = '000000003' & ++i
  v[i] = ptr_new(  3ul) & f[i] = '(%"%09o")'   & a[i] = '000000003' & ++i
  v[i] = ptr_new(  3ll) & f[i] = '(%"%09o")'   & a[i] = '000000003' & ++i
  v[i] = ptr_new( 3ull) & f[i] = '(%"%09o")'   & a[i] = '000000003' & ++i
  v[i] = ptr_new(    3) & f[i] = '(%"%9o")'    & a[i] = '        3' & ++i
  v[i] = ptr_new(   -3) & f[i] = '(%"%09s")'   & a[i] = '       -3' & ++i
  v[i] = ptr_new(  11b) & f[i] = '(%"%3x")'    & a[i] = '  B'       & ++i
  v[i] = ptr_new(   11) & f[i] = '(%"%3x")'    & a[i] = '  B'       & ++i
  v[i] = ptr_new(  11l) & f[i] = '(%"%3x")'    & a[i] = '  B'       & ++i
  v[i] = ptr_new( 11ul) & f[i] = '(%"%3x")'    & a[i] = '  B'       & ++i
  v[i] = ptr_new( 11ll) & f[i] = '(%"%3x")'    & a[i] = '  B'       & ++i
  v[i] = ptr_new(11ull) & f[i] = '(%"%3x")'    & a[i] = '  B'       & ++i
  v[i] = ptr_new(  11b) & f[i] = '(%"%03x")'   & a[i] = '00B'       & ++i
  v[i] = ptr_new(   11) & f[i] = '(%"%03x")'   & a[i] = '00B'       & ++i
  v[i] = ptr_new(  11l) & f[i] = '(%"%03x")'   & a[i] = '00B'       & ++i
  v[i] = ptr_new( 11ul) & f[i] = '(%"%03x")'   & a[i] = '00B'       & ++i
  v[i] = ptr_new( 11ll) & f[i] = '(%"%03x")'   & a[i] = '00B'       & ++i
  v[i] = ptr_new(11ull) & f[i] = '(%"%03x")'   & a[i] = '00B'       & ++i

  ; complex/integer
  v[i] = ptr_new(complex(-1,-1))    & f[i] = '(2I)'     & a[i] = '          -1          -1' & ++i
  v[i] = ptr_new(complex(-1,-1))    & f[i] = '(2I2)'    & a[i] = '-1-1'                     & ++i
  v[i] = ptr_new(complex(-1,-1))    & f[i] = '(2I03)'   & a[i] = '-01-01'                   & ++i
  v[i] = ptr_new(complex(-1.2,-1))  & f[i] = '(2I5)'    & a[i] = '   -1   -1'               & ++i
  v[i] = ptr_new(complex(-1.2,-1))  & f[i] = '(2I05)'   & a[i] = '-0001-0001'               & ++i
  v[i] = ptr_new(dcomplex(-1,-1))   & f[i] = '(2I)'     & a[i] = '          -1          -1' & ++i
  v[i] = ptr_new(dcomplex(-1,-1))   & f[i] = '(2I2)'    & a[i] = '-1-1'                     & ++i
  v[i] = ptr_new(dcomplex(-1,-1))   & f[i] = '(2I03)'   & a[i] = '-01-01'                   & ++i
  v[i] = ptr_new(dcomplex(-1.2,-1)) & f[i] = '(2I05)'   & a[i] = '-0001-0001'               & ++i
  v[i] = ptr_new(dcomplex(-1.2,-1)) & f[i] = '(2I05.3)' & a[i] = ' -001 -001'               & ++i
  v[i] = ptr_new(complex(8,8))      & f[i] = '(2O03)'   & a[i] = '010010'                   & ++i
  v[i] = ptr_new(dcomplex(8,8))     & f[i] = '(2O03)'   & a[i] = '010010'                   & ++i
  v[i] = ptr_new(complex(10,10))    & f[i] = '(2Z03)'   & a[i] = '00A00A'                   & ++i
  v[i] = ptr_new(dcomplex(10,10))   & f[i] = '(2z03)'   & a[i] = '00a00a'                   & ++i

  ; struct
  v[i] = ptr_new({a:1}) & f[i] = '(I05)' & a[i] = '00001' & ++i

  n = i
  for i = 0, n - 1 do begin
    if string(*v[i], format=f[i]) ne a[i] then begin
      message, 'string(' + strtrim(string(*v[i]), 2) + ', format=''' + f[i] + ''') = ''' + string(*v[i], format=f[i]) + ''' != ''' + a[i] + '''', /conti
      ++err
    endif
  endfor

  if err ne 0 then exit, status=1
  
end
