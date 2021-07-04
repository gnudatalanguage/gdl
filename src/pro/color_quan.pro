; small stub to be expanded.

function COLOR_QUAN, red, grn, blu, r, g, b , COLORS=COLORS , CUBE=CUBE, GET_TRANSLATION=GET_TRANSLATION, MAP_ALL=MAP_ALL, DITHER=DITHER, ERROR=ERROR, TRANSLATION=TRANSLATION
  print,"Warning, color_quant is a stub, please fixme"
  if n_params() eq 5 then begin
     blu=indgen(256)
     r=indgen(256)
     g=indgen(256)
     return, red
  endif
  if n_params() eq 6 then begin
     r=indgen(256)
     g=indgen(256)
     b=indgen(256)
     return,red     
  endif

end

  
