;
; AC, 05-Feb-2012
;
; Testing if TVLCT is working ...
;
pro GENERATE_COLORS, nbp, r, g, b
;
r=BYTSCL(SIN(FINDGEN(nbp)*!pi/nbp*10))
g=BYTSCL(SIN(FINDGEN(nbp)*!pi/nbp*30))
b=BINDGEN(nbp) 
;
end
;
pro TEST_TVSCL
;
nbp=256
;
GENERATE_COLORS, nbp, r, g, b
;
WINDOW, 0, xsize=nbp, ysize=nbp, title='TVLCT, R,G,B'
TVLCT, r, g, b
TV, DIST(nbp)
;
; permutation
WINDOW, 1, xsize=nbp, ysize=nbp, title='TVLCT, B,R,G'
TVLCT, b, r, g
TV, DIST(nbp)
;
; vectors
;
; reset to basic RGB
WINDOW, 2, xsize=nbp, ysize=nbp, title='TVLCT, [R, G, B]'
TVLCT, [[r],[g],[b]]
TV, DIST(nbp)
;
; permutation
WINDOW, 3, xsize=nbp, ysize=nbp, title='TVLCT, [B, G, R]'
TVLCT, [[b],[g],[r]]
TV, DIST(nbp)
;
end
