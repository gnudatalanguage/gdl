pro test_nestedloop
  for i=0, 0 do begin
    for j=0, 1 do begin
      if i eq 0 then continue
    endfor
    a = 1
  endfor
  if ~keyword_set(a) then begin
    exit, status=1
  endif
end
