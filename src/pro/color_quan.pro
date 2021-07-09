

function COLOR_QUAN5, image, dim, r, g, b , COLORS=NCOLORS , CUBE=CUBE, GET_TRANSLATION=GET_TRANSLATION, MAP_ALL=MAP_ALL, DITHER=DITHER, ERROR=ERROR, TRANSLATION=TRANSLATION
  compile_opt hidden, idl2
  ON_ERROR,2
  if (~KEYWORD_SET(ncolors)) then ncolors=252 < (!D.N_COLORS-1)
  ncolors = ncolors < 32 > 1
  tmp=image
  sz=size(tmp)
  if sz[0] ne 3 then Message,"Image is not TrueColor."
  t=[0,1,2]
  if (dim eq 2) then t=[1,0,2]
  if (dim eq 3) then t=[1,2,0]
  tmp=TRANSPOSE(tmp, t)
  
  mid=magick_create(sz[2],sz[3])
  magick_write,mid,tmp,/rgb
  MAGICK_QUANTIZE, mid, ncolors, dither=dither
  MAGICK_READCOLORMAPRGB,mid,r,g,b
  tmp=MAGICK_READ(mid)
  MAGICK_CLOSE, mid
  index=intarr(256)
  for i=0,n_elements(r)-1 do index[r[i]]=i
  tmp=reform(tmp[0,*,*])
  tmp[*]=index[tmp[*]]
  return,tmp
end

function COLOR_QUAN6, red, grn, blu, r, g, b , COLORS=COLORS , CUBE=CUBE, GET_TRANSLATION=GET_TRANSLATION, MAP_ALL=MAP_ALL, DITHER=DITHER, ERROR=ERROR, TRANSLATION=TRANSLATION
compile_opt hidden, idl2
ON_ERROR,2
  image=[[[red]],[[grn]],[[blu]]]
  return, COLOR_QUAN5(image, 1, r, g, b , COLORS=NCOLORS , CUBE=CUBE, GET_TRANSLATION=GET_TRANSLATION, MAP_ALL=MAP_ALL, DITHER=DITHER, ERROR=ERROR, TRANSLATION=TRANSLATION)
end

function  COLOR_QUAN, one, two, three, four, five, six , COLORS=COLORS , CUBE=CUBE, GET_TRANSLATION=GET_TRANSLATION, MAP_ALL=MAP_ALL, DITHER=DITHER, ERROR=ERROR, TRANSLATION=TRANSLATION
compile_opt hidden, idl2
ON_ERROR,2
  if n_params() eq 5 then return, COLOR_QUAN5( one, two, three, four, five,  COLORS=COLORS , CUBE=CUBE, GET_TRANSLATION=GET_TRANSLATION, MAP_ALL=MAP_ALL, DITHER=DITHER, ERROR=ERROR, TRANSLATION=TRANSLATION)
  if n_params() eq 6 then return, COLOR_QUAN6( one, two, three, four, five, six, COLORS=COLORS , CUBE=CUBE, GET_TRANSLATION=GET_TRANSLATION, MAP_ALL=MAP_ALL, DITHER=DITHER, ERROR=ERROR, TRANSLATION=TRANSLATION)
  message,"Incorrect number of arguments."
end


  
