;
; hyper basic preliminary GDL version of WARP_TRI
; https://github.com/gnudatalanguage/gdl/issues/2222
;
function WARP_TRI, xo, yo, xi, yi, input_image, $
                   output_size = output_size, quintic = quintic, $
                   extrapolate = extra, tps=tps, test=test
;
; Some parameters are not ready in GDL
please=' not ready, please contribute'
if KEYWORD_SET(quintic) then MESSAGE, '/Quintic'+please
if KEYWORD_SET(extrapolate) then MESSAGE, '/Extra'+please
if KEYWORD_SET(tps) then MESSAGE, '/TPS'+please
;
;
s = SIZE(input_image)
if s[0] ne 2 then MESSAGE, 'WARP_TRI - <<INPUT_IMAGE>> must be 2D'

if N_ELEMENTS(output_size) ge 2 then begin
   nx = output_size[0]
   ny = output_size[1]
endif else begin
   nx = s[1]
   ny = s[2]
endelse
;
;Grid spacing
gs = [1,1]
;Bounds
b = [0,0, nx-1, ny-1]
;
TRIANGULATE, xo, yo, tr, bounds

triXi=TRIGRID(xo,yo,xi,tr, gs, b)
triYi=TRIGRID(xo,yo,yi,tr, gs, b)
;
res=INTERPOLATE( input_image, triXi, triYi)
;
if KEYWORD_SET(test) then  STOP
;
return, res
;
end

