;
; AC, le 21/02/2006
;
; several tests around CONTOUR procedure
;
; We can jump to level 1 or level 2 tests
;
; I defined 3 levels: 
;  -level 0- basic (only Z)
;  -level 1- intermediate (Z, and (X,Y) 1D)
;  -level 2- high (z, x, Y are both 2D with same size)
;
; GDL 0.8.11
; Contour crashes even for basic call
;
; GDL 0.8.12 cvs
; Contour is OK for basic call
; Contour is OK when Z, X, Y provided AND Y > 0
; Contour is OK when Z, X, Y provided AND some Y are < 0
; Contour fails when Z, X and Y are all 2D arrays with same size
;
; GDL 0.9 All tests OK
;
pro TEST_CONTOUR_BASIC, nbp=nbp, level=level, test=test, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_CONTOUR_BASIC, nbp=nbp, level=level, test=test, help=help'
   return
endif
;
old_version=0
;
mess=''
if (N_ELEMENTS(nbp) NE 1) then nbp=128
;
if (N_ELEMENTS(level) EQ 0) then level=0
;
; we generate data, and X, Y
;
data=DIST(nbp)
x=FINDGEN(nbp)
y=FINDGEN(nbp)
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
print, 'Start of basic test (contour, z)'
;
contour, data
read, 'press enter to continue: ', mess
;
contour, data, /xstyle, title='/Xstyle enforced'
read, 'press enter to continue: ', mess
;
contour, data, /ystyle, title='/Ystyle enforced'
read, 'press enter to continue: ', mess
;
contour, data, /xstyle, /ystyle, title='/Xstyle and /Ystyle enforced'
read, 'press enter to continue: ', mess
;
contour, data, pos=pos_info, title='pos= used (no X or Y style ...)'
read, 'press enter to continue: ', mess
;
contour, data, pos=pos_info, /xstyle, /ystyle, title='pos= used, /Xstyle and /Ysstyle also'
read, 'press enter to continue: ', mess
;
; ----- intermediate tests ---------
level1:
print, 'Start of tests with intermediate difficulties (contour, z, x, y, with x and y 1D)'
;
contour, data, x, y
read, 'press enter to continue: ', mess
;
contour, data, x, y, /xstyle, title='X style enforced'
read, 'press enter to continue: ', mess
;
contour, data, x, y, /xstyle, /ystyle, title='X and Y style enforced'
read, 'press enter to continue: ', mess
;
contour, data, x, y,  pos=pos_info, title='pos= used'
read, 'press enter to continue: ', mess
;
contour, data, x*2, y,  title='X range expanded (*2)'
read, 'press enter to continue: ', mess
;
contour, data, x-nbp, y,  title='X range shifted (shifted to neg.)'
read, 'press enter to continue: ', mess
;
contour, data, x-nbp/2, y,  title='X range shifted (center on 0.)'
read, 'press enter to continue: ', mess
;
contour, data, x, y*2,  title='Y range expanded (*2)'
read, 'press enter to continue: ', mess
;
if (old_version EQ 1) then print, 'Warning: this should be wrong'
contour, data, x, y-nbp,  title='Y range shifted (shifted to neg.)'
read, 'press enter to continue: ', mess
;
if (old_version EQ 1) then print, 'Warning: this should be wrong'
contour, data, x, y-nbp/2,  title='Y range shifted (center on 0.)'
read, 'press enter to continue: ', mess
;
if (old_version EQ 1) then print, 'Warning: this should be wrong'
contour, data, x, y-nbp/2, pos=pos_info, title='Y range shifted (center on 0.) with pos='
read, 'press enter to continue: ', mess
;
if (old_version EQ 1) then print, 'Warning: this should be wrong'
contour, data, x, y-nbp/2, yrange=[-nbp, nbp], title='Y range shifted (center on 0.) with yrange'
read, 'press enter to continue: ', mess
;
if KEYWORD_SET(test) then STOP
;
; ----- high tests ---------
level2:
print, 'Start of tests with high difficulties (working since GDL 0.9)'
print, 'X, Y and Z and all 2D arrays with same size'
;
; we compute the locations on the grid (i.e. one (x,y) for each z)
x=x#replicate(1.,nbp)
y=transpose(x)
;
if (old_version EQ 1) then print, 'Warning: this should be not working ...'
contour, data, x, y
read, 'press enter to continue: ', mess
;
; the same but we add a noise on X positions
;
if (old_version EQ 1) then print, 'Warning: this should be not working ...'
contour, data, x+randomn(seed,nbp,nbp), y, /xstyle, /ystyle
read, 'press enter to continue: ', mess
;
print, 'test suite for CONTOUR is now finished'
;
if KEYWORD_SET(test) then STOP
;
end

