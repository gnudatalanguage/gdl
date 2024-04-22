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
pro TEST_TVLCT
;
; direct switch into True Color mode
DEVICE, decomposed=0
;
nbp=256
x_pos=10
y_pos=500
offset=nbp
;
GENERATE_COLORS, nbp, r, g, b
;
WINDOW, 0, title='TVLCT, R,G,B', $
        xpos=x_pos, ypos=y_pos, xsize=nbp, ysize=nbp
TVLCT, r, g, b
TV, DIST(nbp)
;
; permutation
WINDOW, 1, title='TVLCT, B,R,G', $
        xpos=x_pos+offset, ypos=y_pos, xsize=nbp, ysize=nbp
TVLCT, b, r, g
TV, DIST(nbp)
;
; vectors
;
; reset to basic RGB
WINDOW, 2, title='TVLCT, [R, G, B]', $
        xpos=x_pos, ypos=y_pos-offset, xsize=nbp, ysize=nbp
TVLCT, [[r],[g],[b]]
TV, DIST(nbp)
;
; permutation
WINDOW, 3, title='TVLCT, [B, G, R]', $
        xpos=x_pos+offset, ypos=y_pos-offset, xsize=nbp, ysize=nbp
TVLCT, [[b],[r],[g]]
TV, DIST(nbp)
;
end
