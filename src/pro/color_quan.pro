

function COLOR_QUAN5, image, dim, r, g, b , COLORS=COLORS , CUBE=CUBE, GET_TRANSLATION=GET_TRANSLATION, MAP_ALL=MAP_ALL, DITHER=DITHER, ERROR=ERROR, TRANSLATION=TRANSLATION
  compile_opt hidden, idl2
  ON_ERROR,2
  if (~KEYWORD_SET(colors)) then colors=256
  ERROR=0.0001 ; fake
  GET_TRANSLATION=findgen(256) ; fake
  tmp=image
  sz=size(tmp)
  if sz[0] ne 3 then Message,"Image is not TrueColor."
  if (dim eq 2) then tmp=transpose(tmp,[1,0,2])
  if (dim eq 3) then tmp=transpose(tmp,[1,2,0])
  
  mid=magick_create(sz[2],sz[3])
  magick_write,mid,tmp,/rgb
  magick_flip,mid
  MAGICK_QUANTIZE, mid, colors, dither=dither
  indexed=(magick_class(mid) eq "PseudoClass") ; new command
  if (indexed) then begin
     tmp=MAGICK_READINDEXES(mid) ; may be [2,n,m] --> unsupported by IDL
     if ((size(tmp))[0] eq 3) then tmp=reform(tmp[0,*,*]) ; get only index image, not alpha channel
     MAGICK_READCOLORMAPRGB,mid,r,g,b,background_color=bgc
  endif else begin
     tmp=MAGICK_READ(mid)     ; NOT POSSIBLE BUT IN CASE: just read! [3,n,m] or [4,n,m]
  endelse
  MAGICK_CLOSE, mid
  return,tmp
end

function COLOR_QUAN6, red, grn, blu, r, g, b , COLORS=COLORS , CUBE=CUBE, GET_TRANSLATION=GET_TRANSLATION, MAP_ALL=MAP_ALL, DITHER=DITHER, ERROR=ERROR, TRANSLATION=TRANSLATION
compile_opt hidden, idl2
ON_ERROR,2
  image=[[[red]],[[grn]],[[blu]]]
  return, COLOR_QUAN5(image, 1, r, g, b , COLORS=COLORS , CUBE=CUBE, GET_TRANSLATION=GET_TRANSLATION, MAP_ALL=MAP_ALL, DITHER=DITHER, ERROR=ERROR, TRANSLATION=TRANSLATION)
end

function  COLOR_QUAN, one, two, three, four, five, six , COLORS=COLORS , CUBE=CUBE, GET_TRANSLATION=GET_TRANSLATION, MAP_ALL=MAP_ALL, DITHER=DITHER, ERROR=ERROR, TRANSLATION=TRANSLATION
compile_opt hidden, idl2
ON_ERROR,2
  if n_params() eq 5 then return, COLOR_QUAN5( one, two, three, four, five,  COLORS=COLORS , CUBE=CUBE, GET_TRANSLATION=GET_TRANSLATION, MAP_ALL=MAP_ALL, DITHER=DITHER, ERROR=ERROR, TRANSLATION=TRANSLATION)
  if n_params() eq 6 then return, COLOR_QUAN6( one, two, three, four, five, six, COLORS=COLORS , CUBE=CUBE, GET_TRANSLATION=GET_TRANSLATION, MAP_ALL=MAP_ALL, DITHER=DITHER, ERROR=ERROR, TRANSLATION=TRANSLATION)
  message,"Incorrect number of arguments."
end


  
