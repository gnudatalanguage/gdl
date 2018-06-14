; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
pro test_binfmt

  err = 0

  ; testing the '...'b syntax
  a = '101010'b
  if a ne 42 then begin
    message, '''101010''b != 42', /conti
    err++
  endif

  ; testing b|B|%b|%B input formats using READS
  fmt = ['(B)', '(b)', '(%"%b")', '(%"%6b")', '(%"%8B")', '(2b)', '(b6)', '(3B)', '(B8)']
  for i = 0, n_elements(fmt) - 1 do begin
    reads, '101010', a, format=fmt[i]
    if a ne 42 then begin
      message, 'reads(''101010'', format=' + fmt[i] + ') != 42', /conti
      err++
    endif
  endfor

  ; testing various output formats using diverse input
  sfx = ['', 'b', '', 'l', '.', 'd', '', '', '', '', '', '', 'u', 'ul', 'll', 'ull']
  a = [$
    ptr_new(-1), ptr_new('(b)'), ptr_new('1111111111111111'), $
    ptr_new(1l), ptr_new('(b032)'), ptr_new('00000000000000000000000000000001'), $
    ptr_new(1ll), ptr_new('(b1)'), ptr_new('1'), $
    ptr_new(-1ll), ptr_new('(b1)'), ptr_new('*'), $
    ptr_new(1), ptr_new('(b0)'), ptr_new('1'), $
    ptr_new(255b), ptr_new('(b)'), ptr_new('11111111'), $
    ptr_new(-255ll), ptr_new('(B)'), ptr_new('1111111111111111111111111111111111111111111111111111111100000001') $
  ]
  for i=0, n_elements(a) - 1, 3 do begin
    for prnt=0, 1 do begin
      if string(*a[i], format=*a[i+1], print=prnt) ne *a[i+2] then begin
        message, 'string(' + strtrim(string(*a[i]),2) + sfx[size(*a[i], /type)] + $
          ', format=''' + *a[i+1] + ''', print=' $
          + strtrim(string(prnt),2) + ') != ''' + *a[i+2] + ''' (''' + $
          string(*a[i], format=*a[i+1], print=prnt) + ''')', /conti
        err++
      endif
    endfor
    ptr_free, a[i], a[i+1], a[i+2]
  endfor

  ; summary - exiting with status=1 in case test(s) failed
  if err gt 0 then begin
    message, string(err) + ' test' + (err gt 1 ? 's' : '') + ' failed', /conti
    exit, status=1
  endif

end
