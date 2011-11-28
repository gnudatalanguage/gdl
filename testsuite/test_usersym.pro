;
; Alain Coulais, 28/11/2011
; Help tracking bug related to USERSYM
;
pro INTERNAL_PLOT, nbp, title=title, psym=psym, NaN=NaN
;
if N_PARAMS() EQ 0 then nbp=2
;
x=FINDGEN(nbp)/nbp
y=x
;
if KEYWORD_SET(NaN) and (nbp GT 4) then begin
   x[4]=!values.f_nan
   x[6]=!values.f_nan
   x[7]=!values.f_nan
endif
;
plot, x,y, psym=psym, xrange=[-1,2], yrange=[-1,2], title=title
;
;print, '==========================================='
;
end
;
; ----------------------------------
;
pro TEST_USERSYM, psym=psym, help=help, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_USERSYM, psym=psym, help=help, test=test'
   return
end
;
if N_ELEMENTS(psym) EQ 0 then psym=8
;
UserSym, [-1, -0.5, -0.5, 0.5, 0.5, 1, -1], $
         [-0.7, -0.7, 0.7, 0.7, -0.7, -0.7, -0.7]
;
!p.multi=[0,2,2]
;
WINDOW, 0, title='Without NaN'
INTERNAL_PLOT, 1, psym=psym, title='1 point'
INTERNAL_PLOT, 2, psym=psym, title='2 point'
INTERNAL_PLOT, 3, psym=psym, title='3 point'
INTERNAL_PLOT, 12, psym=psym, title='12 point'
;
WINDOW, 1, title='With NaN'
INTERNAL_PLOT, 12, psym=psym, title='12 point', /nan
;
!p.multi=0
;
if KEYWORD_SET(test) then STOP
;
end
