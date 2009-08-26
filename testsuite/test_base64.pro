; by Sylwester Arabas <slayoo (at) igf.fuw.edu.pl>
pro test_base64

  ; empty string -> 0b
  if idl_base64('') ne 0b then  message, 'FAILED 01'

  ; random byte arrays of length from 1 to 10.000
  for c = 1l, 10000 do begin
    arr = bytarr(c, /nozero)
    if ~array_equal(idl_base64(idl_base64(arr)), arr) then begin 
      help, arr
      print, arr
      print, idl_base64(arr)
      print, idl_base64(idl_base64(arr))
      message, 'FAILED 02'
    endif
  endfor
 
end
