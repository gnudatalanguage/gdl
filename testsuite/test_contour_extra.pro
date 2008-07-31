;
; Alain C., 23 july 2008 during HFI Planck tests at IAS
;
; For CONTOUR, testing : Nlevels, Levels (list), /Fill
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
pro TEST_CONTOUR_NLEVELS, nlevels=nlevels, test=test, data=data
;
if N_ELEMENTS(nlevels) EQ 0 then nlevels=5
;
if (N_ELEMENTS(data) EQ 0) then data=VAGUES2D()
;
mess=''
;
CONTOUR, data, nlevels=nlevels
READ, 'press enter to continue: ', mess
;
CONTOUR, data, levels=FINDGEN(11)
READ, 'press enter to continue: ', mess
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------------------
;
pro TEST_CONTOUR_LEVELS, levels=levels, test=test, data=data
;
if N_ELEMENTS(nlevels) EQ 0 then nlevels=FINDGEN(10)
;
if (N_ELEMENTS(data) EQ 0) then data=VAGUES2D()
;
mess=''
;
CONTOUR, data, levels=levels
READ, 'press enter to continue: ', mess
;
CONTOUR, data, levels=[-1,0,1]
READ, 'press enter to continue: ', mess
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------------------
;
pro TEST_CONTOUR_FILL, nlevels=nlevels, test=test, data=data
;
mess=''
;
if (N_ELEMENTS(data) EQ 0) then data=VAGUES2D()
;
DEVICE, get_decomposed=get_dec
DEVICE, decomposed=0
LOADCT, 0
;
CONTOUR, data, /fill
READ, 'press enter to continue: ', mess
;
CONTOUR, data, /fill, /xstyle, /ystyle
READ, 'press enter to continue: ', mess
;
CONTOUR, data, /fill, nlevels=10
READ, 'press enter to continue: ', mess
;
CONTOUR, data, /fill, levels=[1,4,6,7]
READ, 'press enter to continue: ', mess
;
; Another Color Table !
;
LOADCT, 25
CONTOUR, data, /fil, nle=2, /xst,/yst
READ, 'press enter to continue: ', mess
CONTOUR, data, /fil, nle=12, /xst,/yst
READ, 'press enter to continue: ', mess
;
; Another Color Table !
;
LOADCT, 12
CONTOUR, data, /fil, nle=12, /xst,/yst
READ, 'press enter to continue: ', mess

DEVICE, decomposed=get_dec
LOADCT, 0

;GDL> contour, cos(a)#cos(a), nl=5,/fil
;GDL> contour, cos(a)#cos(a), nl=12,/fil
;GDL> contour, cos(a)#cos(a), nle=12,/fil
;GDL> contour, cos(a)#cos(a), nl=12,/fil
;GDL> contour, cos(a)#cos(a), le=[-.5,0,.5],/fil
;d GDL> contour, cos(a)#cos(a), lev=[-.5,0,.5],/fil
;
if KEYWORD_SET(test) then STOP
;
end

pro TEST_CONTOUR_NAN_INF, nlevels=nlevels, test=test
;
mess=''
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
READ, 'press enter to continue: ', mess
oplot, [25,25.], [0,128]
oplot, [0,128], [25,25.]


if N_ELEMENTS(nlevels) EQ 0 then nlevels=15

CONTOUR, data, nlevels=nlevels, title='Nlevels='+STRING(nlevels)
READ, 'press enter to continue: ', mess

CONTOUR, data_inf, nlevels=nlevels, title='Inf + Nlevels='+STRING(nlevels)
READ, 'press enter to continue: ', mess

CONTOUR, data_nan, nlevels=nlevels, title='NaN + Nlevels='+STRING(nlevels)
READ, 'press enter to continue: ', mess

if KEYWORD_SET(test) then STOP

end
