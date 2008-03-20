;
; AC, le 19/03/2008 derivated from TEST_CONTOUR_BASIC
; AC, le 20/03/2008 non square case ! more useful !
;
; several tests around SURFACE procedure
;
; We can jump to level 1 or level 2 tests
;
; I defined 3 levels: 
;  -level 0- basic (only Z)
;  -level 1- intermediate (Z, and (X,Y) 1D)
;  -level 2- high (z, x, Y are both 2D with same size)
;
; GDL 0.9 All tests OK
;
pro TEST_SURFACE_BASIC, xnbp=xnbp, ynbp=ynbp, $
                        level=level, test=test, help=help
;
if KEYWORD_SET(help) then begin
   PRINT, 'pro TEST_SURFACE_BASIC, xnbp=xnbp, ynbp=ynbp, $'
   PRINT, '              level=level, test=test, help=help'
   return
endif
;
old_version=0
;
mess=''
;
if (N_ELEMENTS(level) EQ 0) then level=0
;
; we generate data, and X, Y
;
if (N_ELEMENTS(xnbp) NE 1) then xnbp=128
if (N_ELEMENTS(ynbp) NE 1) then ynbp=2*xnbp
;
data=DIST(xnbp, ynbp)
x=FINDGEN(xnbp)
y=FINDGEN(ynbp)
;
pos_info=[0.2,0.2,0.8,0.8]
;
; we jump if we check only from specific level (0,1,2)
;
if (level EQ 1) then goto, level1
if (level EQ 2) then goto, level2
;
; ----- basic tests ---------
level0:
PRINT, 'Start of basic test (SURFACE, z)'
;
SURFACE, data
READ, 'press enter to continue: ', mess
;
SURFACE, data, /xstyle, title='/Xstyle enforced'
READ, 'press enter to continue: ', mess
;
SURFACE, data, /ystyle, title='/Ystyle enforced'
READ, 'press enter to continue: ', mess
;
SURFACE, data, /xstyle, /ystyle, title='/Xstyle and /Ystyle enforced'
READ, 'press enter to continue: ', mess
;
SURFACE, data, pos=pos_info, title='pos= used (no X or Y style ...)'
READ, 'press enter to continue: ', mess
;
SURFACE, data, pos=pos_info, /xstyle, /ystyle, title='pos= used, /Xstyle and /Ysstyle also'
READ, 'press enter to continue: ', mess
;
; ----- intermediate tests ---------
level1:
PRINT, 'Start of tests with intermediate difficulties (SURFACE, z, x, y, with x and y 1D)'
;
SURFACE, data, x, y
READ, 'press enter to continue: ', mess
;
SURFACE, data, x, y, /xstyle, title='X style enforced'
READ, 'press enter to continue: ', mess
;
SURFACE, data, x, y, /xstyle, /ystyle, title='X and Y style enforced'
READ, 'press enter to continue: ', mess
;
SURFACE, data, x, y,  pos=pos_info, title='pos= used'
READ, 'press enter to continue: ', mess
;
SURFACE, data, x*2, y,  title='X range expanded (*2)'
READ, 'press enter to continue: ', mess
;
SURFACE, data, x-xnbp, y,  title='X range shifted (shifted to neg.)'
READ, 'press enter to continue: ', mess
;
SURFACE, data, x-xnbp/2, y,  title='X range shifted (center on 0.)'
READ, 'press enter to continue: ', mess
;
SURFACE, data, x, y*2,  title='Y range expanded (*2)'
READ, 'press enter to continue: ', mess
;
if (old_version EQ 1) then PRINT, 'Warning: this should be wrong'
SURFACE, data, x, y-ynbp,  title='Y range shifted (shifted to neg.)'
READ, 'press enter to continue: ', mess
;
if (old_version EQ 1) then PRINT, 'Warning: this should be wrong'
SURFACE, data, x, y-ynbp/2,  title='Y range shifted (center on 0.)'
READ, 'press enter to continue: ', mess
;
if (old_version EQ 1) then PRINT, 'Warning: this should be wrong'
SURFACE, data, x, y-ynbp/2, pos=pos_info, title='Y range shifted (center on 0.) with pos='
READ, 'press enter to continue: ', mess
;
if (old_version EQ 1) then PRINT, 'Warning: this should be wrong'
SURFACE, data, x, y-ynbp/2, yrange=[-ynbp, ynbp], title='Y range shifted (center on 0.) with yrange'
READ, 'press enter to continue: ', mess
;
if KEYWORD_SET(test) then STOP
;
; ----- high tests ---------
level2:
PRINT, 'Start of tests with high difficulties (working since GDL 0.9)'
PRINT, 'X, Y and Z and all 2D arrays with same size'
;
; we compute the locations on the grid (i.e. one (x,y) for each z)
x=x#REPLICATE(1.,ynbp)
y=REPLICATE(1.,xnbp)#y
;
if (old_version EQ 1) then PRINT, 'Warning: this should be not working ...'
SURFACE, data, x, y
READ, 'press enter to continue: ', mess
;
; the same but we add a noise on X positions
;
if (old_version EQ 1) then PRINT, 'Warning: this should be not working ...'
SURFACE, data, x+RANDOMN(seed, xnbp, ynbp), y, /xstyle, /ystyle
READ, 'press enter to continue: ', mess
;
; the same but we add a noise on Y positions
;
if (old_version EQ 1) then PRINT, 'Warning: this should be not working ...'
SURFACE, data, x, y+RANDOMN(seed, xnbp, ynbp), /xstyle, /ystyle
READ, 'press enter to continue: ', mess
;
PRINT, 'test suite for CONTOUR is now finished'
;
if KEYWORD_SET(test) then STOP
;
end

