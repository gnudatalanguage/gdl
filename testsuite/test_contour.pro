;
; Alain C., 23 july 2008 during HFI Planck tests at IAS
;
; For CONTOUR, testing : Nlevels, Levels (list), /Fill
;
; ---------------------------------
; 
; Modifications history :
;
; 2018-09-26 : Alain C.
; * Merging of "test_contour_basic.pro" & "test_contour_extra.pro"
; * Graphical tests, no value tested ...
; * Revised to run without stops ...
; ---------------------------------------------------
;
function VAGUES2D, nbp=nbp, x=x, y=y, help=help
;
if N_ELEMENTS(nbp) EQ 0 then nbp=128
;
indices=!pi/10.*FINDGEN(nbp)
; between -1. and 1.
data=COS(indices)#COS(indices)
; renormalisation between 0. and 10.
data=(data+1.)*5.
x=indices
y=x
;
return, data
end
;
; ---------------------------------------------------
;
pro STOP_OR_CONTINUE, delais=delais, enter=enter
;
if KEYWORD_SET(enter) then begin
   mess=''
   READ, 'press enter to continue: ', mess
endif
if KEYWORD_SET(delais) then WAIT, ABS(delais)
;
end
;
; ---------------------------------------------------
;
pro TEST_CONTOUR_NLEVELS, nlevels=nlevels, test=test, data=data, $
                          delais=delais, enter=enter
;
if N_ELEMENTS(nlevels) EQ 0 then nlevels=5
;
if (N_ELEMENTS(data) EQ 0) then data=VAGUES2D()
;
CONTOUR, data, nlevels=nlevels
STOP_OR_CONTINUE, delais=delais, enter=enter
;
CONTOUR, data, levels=FINDGEN(11)
STOP_OR_CONTINUE, delais=delais, enter=enter
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------------------
;
pro TEST_CONTOUR_LEVELS, levels=levels, test=test, data=data, $
                         delais=delais, enter=enter
;
if N_ELEMENTS(nlevels) EQ 0 then nlevels=FINDGEN(10)
;
if (N_ELEMENTS(data) EQ 0) then data=VAGUES2D()
;
CONTOUR, data, levels=levels
STOP_OR_CONTINUE, delais=delais, enter=enter
;
CONTOUR, data, levels=[-1,0,1]
STOP_OR_CONTINUE, delais=delais, enter=enter
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------------------
;
pro TEST_CONTOUR_FILL, nlevels=nlevels, test=test, data=data, $
                       delais=delais, enter=enter
;
if (N_ELEMENTS(data) EQ 0) then data=VAGUES2D()
;
DEVICE, get_decomposed=get_dec
DEVICE, decomposed=0
LOADCT, 0
;
CONTOUR, data, /fill
STOP_OR_CONTINUE, delais=delais, enter=enter
;
CONTOUR, data, /fill, /xstyle, /ystyle
STOP_OR_CONTINUE, delais=delais, enter=enter
;
CONTOUR, data, /fill, nlevels=10
STOP_OR_CONTINUE, delais=delais, enter=enter
;
CONTOUR, data, /fill, levels=[1,4,6,7]
STOP_OR_CONTINUE, delais=delais, enter=enter
;
; Another Color Table !
;
LOADCT, 25
CONTOUR, data, /fil, nle=2, /xst,/yst
STOP_OR_CONTINUE, delais=delais, enter=enter
CONTOUR, data, /fil, nle=12, /xst,/yst
STOP_OR_CONTINUE, delais=delais, enter=enter
;
; Another Color Table !
;
LOADCT, 12
CONTOUR, data, /fil, nle=12, /xst,/yst
STOP_OR_CONTINUE, delais=delais, enter=enter
;
DEVICE, decomposed=get_dec
LOADCT, 0
;
;GDL> contour, cos(a)#cos(a), le=[-.5,0,.5],/fil
;GDL> contour, cos(a)#cos(a), lev=[-.5,0,.5],/fil
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------------------
;
pro TEST_CONTOUR_NAN_INF, nlevels=nlevels, test=test, $
                          delais=delais, enter=enter
;
data=DIST(128)
;
data_inf=data
data_inf[4,4]=!values.f_infinity
;
data_nan=data
data_nan[4,4]=!values.f_nan
;
CONTOUR, data, nlevels=3, title='Nlevels='+STRING(3)
STOP_OR_CONTINUE, delais=delais, enter=enter
oplot, [25,25.], [0,128]
oplot, [0,128], [25,25.]
;
if N_ELEMENTS(nlevels) EQ 0 then nlevels=15
;
CONTOUR, data, nlevels=nlevels, title='Nlevels='+STRING(nlevels)
STOP_OR_CONTINUE, delais=delais, enter=enter
;
CONTOUR, data_inf, nlevels=nlevels, title='Inf + Nlevels='+STRING(nlevels)
STOP_OR_CONTINUE, delais=delais, enter=enter
;
CONTOUR, data_nan, nlevels=nlevels, title='NaN + Nlevels='+STRING(nlevels)
STOP_OR_CONTINUE, delais=delais, enter=enter
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------------------
;;
; AC, le 21/02/2006
; AC, le 20/03/2008 non square case ! more useful !
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
; CONTOUR crashes even for basic call
;
; GDL 0.8.12 cvs
; CONTOUR is OK for basic call
; CONTOUR is OK when Z, X, Y provided AND Y > 0
; CONTOUR is OK when Z, X, Y provided AND some Y are < 0
; CONTOUR fails when Z, X and Y are all 2D arrays with same size
;
; GDL 0.9 All tests OK
;
pro TEST_CONTOUR_BASIC, xnbp=xnbp, ynbp=ynbp, $
                        delais=delais, enter=enter, $
                        level=level, test=test, help=help
;
if KEYWORD_SET(help) then begin
   PRINT, 'pro TEST_CONTOUR_BASIC, xnbp=xnbp, ynbp=ynbp, $'
   PRINT, '                        level=level, test=test, help=help'
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
PRINT, 'Start of basic test (CONTOUR, z)'
;
CONTOUR, data
STOP_OR_CONTINUE, delais=delais, enter=enter
;
CONTOUR, data, /xstyle, title='/Xstyle enforced'
STOP_OR_CONTINUE, delais=delais, enter=enter
;
CONTOUR, data, /ystyle, title='/Ystyle enforced'
STOP_OR_CONTINUE, delais=delais, enter=enter
;
CONTOUR, data, /xstyle, /ystyle, title='/Xstyle and /Ystyle enforced'
STOP_OR_CONTINUE, delais=delais, enter=enter
;
CONTOUR, data, pos=pos_info, title='pos= used (no X or Y style ...)'
STOP_OR_CONTINUE, delais=delais, enter=enter
;
CONTOUR, data, pos=pos_info, /xstyle, /ystyle, title='pos= used, /Xstyle and /Ysstyle also'
STOP_OR_CONTINUE, delais=delais, enter=enter
;
; ----- intermediate tests ---------
level1:
PRINT, 'Start of tests with intermediate difficulties (CONTOUR, z, x, y, with x and y 1D)'
;
CONTOUR, data, x, y
STOP_OR_CONTINUE, delais=delais, enter=enter
;
CONTOUR, data, x, y, /xstyle, title='X style enforced'
STOP_OR_CONTINUE, delais=delais, enter=enter
;
CONTOUR, data, x, y, /xstyle, /ystyle, title='X and Y style enforced'
STOP_OR_CONTINUE, delais=delais, enter=enter
;
CONTOUR, data, x, y,  pos=pos_info, title='pos= used'
STOP_OR_CONTINUE, delais=delais, enter=enter
;
CONTOUR, data, x*2, y,  title='X range expanded (*2)'
STOP_OR_CONTINUE, delais=delais, enter=enter
;
CONTOUR, data, x-xnbp, y,  title='X range shifted (shifted to neg.)'
STOP_OR_CONTINUE, delais=delais, enter=enter
;
CONTOUR, data, x-xnbp/2, y,  title='X range shifted (center on 0.)'
STOP_OR_CONTINUE, delais=delais, enter=enter
;
CONTOUR, data, x, y*2,  title='Y range expanded (*2)'
STOP_OR_CONTINUE, delais=delais, enter=enter
;
if (old_version EQ 1) then PRINT, 'Warning: this should be wrong'
CONTOUR, data, x, y-ynbp,  title='Y range shifted (shifted to neg.)'
STOP_OR_CONTINUE, delais=delais, enter=enter
;
if (old_version EQ 1) then PRINT, 'Warning: this should be wrong'
CONTOUR, data, x, y-ynbp/2,  title='Y range shifted (center on 0.)'
STOP_OR_CONTINUE, delais=delais, enter=enter
;
if (old_version EQ 1) then PRINT, 'Warning: this should be wrong'
CONTOUR, data, x, y-ynbp/2, pos=pos_info, title='Y range shifted (center on 0.) with pos='
STOP_OR_CONTINUE, delais=delais, enter=enter
;
if (old_version EQ 1) then PRINT, 'Warning: this should be wrong'
CONTOUR, data, x, y-ynbp/2, yrange=[-ynbp, ynbp], title='Y range shifted (center on 0.) with yrange'
STOP_OR_CONTINUE, delais=delais, enter=enter
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
CONTOUR, data, x, y
STOP_OR_CONTINUE, delais=delais, enter=enter
;
; the same but we add a noise on X positions
;
if (old_version EQ 1) then PRINT, 'Warning: this should be not working ...'
CONTOUR, data, x+RANDOMN(seed, xnbp, ynbp), y, /xstyle, /ystyle
STOP_OR_CONTINUE, delais=delais, enter=enter
;
; the same but we add a noise on X positions
;
if (old_version EQ 1) then PRINT, 'Warning: this should be not working ...'
CONTOUR, data, x, y+RANDOMN(seed, xnbp, ynbp), /xstyle, /ystyle
STOP_OR_CONTINUE, delais=delais, enter=enter
;
PRINT, 'test suite for CONTOUR is now finished'
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------------------
;
pro TEST_CONTOUR, test=test, delais=delais, enter=enter
;
if ~KEYWORD_SET(delais) then delais=0.1
;
WINDOW, 0
;
TEST_CONTOUR_BASIC, test=test, delais=delais, enter=enter
;
TEST_CONTOUR_NLEVELS, test=test, delais=delais, enter=enter
TEST_CONTOUR_LEVELS, test=test, delais=delais, enter=enter
TEST_CONTOUR_FILL, test=test, delais=delais, enter=enter
TEST_CONTOUR_NAN_INF, test=test, delais=delais, enter=enter
;
WDELETE, 0
;
if KEYWORD_SET(test) then STOP
;
end
