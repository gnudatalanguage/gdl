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
    w=where(a ge t, na)
    if na ne 0 then b[w]=1b
    ; compress 8 bytes in 1:
    z=bytarr(nx*sy)
    
    v=[[1],[2],[4],[8],[16],[32],[64],[128]]
    for i=0,n_elements(b)-1,8 do begin
       x=b[i:i+7]
       z[i/8]=byte(x##v)
    endfor
    return,reform(z,nx,sy)
end

