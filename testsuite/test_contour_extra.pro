;
; Alain C., 23 july 2008 during HFI Planck tests at IAS
;
; For CONTOUR, testing : Nlevels, Levels (list), /Fill
;
; AC 2018-09-26 : Graphical tests, no velue tested ...
; Revised to run without stops ...
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
;
pro TEST_CONTOUR_EXTRA, test=test, delais=delais, enter=enter
;
if ~KEYWORD_SET(delais) then delais=0.1
;
WINDOW, 0
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
