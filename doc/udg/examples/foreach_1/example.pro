pro example
  letters = ['a', 'b', 'c', 'd', 'e']
  foreach l, letters do begin
    if l eq 'c' then continue
    if l eq 'd' then break
    print, 'trying to replace '+ l + ' with ''x'''
    l = 'x'
  endforeach
  print, letters
  print, l
end
