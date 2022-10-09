pro rgbToCmyk,r, g, b, c, m, y, k
  max = BYTE(R > G > B)
  K = 255b - max ; min
  if (k eq 255) then begin
     C=255b
     M=255b
     Y=255b
     return
  endif
  delta = (255.0/max)
  C = 255b - BYTE(R*delta)
  M = 255b - BYTE(G*delta)
  Y = 255b - BYTE(B*delta)
end

pro cmyktorgb,c,m,y,k,r,g,b
  c=byte(c)
  m=byte(m)
  y=byte(y)
  k=byte(k)
   R = (255 - C) * (1 - K/255.)
   G = (255 - M) * (1 - K/255.)
   B = (255 - Y) * (1 - K/255.)
   r=byte(r)
   g=byte(g)
   b=byte(b)
end
pro cmyk_convert, c, m, y, k, r, g, b , to_cmyk=to_cmyk
  if keyword_set(to_cmyk) then rgbToCmyk,r, g, b, c, m, y, k else cmyktorgb,c,m,y,k,r,g,b
end
