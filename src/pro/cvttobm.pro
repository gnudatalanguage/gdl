; converts a byte 'image' thresholded on 1 bit, to a 1 bit bitmap
; (B/W) for, e.g., bitmap buttons.

function cvttobm,image,threshold=t
    on_error, 2
    compile_opt idl2, hidden

    a=reverse(bytscl(image),2)
    ; check dims
    s=size(a)
    if s[0] ne 2 then message, "Input array is not a 2D array!"
    sx=s[1] & sy=s[2]

    ; scale bytes between 0 and 255
    ; if threshold is absent, threshold is mean
    if n_elements(t) eq 0 then begin
       t=total(a)/(sx*sy)
    endif
    
    ; to compress 8 bytes in 1 at the end,
    ; plunge into nearest multiple of 8
    ; size in x
    nx=(sx-1)/8+1
    nsx=8*nx
    b=bytarr(nsx,sy)
    
    ; JTappin's patch
    b[0, 0] = a ge t

  z = bytarr(nx, sy)
  isf = 1b
  for j = 0, 7 do begin
     z += isf*b[j:*:8, *]
     isf *= 2b
  endfor

  return, z
end

