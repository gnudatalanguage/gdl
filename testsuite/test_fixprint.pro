; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
pro test_fixprint

  if ~execute('a=fix(0,type=0)') then begin
    message, 'fix(type=0) failed!', /conti
    exit, status=1
  endif

  if (fix('123',/print,type=1))[0] ne 123 then begin
    message, 'fix(string,/print,type=byte) failed!', /conti
    exit, status=1
  endif

  if (fix(123,/print,type=1))[0] ne 123 then begin
    message, 'fix(int,/print,type=byte) failed!', /conti
    exit, status=1
  endif

  if (fix('123',type=1))[0] ne 49 then begin
    message, 'fix(string,type=byte) failed!', /conti
    exit, status=1
  endif

  if fix(123b,/print,type=7) ne ' 123' then begin
    message, 'fix(byte,/print,type=string) failed!', /conti
    exit, status=1
  endif

  if fix(123b,type=7) ne '{' then begin
    message, 'fix(byte,type=string) failed!', /conti
    exit, status=1
  endif

end
